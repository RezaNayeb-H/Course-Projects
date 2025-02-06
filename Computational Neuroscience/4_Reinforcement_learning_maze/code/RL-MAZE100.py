import numpy as np
import random
import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
import matplotlib.pyplot as plt
import imageio
import os
from collections import deque

# Environment Class
class Environment:
    def __init__(self, maze_file):
        self.maze = np.load(maze_file)
        self.ny, self.nx = self.maze.shape
        self.start = (0, 0)
        self.goal = (self.nx - 1, self.ny - 1)
        self.actions = [(0, -1), (1, 0), (0, 1), (-1, 0)]  # Up, Right, Down, Left
        self.output_dir = os.path.join(os.path.dirname(maze_file), "output_dqn")
        os.makedirs(self.output_dir, exist_ok=True)

    def get_valid_actions(self, x, y):
        """Returns a list of valid actions for a given state."""
        valid_actions = []
        for i, (dx, dy) in enumerate(self.actions):
            nx_, ny_ = x + dx, y + dy
            if 0 <= nx_ < self.nx and 0 <= ny_ < self.ny and self.maze[ny_, nx_] == 0:
                valid_actions.append(i)
        return valid_actions

    def get_reward(self, x, y):
        """Returns the reward for a given state."""
        return 100 if (x, y) == self.goal else -1

    def is_goal(self, x, y):
        """Checks if the given state is the goal."""
        return (x, y) == self.goal

# Deep Q-Network (DQN) Model
class DQN(nn.Module):
    def __init__(self, input_dim, output_dim):
        super(DQN, self).__init__()
        self.fc1 = nn.Linear(input_dim, 128)
        self.fc2 = nn.Linear(128, 128)
        self.fc3 = nn.Linear(128, output_dim)

    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        return self.fc3(x)

# DQN Agent
class DQNAgent:
    def __init__(self, env, gamma=0.9, lr=0.001, batch_size=64, memory_size=50000):
        self.env = env
        self.gamma = gamma
        self.batch_size = batch_size
        self.epsilon = 1.0
        self.epsilon_decay = 0.995
        self.epsilon_min = 0.01
        self.memory = deque(maxlen=memory_size)

        self.model = DQN(2, len(env.actions))
        self.target_model = DQN(2, len(env.actions))
        self.target_model.load_state_dict(self.model.state_dict())
        self.optimizer = optim.Adam(self.model.parameters(), lr=lr)

    def get_state(self, x, y):
        """Encodes state as a tensor."""
        return torch.tensor([x / self.env.nx, y / self.env.ny], dtype=torch.float32)

    def choose_action(self, x, y):
        """Epsilon-greedy policy."""
        if random.random() < self.epsilon:
            return random.choice(self.env.get_valid_actions(x, y))
        else:
            state = self.get_state(x, y).unsqueeze(0)
            q_values = self.model(state)
            valid_actions = self.env.get_valid_actions(x, y)
            return max(valid_actions, key=lambda a: q_values[0, a].item())

    def store_experience(self, state, action, reward, next_state, done):
        """Stores experience in replay memory."""
        self.memory.append((state, action, reward, next_state, done))

    def train(self):
        """Trains the model using replay memory."""
        if len(self.memory) < self.batch_size:
            return

        batch = random.sample(self.memory, self.batch_size)
        states, actions, rewards, next_states, dones = zip(*batch)

        states = torch.stack(states)
        next_states = torch.stack(next_states)
        actions = torch.tensor(actions, dtype=torch.long)
        rewards = torch.tensor(rewards, dtype=torch.float32)
        dones = torch.tensor(dones, dtype=torch.float32)

        q_values = self.model(states).gather(1, actions.unsqueeze(1)).squeeze(1)
        next_q_values = self.target_model(next_states).max(1)[0].detach()
        target_q_values = rewards + self.gamma * next_q_values * (1 - dones)

        loss = F.mse_loss(q_values, target_q_values)
        self.optimizer.zero_grad()
        loss.backward()
        self.optimizer.step()

    def update_target_network(self):
        """Updates target network weights."""
        self.target_model.load_state_dict(self.model.state_dict())

    def train_agent(self, num_episodes, update_target_every=50, frame_time=5.0, d_episode=50):
        """Trains the DQN agent and creates a GIF."""
        frames = []
        for episode in range(num_episodes):
            x, y = self.env.start
            total_reward = 0
            state = self.get_state(x, y)

            while not self.env.is_goal(x, y):
                action = self.choose_action(x, y)
                dx, dy = self.env.actions[action]
                new_x, new_y = x + dx, y + dy

                reward = self.env.get_reward(new_x, new_y)
                next_state = self.get_state(new_x, new_y)
                done = self.env.is_goal(new_x, new_y)

                self.store_experience(state, action, reward, next_state, done)
                self.train()

                x, y = new_x, new_y
                state = next_state
                total_reward += reward

            self.epsilon = max(self.epsilon * self.epsilon_decay, self.epsilon_min)

            if episode % update_target_every == 0:
                self.update_target_network()

            if episode % d_episode == 0:
                save_path = os.path.join(self.env.output_dir, f"policy_{episode}.png")
                self.visualize_policy(save_path)
                frames.append(imageio.imread(save_path))

            print(f"Episode {episode}, Total Reward: {total_reward}, Epsilon: {self.epsilon:.3f}")

        gif_path = os.path.join(self.env.output_dir, "policy_evolution.gif")
        imageio.mimsave(gif_path, frames, duration=frame_time*num_episodes/d_episode)
        print(f"GIF saved as {gif_path}")

    def visualize_policy(self, save_path=None):
        """Visualizes the current policy."""
        fig, ax = plt.subplots(figsize=(10, 10))
        ax.imshow(1 - self.env.maze, cmap="gray")

        for y in range(self.env.ny):
            for x in range(self.env.nx):
                if self.env.maze[y, x] == 0:
                    best_action = max(self.env.get_valid_actions(x, y), key=lambda a: self.model(self.get_state(x, y).unsqueeze(0))[0, a].item())
                    dx, dy = self.env.actions[best_action]
                    ax.arrow(x, y, dx * 0.3, dy * 0.3, head_width=0.2, head_length=0.2, fc="red", ec="red")

        if save_path:
            plt.savefig(save_path)
            plt.close()
        else:
            plt.show()

# Main Execution
if __name__ == "__main__":
    env = Environment('D:\\MyFiles\\Courses\\Term5\\Neural Computation\\Project\\proj4\\CHW4_files\\maze100.npy')
    agent = DQNAgent(env)
    agent.train_agent(num_episodes=5000)

    # Save final policy visualization
    final_policy_path = os.path.join(env.output_dir, "'D:\\MyFiles\\Courses\\Term5\\Neural Computation\\Project\\proj4\\CHW4_files\\output100\\final_policy.png")
    agent.visualize_policy(final_policy_path)
