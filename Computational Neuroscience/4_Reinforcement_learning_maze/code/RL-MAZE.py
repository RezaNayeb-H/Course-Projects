import numpy as np
import random
import matplotlib.pyplot as plt
import imageio
import os

class Environment:
    def __init__(self, maze_file):
        self.maze = np.load(maze_file)
        self.ny, self.nx = self.maze.shape
        self.start = (0, 0)
        self.goal = (self.nx - 1, self.ny - 1)
        self.maze_file = maze_file
        self.output_dir = os.path.join(os.path.dirname(maze_file), "output")
        os.makedirs(self.output_dir, exist_ok=True)  # Create output directory if it doesn't exist

    def get_valid_actions(self, x, y):
        """Returns a list of valid actions for a given state."""
        valid_actions = []
        actions = [(0, -1), (1, 0), (0, 1), (-1, 0)]  # Up, Right, Down, Left
        for i, (dx, dy) in enumerate(actions):
            nx_, ny_ = x + dx, y + dy
            if 0 <= nx_ < self.nx and 0 <= ny_ < self.ny and self.maze[ny_][nx_] == 0:
                valid_actions.append(i)
        return valid_actions

    def get_reward(self, x, y):
        """Returns the reward for a given state."""
        if (x, y) == self.goal:
            return 100  # Big reward for reaching the goal
        else:
            return -1  # Small penalty for each step

    def is_goal(self, x, y):
        """Checks if the given state is the goal."""
        return (x, y) == self.goal

    def visualize_maze(self):
        """Visualizes the maze."""
        plt.imshow(self.maze, cmap='gray')
        plt.title("Maze")
        plt.show()

class Agent:
    def __init__(self, env, alpha=0.1, gamma=0.9, epsilon=1.0, epsilon_decay=0.995, epsilon_min=0.01):
        self.env = env
        self.alpha = alpha  # Learning rate
        self.gamma = gamma  # Discount factor
        self.epsilon = epsilon  # Initial exploration rate
        self.epsilon_decay = epsilon_decay  # Decay per episode
        self.epsilon_min = epsilon_min  # Minimum epsilon value
        self.actions = [(0, -1), (1, 0), (0, 1), (-1, 0)]  # Up, Right, Down, Left
        self.num_actions = len(self.actions)
        self.Q_table = np.zeros((env.ny, env.nx, self.num_actions))  # Q-table

    def choose_action(self, x, y):
        """Epsilon-greedy policy."""
        if random.uniform(0, 1) < self.epsilon:
            return random.choice(self.env.get_valid_actions(x, y))  # Explore
        else:
            valid_actions = self.env.get_valid_actions(x, y)
            return max(valid_actions, key=lambda a: self.Q_table[y, x, a])  # Exploit

    def train(self, num_episodes):
        """Trains the agent using Q-learning."""
        for episode in range(num_episodes):
            x, y = self.env.start  # Start position
            total_reward = 0

            while not self.env.is_goal(x, y):  # Until reaching the goal
                action = self.choose_action(x, y)
                dx, dy = self.actions[action]
                new_x, new_y = x + dx, y + dy

                # Reward system
                reward = self.env.get_reward(new_x, new_y)

                # Q-value update
                best_next_action = max(self.env.get_valid_actions(new_x, new_y), key=lambda a: self.Q_table[new_y, new_x, a])
                self.Q_table[y, x, action] += self.alpha * (reward + self.gamma * self.Q_table[new_y, new_x, best_next_action] - self.Q_table[y, x, action])

                x, y = new_x, new_y
                total_reward += reward

            # Decay epsilon
            self.epsilon = max(self.epsilon * self.epsilon_decay, self.epsilon_min)

            if episode % 500 == 0:
                print(f"Episode {episode}, Total Reward: {total_reward}, Epsilon: {self.epsilon:.3f}")

    def solve_maze(self):
        """Uses the trained Q-table to solve the maze."""
        x, y = self.env.start
        path = [(x, y)]
        while not self.env.is_goal(x, y):
            action = max(self.env.get_valid_actions(x, y), key=lambda a: self.Q_table[y, x, a])
            dx, dy = self.actions[action]
            x, y = x + dx, y + dy
            path.append((x, y))
        return path

    def draw_policy(self, episode, save_path=None):
        """Visualizes the best policy at each cell using arrows.
        The optimal path (even if it leads to dead ends) is shown in green.
        """
        fig, ax = plt.subplots(figsize=(10, 10))
        ax.imshow(1 - self.env.maze, cmap="gray")  # Show maze

        arrow_map = {
            0: (0, -1),   # Up
            1: (1, 0),    # Right
            2: (0, 1),    # Down
            3: (-1, 0)    # Left
        }

        # Compute the best current path, even if not optimal (can hit dead ends)
        x, y = self.env.start
        visited = set()
        best_path = []

        while (x, y) not in visited and not self.env.is_goal(x, y):
            visited.add((x, y))
            best_path.append((x, y))

            valid_actions = self.env.get_valid_actions(x, y)
            if not valid_actions:  # If no valid moves, stop
                break
            
            best_action = max(valid_actions, key=lambda a: self.Q_table[y, x, a])
            dx, dy = arrow_map[best_action]
            x, y = x + dx, y + dy

        best_path_set = set(best_path)  # Convert to set for quick lookup

        # Draw policy arrows
        for y in range(self.env.ny):
            for x in range(self.env.nx):
                if self.env.maze[y, x] == 0:  # Ignore walls
                    valid_actions = self.env.get_valid_actions(x, y)
                    if valid_actions:  # If there are valid actions
                        best_action = max(valid_actions, key=lambda a: self.Q_table[y, x, a])
                        dx, dy = arrow_map[best_action]

                        color = 'green' if (x, y) in best_path_set else 'red'
                        ax.arrow(x, y, dx * 0.3, dy * 0.3, head_width=0.2, head_length=0.2, fc=color, ec=color)

        ax.set_title(f"Policy Visualization - Episode {episode}")

        if save_path:
            plt.savefig(save_path)
            plt.close(fig)  # Close the figure to avoid displaying it
        else:
            plt.show()


    def draw_optimal_path(self, path=None, save_path=None):
        """Draws the maze with the optimal path overlaid and optionally saves it as a PNG."""
        if path is None:
            path = self.solve_maze()  # Solve the maze if no path is provided

        fig, ax = plt.subplots(figsize=(10, 10))
        ax.imshow(1-self.env.maze, cmap="gray")  # Show maze

        # Extract X and Y coordinates from path
        x_coords, y_coords = zip(*path)

        # Plot the path with red line
        ax.plot(x_coords, y_coords, color='red', linewidth=2, marker='o', markersize=4, label="Optimal Path")

        # Mark start and goal positions
        ax.scatter([self.env.start[0]], [self.env.start[1]], color='green', s=100, label="Start (0,0)")
        ax.scatter([self.env.goal[0]], [self.env.goal[1]], color='blue', s=100, label=f"Goal ({self.env.goal[0]},{self.env.goal[1]})")

        ax.legend()
        plt.title("Optimal Path in the Maze")

        if save_path:
            plt.savefig(save_path)
            plt.close(fig)  # Close the figure to avoid displaying it
        else:
            plt.show()

    def train_with_visualization(self, num_episodes, frame_time, d_episode):
        """Trains the agent and saves a GIF of the policy evolution."""
        frames = []  # List to store frames

        for episode in range(num_episodes):
            x, y = self.env.start  # Start position
            total_reward = 0

            while not self.env.is_goal(x, y):  # Until reaching the goal
                action = self.choose_action(x, y)
                dx, dy = self.actions[action]
                new_x, new_y = x + dx, y + dy

                # Reward system
                reward = self.env.get_reward(new_x, new_y)

                # Q-value update
                best_next_action = max(self.env.get_valid_actions(new_x, new_y), key=lambda a: self.Q_table[new_y, new_x, a])
                self.Q_table[y, x, action] += self.alpha * (reward + self.gamma * self.Q_table[new_y, new_x, best_next_action] - self.Q_table[y, x, action])

                x, y = new_x, new_y
                total_reward += reward

            # Decay epsilon
            self.epsilon = max(self.epsilon * self.epsilon_decay, self.epsilon_min)

            # Save policy visualization at key episodes
            if episode % d_episode == 0 or episode == num_episodes - 1:
                save_path = os.path.join(self.env.output_dir, f"policy_episode_{episode}.png")
                self.draw_policy(episode, save_path)
                frames.append(imageio.imread(save_path))  # Store frame

            if episode % d_episode == 0:
                print(f"Episode {episode}, Total Reward: {total_reward}, Epsilon: {self.epsilon:.3f}")

        # Save GIF with longer duration between frames
        gif_path = os.path.join(self.env.output_dir, "policy_evolution.gif")
        imageio.mimsave(gif_path, frames, duration=frame_time*num_episodes/d_episode)
        print(f"GIF saved as {gif_path}")

        # Clean up temporary files
        for episode in range(0, num_episodes, d_episode):
            os.remove(os.path.join(self.env.output_dir, f"policy_episode_{episode}.png"))
        os.remove(os.path.join(self.env.output_dir, f"policy_episode_{num_episodes - 1}.png"))


# Main execution
if __name__ == "__main__":
    # Initialize environment and agent
    env = Environment('D:\\MyFiles\\Courses\\Term5\\Neural Computation\\Project\\proj4\\CHW4_files\\maze.npy')
    agent = Agent(env)

    # Train the agent with visualization
    agent.train_with_visualization(num_episodes=5000, frame_time=5.0, d_episode=25)

    # Solve the maze and visualize the optimal path
    solution_path = agent.solve_maze()
    
    # Save the optimal path visualization as a PNG file
    optimal_path_image_path = os.path.join(env.output_dir, "optimal_path.png")
    agent.draw_optimal_path(solution_path, save_path=optimal_path_image_path)
    print(f"Optimal path visualization saved as {optimal_path_image_path}")

    # Optionally, display the optimal path on the screen
    agent.draw_optimal_path(solution_path)