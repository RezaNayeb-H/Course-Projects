#ifndef _PIXEL_ZUMA_
#define _PIXEL_ZUMA_





/* includes are here */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include "mathHelper.hpp"
#include "BezierPath.hpp"
#include "SDLtools.hpp"

#include "menuclasses.hpp"
/* here we put some definitions for readability */
// this is the default radius size for the marble, the marbles radius probably shouldn't be changed anyways
#define RADI 20

// obvious
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const int SHOOTER_SIZE = 200;

// dont ask me how I found these numbers!
#define MARBLE_DISTANCE_FROM_BIRD 26
#define MARBLE_DISTANCE_ON_PATH 8

// this defined how many marble textures are in the game
#define MARBLE_TYPES_MAX 5

void WinFunc();
void LoseFunc();
/* here we place some ENUMS for better readability */
// this shows the marbles power type
enum MPOWER {NORMAL, BOMB, FIRE, LIGHTNING};

// game types:
enum GAME_TYPE {DELETE_MARBLES};


int score = 0;
int lose = 0;
bool quit = false;

///////////////////////////////////////////////////////////////////////////////////////////////////////
int game_ending = -1; //-1 : not over , 0 :win , 1:lost
int power = 0;   //idk
int g_theme_index=0;
int music_index=0;
////////////////////////////////////////////////////////////////////////////////////////////////////////


/* some global variables */
int TextureInPathCount[MARBLE_TYPES_MAX];
Texture marbleTextures[MARBLE_TYPES_MAX];

Texture shooterTexture;
Texture backgroundTexture;
Texture portalTexture;
Texture bombTexture;
Texture fireTexture;
Texture lightningTexture;

//////////////////////////////////////////////////////////////////////////////////////////////////////////

Menu g_gameinterface;
Menu g_pausescreen;
Menu* g_active_menu;

Button * currentButton;  //mouse is hovering on this
Button * activeButton;   //mouse has clicked on this
TextBox game_timer(SCREEN_WIDTH-410,40,200,50);


void exit_handler()
{
    quit=true;
}
void resume_handler()
{
    g_active_menu=&g_gameinterface;
}
void pause_handler()
{
    g_active_menu=&g_pausescreen;
}
void soundbuttonfunc()
{
    wantsound=!wantsound;
}
void musicbuttonfunc()
{
     if(wantmusic)
     {
         Mix_PauseMusic();
         wantmusic=0;
     }
     else
     {
         Mix_ResumeMusic();
         wantmusic=1;
     }
}
void init_interface()
{
    g_active_menu=&g_gameinterface;
    Button pause(SCREEN_WIDTH-210,40,200,30);
    pause.loadtexture("./textures/pause.png");

    Button exit(SCREEN_WIDTH/2-200,SCREEN_HEIGHT/2-30,400,60);
    pause.setHandlerFunc(pause_handler);
    exit.loadtexture("./textures/exit.png");
    exit.setHandlerFunc(exit_handler);

    Button resume(SCREEN_WIDTH/2-200,SCREEN_HEIGHT/2-100,400,60);
    resume.loadtexture("./textures/resume.png");
    resume.setHandlerFunc(resume_handler);

        Button sound(SCREEN_WIDTH/2-200,SCREEN_HEIGHT/2-170,BUTTON_WIDTH,BUTTON_HEIGHT);
        if(wantsound)
        {
        sound.loadtexture("./textures/soundon.png");
        sound.loadalttexture("./textures/soundoff.png");
        }
        else
        {
        sound.loadtexture("./textures/soundoff.png");
        sound.loadalttexture("./textures/soundon.png");
        }
        sound.setHandlerFunc(soundbuttonfunc);

    Button music(SCREEN_WIDTH/2-200,SCREEN_HEIGHT/2-240,BUTTON_WIDTH,BUTTON_HEIGHT);
        if(wantmusic)
        {
        music.loadtexture("./textures/musicon.png");
        music.loadalttexture("./textures/musicoff.png");
        }
        else
        {
        music.loadtexture("./textures/musicoff.png");
        music.loadalttexture("./textures/musicon.png");
        }
    music.setHandlerFunc(musicbuttonfunc);

    g_gameinterface.addButton(pause);
    g_gameinterface.addBox(game_timer);
    g_pausescreen.addButton(exit);
    g_pausescreen.addButton(resume);
    g_pausescreen.addButton(sound);
    g_pausescreen.addButton(music);

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////

Timer cclock;


/* this is a function wich returns a random texture pointer */
Texture* MarbleTextureRand()
{
	return &(marbleTextures[rand() % MARBLE_TYPES_MAX]);
}

Texture* MarbleTextureRandInGame()
{
	int index = rand() % MARBLE_TYPES_MAX;
	while( TextureInPathCount[index]  == 0)
		index = rand() % MARBLE_TYPES_MAX;

	return &(marbleTextures[index]);

}

/* the fundemental classess of the game which are the marbles , the path and the shooter are defined here */

class Marble
{
private:
	vector2D pos;
	vector2D vel;
	Texture *texture;
	MPOWER power = NORMAL;
	int radius = RADI;
	double angle = 0;
	bool doRend = 1;


public:
	vector2D getpos()
	{
		return pos;
	}

	void setpos(vector2D vec)
	{
		pos = vec;

		if(pos.x < -radius || pos.y < -radius || pos.x > radius + SCREEN_WIDTH || pos.y > radius + SCREEN_WIDTH)
			doRend = false;
		else
			doRend = true;
	}

	vector2D getvel()
	{
		return vel;
	}

	void setvel(vector2D vec)
	{
		vel = vec;
	}

	Texture* getTexture()
	{
		return texture;
	}

	void setTexture(Texture* tex)
	{
		texture = tex;
	}

	double getangle()
	{
		return angle;
	}

	void setangle(double A)
	{
		angle  = A;
	}

	int getpower()
	{
		return power;
	}

	void translate(int deltaT)
	{
		setpos(pos + vel * deltaT);
	}

	void render()
	{
		if(!doRend)
			return;

		if(texture == NULL)
		{
			std::cerr << "[ERROR] marble texture is NULL!\n";
			exit(-1);
		}

		SDL_Rect scale = {0, 0, 2*radius, 2*radius};
		SDL_Point p = {radius, radius};
		if(power == NORMAL)
		{
			texture->render(pos.x - radius, pos.y - radius, NULL, &scale, angle, &p);
		}

		if(power == BOMB)
		{
			bombTexture.render(pos.x - radius, pos.y - radius, NULL, &scale, angle, &p);
		}

		if(power == FIRE)
		{
			fireTexture.render(pos.x - radius, pos.y - radius, NULL, &scale, angle, &p);
		}

		if(power == LIGHTNING)
		{
			lightningTexture.render(pos.x - radius, pos.y - radius, NULL, &scale, angle, &p);
		}
	}

	void makeBomb()
	{
		power = BOMB;
		radius = 30;
	}

	void makeFire()
	{
		power = FIRE;
		radius = 33;
	}

	void makeLightning()
	{
		power = LIGHTNING;
	}


	Marble(Texture* tex, vector2D r)
	{
		setTexture(tex);
		setpos(r);
	}

	~Marble()
	{

	}
};

std::vector<Marble*> flying_marbles;

class Shooter
{
private:
	Marble **ammo;
	vector2D pos = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
	double angle = 0;


public:
	void SetMarbleOnShooter(Marble* marbie)
	{
		marbie->setpos( pos + ( MARBLE_DISTANCE_FROM_BIRD * angleNorm(angle - 90) ) );
		marbie->setangle(angle);
	}

	Marble* createNewAmmo()
	{
		Texture *tmp;
		tmp = MarbleTextureRandInGame();
		if (tmp == NULL)
			tmp = MarbleTextureRand();

		Marble* marbie = new Marble(tmp, {pos.x, pos.y});

		int trand;
		trand = rand() % 100;
		if( (trand) <= 10 )
			marbie->makeBomb();
		else if((trand) <= 20)
			marbie->makeFire();
		//else if((trand <= 30))
		//	marbie->makeLightning();

		SetMarbleOnShooter(marbie);
		return marbie;
	}

	void handleEvent(SDL_Event* e)
	{
		if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			angle = ( ( 180.0 * atan2(y - pos.y, x - pos.x) ) / PI) + 90;
			if (e->type == SDL_MOUSEBUTTONDOWN)
///////////////////////////////////////////////////////
{


                shoot();
                g_shotsound.play();
}
/////////////////////////////////////////////////////////
			SetMarbleOnShooter(ammo[0]);
			SetMarbleOnShooter(ammo[1]);
		}

		if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_SPACE)
		{
			Marble *tmp = ammo[0];
			ammo[0] = ammo[1];
			ammo[1] = tmp;
		}
	}

	void shoot()
	{
		ammo[0]->setvel(angleNorm(angle - 90) / 1.5f);
		flying_marbles.push_back(ammo[0]);
		ammo[0] = ammo[1];
		ammo[1] = createNewAmmo();
	}

	void render()
	{
		SDL_Rect scale = {0, 0, SHOOTER_SIZE, SHOOTER_SIZE};
		shooterTexture.render(pos.x - SHOOTER_SIZE/2, pos.y - SHOOTER_SIZE/2, NULL, &scale, angle);
		for(int i = 1; i >= 0; i--)
			ammo[i]->render();
	}

	Shooter(int x = -1, int y = -1)
	{
		if(x > 0 && y > 0)
		{
			pos = v2D(x, y);
		}
		ammo = new Marble*[2];
		ammo[0] = createNewAmmo();
		SetMarbleOnShooter(ammo[0]);
		ammo[1] = createNewAmmo();
		SetMarbleOnShooter(ammo[1]);
	}

	~Shooter()
	{
		delete ammo[0];
		delete ammo[1];
		delete ammo;
	}


};


class GamePath
{
public:
	std::vector<vector2D> points;
	std::vector<Marble*>  MarblePlaces;

	GamePath()
	{
	}

	void init()
	{
		for(int i = 0; i < points.size(); i++)
		{
			MarblePlaces.push_back(NULL);
		}
	}

	~GamePath()
	{
	}

	void Reset()
	{
		points.clear();
		MarblePlaces.clear();
	}

	int size()
	{
		return points.size();
	}

	void checkRange(int index)
	{
		if(index >= size())
		{
			std::cerr << "[ERROR] GamePath: index out of range\n";
			exit(-1);
		}
	}

	Marble* operator [](int index)
	{
		checkRange(index);
		return MarblePlaces[index];
	}

	void updatepos()
	{
		for(int i = 0; i < size(); i++)
		{
			if(MarblePlaces[i] != NULL)
				MarblePlaces[i]->setpos(points[i]);
		}
	}

	int shift(int index)
	{
		int ret;
		checkRange(index);
		int new_index = index + MARBLE_DISTANCE_ON_PATH;

		// if it's null do nothing
		if(MarblePlaces[index] == NULL)
			ret = 0;	// i did nothing

		// sorry buddy you are out of range so you will be deleted :(
		else if(new_index >= size())
		{
			delete MarblePlaces[index];
			MarblePlaces[index] = NULL;
			ret = -1;	// Aaaaaaaaand this likely means GAME OVER!
		}

		// well it will move to a new position
		else
		{
			// if there is someone there move them to!
			if(MarblePlaces[new_index] != NULL)
				shift(new_index);
			MarblePlaces[new_index] = MarblePlaces[index];
			MarblePlaces[index] = NULL;
			ret = 1;	// i shifted
		}

		handleBadPos(index + MARBLE_DISTANCE_ON_PATH);
		updatepos();
		return ret;

	}

	int move(int index)
	{
		// there is nothing to move
		if(MarblePlaces[index] == NULL)
			return 0;

		// you have reached end of path
		if(index >= size() - 1)
		{
			if(index < size() && MarblePlaces[index] != NULL)
			{
				delete MarblePlaces[index];
				MarblePlaces[index] = NULL;
			}

			return -1;
		}

		// the next block isn't empty the program should crash!
		if( MarblePlaces[index + 1] != NULL )
		{
			//fprintf(stderr, "the marbles went inside eachother! that's bad!\n");
			shift(index + 1);
		}

		// if there is no problem, move
		MarblePlaces[index + 1] = MarblePlaces[index];
		MarblePlaces[index] = NULL;
		//test
		//return index + 1;
		return index + 1;
	}

	int step()
	{
		int index = -1;
		int ret = 0;

		// skip empty spaces
		while(!ret)
			ret = move(++index);

		index += MARBLE_DISTANCE_ON_PATH;

		for(ret = 1; ret != -1 && ret != 0; index += MARBLE_DISTANCE_ON_PATH)
		{
			ret = move(index);
		}

		updatepos();
		return ret;
	}

	int handleBadPos(int index)
	{
		for(int i = index - 1; i > index - MARBLE_DISTANCE_ON_PATH && i >= 0; i--)
		{
			if(MarblePlaces[i] != NULL) // shift!
			{
				int j;
				for(j = index; j < i + 8; j++)
					move(j);
				return j;
			}

			else return index;
		}
	}

	int insert(int index, Marble* marbie, int *gameOver = NULL)
	{
		int ret = 1;
		int gg;
		gg = shift(index);
		if(gg == -1)
			LoseFunc();
		if (gameOver != NULL)
			*gameOver = gg;

		MarblePlaces[index] = marbie;

		ret = handleBadPos(index);

		if(MarblePlaces[index]->getpower() == NORMAL)
			TextureInPathCount[MarblePlaces[index]->getTexture() - marbleTextures]++;
		updatepos();
		return ret;
	}

	int popBomb(int index)
	{
		int upper = index + MARBLE_DISTANCE_ON_PATH * 4;
		int lower = index - MARBLE_DISTANCE_ON_PATH * 4;
		int gone = 0;
		for(int i = lower; i <= upper; i += MARBLE_DISTANCE_ON_PATH)
		{
			if(i < 0 || i > size())
				continue;
			if(MarblePlaces[i] != NULL)
			{
				gone++;
				delete MarblePlaces[i];
			}
			MarblePlaces[i] = NULL;
		}
		//////////////////////////////
		            g_smallblast.play();
		return gone;

	}

	int popFire(int index)
	{
		int gone = 0;
		for(int i = index - MARBLE_DISTANCE_ON_PATH; i <= index; i += MARBLE_DISTANCE_ON_PATH)
		{
			if(i < 0 || i > size())
				continue;
			if(MarblePlaces[i] != NULL)
			{
				gone++;
				delete MarblePlaces[i];
			}
			MarblePlaces[i] = NULL;

		}
		///////////////////////////////
		            g_smallblast.play();
		return gone;
	}

	// this returns the scores that we got
	int pop1(int index)
	{

		while(index >= size())
			index -= MARBLE_DISTANCE_ON_PATH;


		if(MarblePlaces[index]->getpower() == BOMB)
		{
			return popBomb(index);
		}

		if(MarblePlaces[index]->getpower() == FIRE || MarblePlaces[index]->getpower() == LIGHTNING)
		{
			return popFire(index);
		}


		int upper = index;
	    	int lower = index;

		for(int i = index + MARBLE_DISTANCE_ON_PATH; i < size(); i += MARBLE_DISTANCE_ON_PATH)
		{
			if( MarblePlaces[i] == NULL || (MarblePlaces[ i - MARBLE_DISTANCE_ON_PATH ]->getTexture() ) != ( MarblePlaces[i]->getTexture() ) )
			{
				upper = i - MARBLE_DISTANCE_ON_PATH;
				break;
			}

		}

		for(int i = index - MARBLE_DISTANCE_ON_PATH; i >= 0; i -= MARBLE_DISTANCE_ON_PATH)
		{
			if( MarblePlaces[i] == NULL || (MarblePlaces[ i + MARBLE_DISTANCE_ON_PATH ]->getTexture() ) != ( MarblePlaces[i]->getTexture() ) )
			{
				lower = i + MARBLE_DISTANCE_ON_PATH;
				break;
			}
		}

		if( (upper - lower) < (2 * MARBLE_DISTANCE_ON_PATH))
			return 0;	// i poped nothing because there weren't three marbles in a row
		else
		{
			int n = (upper - lower + MARBLE_DISTANCE_ON_PATH)/MARBLE_DISTANCE_ON_PATH;	// this is the number of marbles i popped off
			TextureInPathCount[MarblePlaces[index]->getTexture() - marbleTextures] -= n;
			for(int i = lower; i <= upper; i += MARBLE_DISTANCE_ON_PATH)
			{
				if(MarblePlaces[i] != NULL)
					delete MarblePlaces[i];
				MarblePlaces[i] = NULL;
			}

			updatepos();
			/////////////////////////////////////////////////
                        g_smallblast.play();
			return n;
		}
	}

	void render()
	{
		for(int i = 0; i < size() - 1; i++)
		{
				SDL_SetRenderDrawColor(gRenderer, i * 100, 0, 0, 0xFF);
				SDL_RenderDrawLine(gRenderer, points[i].x, points[i].y, points[i+1].x, points[i+1].y);
		}

		// endpoint texture
		SDL_Rect scale = {0, 0, 80, 80};
		portalTexture.render(points[size() - 1].x - 40, points[size() - 1].y - 40, NULL, &scale, 0);

		// marble rendering part
		for(int i = 0; i < size(); i++)
			if(MarblePlaces[i] != NULL)
				MarblePlaces[i]->render();
	}


};

/* some other global functions based on the classes we just made */
GamePath daWay;




bool isOnShooter(vector2D v)
{
	if( (v.x < (SCREEN_WIDTH/2 + 140)) &&  (v.x > (SCREEN_WIDTH/2 - 140)) && (v.y < (SCREEN_HEIGHT/2 + 140)) && (v.y > (SCREEN_HEIGHT/2 - 140)))
		return true;
	return false;
}

int checkifPathisGood()
{
	for(int i = 0; i < daWay.size(); i++)
	{
		if(isOnShooter(daWay.points[i]))
			return 0;
	}

	return 1;
}
/* some functions we will use in the game(these are more likely to use global variables) */

void createRandomPaths3(std::vector<vector2D>* points)
{
	daWay.Reset();
    	srand(time(NULL));
    	BezierPath *path = new BezierPath();
	int screenMidx = SCREEN_WIDTH / 2;
    	vector2D P[2][2];
    	vector2D C[2][2];
   	P[0][0] = {SCREEN_WIDTH / 8, SCREEN_HEIGHT/ 8};
    	P[1][0] = {SCREEN_WIDTH - SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8};
    	P[1][1] = {SCREEN_WIDTH - SCREEN_WIDTH / 8, SCREEN_HEIGHT - SCREEN_HEIGHT / 8};
    	P[0][1] = {SCREEN_WIDTH / 8, SCREEN_HEIGHT - SCREEN_HEIGHT / 8};

    	for(int i = 0; i < 2; i++)
        	for(int j = 0; j < 2; j++)
            	C[i][j] = {(rand() % (SCREEN_WIDTH)) - SCREEN_WIDTH / 2, (rand() % (SCREEN_HEIGHT)) + SCREEN_HEIGHT / 2};

   	 path->AddCurve({P[0][0] + v2D(300, 0), P[0][0] - C[0][0], P[1][0] + C[1][0], P[1][0]}, 300);
    	path->AddCurve({P[1][0], P[1][0] - C[1][0], P[1][1] + C[1][1], P[1][1]}, 300);
    	path->AddCurve({P[1][1], P[1][1] - C[1][1], P[0][1] + C[0][1], P[0][1]}, 300);
    	path->AddCurve({P[0][1], P[0][1] - C[0][1], P[0][0] + C[0][0], P[0][0]}, 300);

    	path->SampleEqui(points);
    	delete path;
    	if(!checkifPathisGood())
		createRandomPaths3(points);

}

void createUltraRandomPaths4(std::vector<vector2D>* points)
{
	daWay.Reset();
	srand(time(NULL));
    	BezierPath *path = new BezierPath();
    	vector2D P[2][2];
	vector2D C[2][2];
	P[0][0] = {SCREEN_WIDTH / 8, SCREEN_HEIGHT/ 8};
	P[1][0] = {SCREEN_WIDTH - SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8};
	P[1][1] = {SCREEN_WIDTH - SCREEN_WIDTH / 8, SCREEN_HEIGHT - SCREEN_HEIGHT / 8};
	P[0][1] = {SCREEN_WIDTH / 8, SCREEN_HEIGHT - SCREEN_HEIGHT / 8};

	for(int i = 0; i < 2; i++)
		for(int j = 0; j < 2; j++)
			C[i][j] = {(rand() % (SCREEN_WIDTH)) - SCREEN_WIDTH / 2, (rand() % (SCREEN_HEIGHT)) + SCREEN_HEIGHT / 2};

	path->AddCurve({P[0][0] + v2D(300, 0), P[0][0] - C[0][0], P[1][0] + C[1][0], P[1][0]}, 300);
	path->AddCurve({P[1][0], P[1][0] - C[1][0], P[1][1] + C[1][1], P[1][1]}, 300);
	path->AddCurve({P[1][1], P[1][1] - C[1][1], P[0][1] + C[0][1], P[0][1]}, 300);
	path->AddCurve({P[0][1], P[0][1] - C[0][1], P[0][0] + C[0][0], P[0][0]}, 300);


	path->SampleEqui(points);
	delete path;
    	if(!checkifPathisGood())
		createUltraRandomPaths4(points);
}

void AnotherRandomPath5(std::vector<vector2D>* points)
{
	daWay.Reset();
	srand(time(NULL));
	BezierPath *path = new BezierPath();
	int n = (rand() % 8) + 8;
	vector2D p[n];
	vector2D c[n];
	for(int i = 0; i < n; i++)
	{
		p[i] = v2D( (rand() % (SCREEN_WIDTH )), (rand() % (SCREEN_HEIGHT)));
		c[i] = v2D( (rand() % 1000) - 500, (rand() % 1000) - 500);
	}
	for(int i = 0; i < n - 1; i++)
		path->AddCurve({p[i], p[i] + c[i], p[i+1] - c[i+1], p[i+1]}, 300);
	path->SampleEqui(points);
	delete path;
    	if(!checkifPathisGood())
		AnotherRandomPath5(points);
}

void createSpiralPath1(std::vector<vector2D>* points)
{
	daWay.Reset();
	srand(time(NULL));
    	BezierPath *path = new BezierPath();
	vector2D p[4], c[4], o, oc;

	o = {SCREEN_WIDTH - 1500, SCREEN_HEIGHT };
	oc = {-1000, 1000};

	p[0] = {1000, 200};
	c[0] = {-250, -450};

	p[1] = {100, 620};
	c[1] = {-350, -350};

	p[2] = p[0] + v2D(-100, 70);
	c[2] = {250, 250};

	p[3] = p[1] + v2D(300, -200);
	c[3] = {-150, -150};


	path->AddCurve({o, o + oc, p[0] - 2 *c[0], p[0]}, 300);
	path->AddCurve({p[0], p[0] + c[0], p[1] + c[1], p[1]}, 300);
	path->AddCurve({p[1], p[1] - c[1] + v2D(-120, -120), p[2] + c[2], p[2]}, 300);
	path->AddCurve({p[2], p[2] - c[2], p[3] + c[3], p[3]}, 300);

	path->SampleEqui(points);
	delete path;

}

void createOhmPath2(std::vector<vector2D>* points)
{
	srand(time(NULL));
    	BezierPath *path = new BezierPath();
	vector2D p[4], c[4], o, oc;

	o = {SCREEN_WIDTH - 2000, SCREEN_HEIGHT - 2000 };
	oc = {-1000, 1000};

	p[0] = {100, 30};
	c[0] = {500, 100};

	p[1] = {300, 650};
	c[1] = {-80, -80};

	p[2] = v2D(SCREEN_WIDTH - p[1].x, p[1].y);
	c[2] = {-c[1].x, c[1].y};

	p[3] = {620, 30};
	c[2] = {-500, 100};

	path->AddCurve({o, o + oc, p[0] - 2 *c[0], p[0]}, 300);
	path->AddCurve({p[0], p[0] + c[0], p[1] + c[1], p[1]}, 300);
	path->AddCurve({p[1], p[1] - c[1] + v2D(-120, -120), p[2] + c[2], p[2]}, 300);
	path->AddCurve({p[2], p[2] - c[2], p[3] + c[3], p[3]}, 300);

	path->SampleEqui(points);
	delete path;
}

int AllPathMarbleCounts()
{
	for(int i = 0; i < MARBLE_TYPES_MAX; i++)
	{
		TextureInPathCount[i] = 0;
	}

	int count = 0;
	for(int i = 0; i < daWay.size(); i++)
	{
		if(daWay.MarblePlaces[i] == NULL)
			continue;
		TextureInPathCount[daWay.MarblePlaces[i]->getTexture() - marbleTextures]++;
		count++;
	}
	return count;
}

void generatePathMarbles1(int n)
{
	for(int i = 0; i < MARBLE_TYPES_MAX; i++)
		TextureInPathCount[i] = 0;

	int textureIndex = rand() % MARBLE_TYPES_MAX;
	for(int i = 0; i < n * MARBLE_DISTANCE_ON_PATH; i += MARBLE_DISTANCE_ON_PATH)
	{
		if(rand() % 3 == 0)
			textureIndex = rand() % MARBLE_TYPES_MAX;
		TextureInPathCount[textureIndex]++;
		daWay.MarblePlaces[i] = new Marble(&(marbleTextures[textureIndex]), daWay.points[i] );
	}

	for(int i = 0; i < MARBLE_TYPES_MAX; i++)
		std::cout << TextureInPathCount[i] << "\n";
}

void WinFunc()
{
	printf("you won with score: %d\n", score);
	quit = true;
/////////////////////////////////////////////////////////////////
    game_ending=0;
}

void LoseFunc()
{
	printf("sorry try again!\n");
	lose = 1;
/////////////////////////////////////////////////////////////////
    game_ending=1;
	quit  = true;
}

void handleCollision()
{
	for(int i = 0; i < flying_marbles.size(); i++)
	{
		for(int j = 0; j < daWay.size(); j++)
		{
			if(daWay[j] == NULL || flying_marbles[i] == NULL)
				continue;
			vector2D pos1 = daWay[j]->getpos();
			vector2D pos2 = flying_marbles[i]->getpos();
			if ( CheckCircleCollision(pos1, RADI, pos2, RADI) )
			{
				flying_marbles[i]->setangle(0);
				int gameOver = 0;
				if(daWay.insert(j, flying_marbles[i]) == -1)
				{
					LoseFunc();
				}
				if(flying_marbles[i]->getpower() != LIGHTNING)
					flying_marbles.erase(flying_marbles.begin() + i);
				daWay.pop1(j);
				i = 0;
				if(flying_marbles.size() == 0)
					return;
				if(AllPathMarbleCounts() == 0)
				{
					WinFunc();
				}

				j = 0;
			}
		}
	}
}

void flyingMove(int deltaT)
{
	for(auto it = flying_marbles.begin(); it != flying_marbles.end(); it++)
	{
		(*it)->translate(deltaT);
	}
}

void flyingRender()
{
	for(auto it = flying_marbles.begin(); it != flying_marbles.end(); it++)
	{
		(*it)->render();
	}
}



/* these here are the initiating, Loading and destroying functions */
void init()
{
	srand(time(NULL));
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		exit(-1);
	}

	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
	{
		printf( "Warning: Linear texture filtering not enabled!" );
	}

	gWindow = SDL_CreateWindow("Zuma Marble Blast!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

	if( gWindow == NULL )
	{
		printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
		exit(-1);
	}

	gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    	if( gRenderer == NULL )
    	{
        	printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        	exit(-1);
    	}

	//Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        exit(-1);
    }

}


void LoadMedia()
{
	std::string s;
	for(int i = 0; i < MARBLE_TYPES_MAX; i++)
	{
		s = std::to_string(i) + ".png";
		if( !marbleTextures[i].loadFromFile(s.c_str()) )
		{
			printf( "Failed to load dot texture!\n" );
			exit(-1);
		}
	}

	if( !shooterTexture.loadFromFile( "Pshooter.png") )
	{
		printf( "Failed to load dot texture!\n" );
		exit(-1);
	}

	if( !backgroundTexture.loadFromFile( "bg.png") )
	{
		printf( "Failed to load dot texture!\n" );
		exit(-1);
	}

	if( !portalTexture.loadFromFile("portal.png") )
	{
		printf( "Failed to load dot texture!\n" );
		exit(-1);
	}

	if( !bombTexture.loadFromFile("bomb.png") )
	{
		printf( "Failed to load dot texture!\n" );
		exit(-1);
	}

	if( !fireTexture.loadFromFile("fire.png") )
	{
		printf( "Failed to load dot texture!\n" );
		exit(-1);
	}

	if( !lightningTexture.loadFromFile("lightning.png") )
	{
		printf( "Failed to load dot texture!\n" );
		exit(-1);
	}
}

/* here are the loops made for the game MODES we declare by using any of this loops in your loop you call the game mode let the player play it and they will return the achieved score */
void SampleGameLoop(int path)
{
    init_interface();
	Uint32 prevT, currentT, deltaT;
	SDL_Event e;
	cclock.start();
	prevT = cclock.getTicks();

	int chosenPath = (rand() % 5) + 1;
	chosenPath = 1;
	if(chosenPath == 1) createSpiralPath1(&daWay.points);
	else if(chosenPath == 2) createOhmPath2(&(daWay.points));
	else if(chosenPath == 3) createRandomPaths3(&daWay.points);
	else if(chosenPath == 4) createUltraRandomPaths4(&(daWay.points));
	else AnotherRandomPath5(&daWay.points);
	daWay.init();
    	generatePathMarbles1(80);

	backgroundTexture.render(0, 0);
	daWay.render();
	SDL_RenderPresent(gRenderer);

	Shooter *bird = new Shooter();
	int i = 0;
	int j = 0;
	while(!quit)
	{
	    game_timer.flush();
	    game_timer.insertline(to_string(cclock.getTicks()));

			while( SDL_PollEvent( &e ) != 0)
			{
				if( e.type == SDL_QUIT)
					quit = true;
				else
					bird->handleEvent( &e );
//                cerr<<"problems";
//////////////////////////////////////////////////////////////////////////////
                if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    if (currentButton != NULL)
                    {
                        currentButton->call();
//                        cerr<<"click";
                            g_buttonclick.play();
                        activeButton=currentButton;
                    }
                }
            }
            if (e.type == SDL_MOUSEMOTION)
                currentButton =
                    g_active_menu->getButtonFromMouse(e.motion.x, e.motion.y);

        }
                /////////////////////////////////
        if (g_active_menu==&g_gameinterface)
        {
			currentT = cclock.getTicks();
			deltaT = currentT - prevT;
			prevT = currentT;
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xff);
			SDL_RenderClear(gRenderer);
			backgroundTexture.render(0, 0);

			bird->render();
			flyingMove(deltaT);

			if((i++ % 10) == 0)
			{
				if(daWay.step() == -1)
					LoseFunc();
			}
			handleCollision();
			daWay.render();
			flyingRender();

        }

        game_timer.flush();
        game_timer.insertline("1");
        g_active_menu->renderBG();
        g_active_menu->renderButtons();
//        g_active_menu->renderBoxes();
        SDL_RenderPresent(gRenderer);
////////////////////////////////////////////////////



			SDL_RenderPresent(gRenderer);

			if(lose == 0)
				score = (6000) - (cclock.getTicks() / 1000);
			else
				score = 0;

	}

}


void TimerGameLoop(int path)
{
	Uint32 prevT, currentT, deltaT;
	SDL_Event e;
	cclock.start();
	prevT = cclock.getTicks();

	int chosenPath = (rand() % 5) + 1;
	chosenPath = 1;

	if(chosenPath == 1) createSpiralPath1(&daWay.points);
	else if(chosenPath == 2) createOhmPath2(&daWay.points);
    else if(chosenPath == 3) createRandomPaths3(&daWay.points);
	else if(chosenPath == 4) createUltraRandomPaths4(&(daWay.points));
	else AnotherRandomPath5(&daWay.points);
	daWay.init();
    	generatePathMarbles1(80);

	backgroundTexture.render(0, 0);
	daWay.render();
	SDL_RenderPresent(gRenderer);

	Shooter *bird = new Shooter();
	int i = 0;
	int j = 0;
	int pmove = 0;
	int tmove = 5;
	int gTime = cclock.getTicks();
	while(!quit)
	{

			while( SDL_PollEvent( &e ) != 0)
			{
				if( e.type == SDL_QUIT)
					quit = true;
				else
					bird->handleEvent( &e );
			}

                /////////////////////////////////
        if (g_active_menu==&g_gameinterface)
        {
			currentT = cclock.getTicks();
			deltaT = currentT - prevT;
			prevT = currentT;
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xff);
			SDL_RenderClear(gRenderer);
			backgroundTexture.render(0, 0);

			bird->render();
			flyingMove(deltaT);

			if((i++ % 10) == 0)
			{
				if(daWay.step() == -1)
					LoseFunc();
			}
			handleCollision();
			daWay.render();
			flyingRender();

        }

        game_timer.flush();
        game_timer.insertline("1");
        g_active_menu->renderBG();
        g_active_menu->renderButtons();
//        g_active_menu->renderBoxes();
        SDL_RenderPresent(gRenderer);
////////////////////////////////////////////////////
			SDL_RenderPresent(gRenderer);

			if(lose == 0)
				score = (60 * 1000 * 4) - (cclock.getTicks() / 1000);
			else
				score = 0;
			gTime = cclock.getTicks();
			if(gTime > 60 * 1000 * 4)
				LoseFunc();

	}

}


#endif


