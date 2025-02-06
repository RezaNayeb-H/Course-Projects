#ifndef _ZUMA_
#define _ZUMA_
#include "SDLtools.hpp"
#include <vector>
#include <algorithm>
#include <time.h>
#include <cstdlib>
#include "mathHelper.hpp"
#include "BezierPath.hpp"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

namespace zuma
{
	/* this is the universal in game timer you shouldn't do anything whith it the functions interact with it themselves(just remember to use the init() function so it starts)*/
	Timer clock;

	std::vector<vector2D> points;

	#define MARBLE_DISTANCE_FROM_BIRD 26
	#define MARBLE_MAX 5
	#define RADI 20
	#define SPEED 0.3f
	#define PART_SIZ 3
	#define DIST 8

	Texture mableTextures[MARBLE_MAX];

	Texture shooterTexture;
	Texture backgroundTexture;

	enum PLACE {IN, OUT};
		enum STATES {flying, in_path, on_shooter};
		enum TYPE { NORMAL };
		double *BAngle;
		vector2D *BPos;



	/* a marble class for the game */
	class marble
	{
		public:
			marble(Texture *tex, STATES state, double px, double py, TYPE t = NORMAL ,double vx = 0, double vy = 0);
			~marble();
			void free();
			//bool move(Uint32 deltaT);
			void shoot(double angle);
			int handleStates(double data);
			int handleFlyingState(int deltaT);
			int handlePathState(int deltaT);
			void handleShooterState();
			void render();

			int connected = 1;
			int doRend = 1;
			int mCurrentWaypoint;
			vector2D pos;				// the position vector
			vector2D  vel;
			int i = 0;				
			int radi = RADI;					// the marble's radius
			Texture *texture;			// the marbles texture
			STATES currentState;
			int angle;
			TYPE type;
	};
	/* thess vectors are the list of marbles that are in the game you should use these vectors if you want other classess like shooter work correctly */
	std::vector<marble*> path_marbles;
	std::vector<marble*> flying_marbles;
	
	// here we define the marble funcions

	marble::marble(Texture *tex, STATES state, double px, double py, TYPE t, double vx, double vy)
	{
		pos.x = px;
		pos.y = py;
		vel.x = vx;
		vel.y = vy;
		texture = tex;
		type = t;
		currentState = state;
		angle = 0;
	}

	marble::~marble()
	{
		return;
	}

	void marble::free()
	{
		texture = NULL;
		pos = {0, 0};
		vel = {0, 0};
		radi = 0;
	}


	void marble::shoot(double angle)
	{
		currentState = flying;
		vel = angleNorm(angle - 90) / 1.5f;
	}

	int marble::handleStates(double data)
	{
		switch (currentState)
		{
			case flying:
				return handleFlyingState(data);
			case on_shooter:
				handleShooterState();
				break;
			case in_path:
				return handlePathState(data);
				break;
		}
		return 0x0;
	}

	int marble::handleFlyingState(int deltaT)
	{
		int size = points.size() - 1;
		vector2D prev = pos;
		pos += vel * deltaT;
		if (pos.x > SCREEN_WIDTH + radi || pos.x < -radi || pos.y > SCREEN_HEIGHT + radi || pos.y < -radi)
			return -1;

		int state;
		for(int i = 0; i < path_marbles.size(); i++)
		{

			if(CheckCircleCollision(pos, RADI, path_marbles[i]->pos, RADI))
			{
				mCurrentWaypoint = path_marbles[i]->mCurrentWaypoint;
				currentState = in_path;
				connected = path_marbles[i]->connected;
				pos = points[mCurrentWaypoint];
				path_marbles.insert(path_marbles.begin() + i, this);
				for(int j = 0; j <= i; j++)
				{
					path_marbles[j]->mCurrentWaypoint -= 8;
					path_marbles[j]->pos = points[mCurrentWaypoint];
				}	
				return i+1;
			}
		}
		return -2;
		
	}

	int marble::handlePathState(int deltaT)
	{
		if (connected == 0)
		{
			render();
			return 0;
		}
		if (i++ % 20 != 0)
				return 0;
		if(mCurrentWaypoint >= points.size())
		{
			vel = {0, 0};
			return -1;
		}

		if(mCurrentWaypoint < 1)
		{
			mCurrentWaypoint++;
			doRend = 0;
			return -2;
		}
		
		angle = 0;
		pos = points[mCurrentWaypoint];
		mCurrentWaypoint++;
		doRend = 1;
		return -2;
	}		

	void marble::handleShooterState()
	{
		angle = *BAngle;
		pos = (angleNorm(angle- 90) * MARBLE_DISTANCE_FROM_BIRD) + v2D(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
	}

	void marble::render()
	{
		if(doRend == 0)
			return;
		if(texture == NULL)
			return;
		SDL_Rect scale = {0, 0, 2*radi, 2*radi};
		SDL_Point p = {radi, radi};
		texture->render(pos.x - radi, pos.y - radi, NULL, &scale, angle, &p);

	}






	/* an object who doesnt move like the shooter */
	class shooter
	{
		public:
			shooter(double x = SCREEN_WIDTH/2, double y = SCREEN_HEIGHT/2);
			~shooter();
			vector2D getPos();
			double getAngle();
			void free();
			void handleEvent(SDL_Event *e);
			void render();
			void shoot();
			double angle = 0;
			vector2D pos;
		private:
			
			//double angle = 0;
			marble **ammo;
			// time of the last shot
			Uint32 last_shotT;
	};

	shooter::shooter(double x, double y)
	{
		pos.x = x;
		pos.y = y;
		ammo = new marble*[2];
		ammo[0] = new marble( &(mableTextures[rand() % MARBLE_MAX]), STATES::on_shooter, x, y - MARBLE_DISTANCE_FROM_BIRD );
		ammo[1] = new marble( &(mableTextures[rand() % MARBLE_MAX]), STATES::on_shooter, x, y - MARBLE_DISTANCE_FROM_BIRD );
		BPos = &pos;
		BAngle = &angle;
	}

	shooter::~shooter()
	{
		shooter::free();
	}

	void shooter::free()
	{
		pos.x = 0;
		pos.y = 0;
	}

	vector2D shooter::getPos()
	{
		return pos;
	}

	double shooter::getAngle()
	{
		return angle;
	}

	void shooter::handleEvent(SDL_Event *e)
	{
		if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			angle = ((180.0 * atan2(y - pos.y, x - pos.x)) / PI) + 90;
			if (e->type == SDL_MOUSEBUTTONDOWN)
			{
				shoot();
			}

			ammo[0]->handleStates(angle);
			ammo[1]->handleStates(angle);
		}
		if(e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_SPACE)
		{
			marble *tmp = ammo[0];
			ammo[0] = ammo[1];
			ammo[1] = tmp; 
		}
	}

	void shooter::shoot()
	{
		ammo[0]->shoot(angle);
		flying_marbles.push_back((ammo[0]));
		//delete ammo[0];
		ammo[0] = ammo[1];
		ammo[1] = new marble( &(mableTextures[rand() % MARBLE_MAX]), STATES::on_shooter, pos.x, pos.y + MARBLE_DISTANCE_FROM_BIRD );
	}

	void shooter::render()
	{

		SDL_Rect scale = {0, 0, 200, 200};
		shooterTexture.render(pos.x - 100, pos.y - 100, NULL, &scale, angle);
		for(int i = 1; i >= 0; i--)
			ammo[i]->render();
	}





	/* bezier Path class inspired by galaga SDL tutorial */


	void CreatePaths(std::vector<vector2D>* points)
	{
    	BezierPath *path = new BezierPath();
    	int screenMidx = SCREEN_WIDTH / 2;
		path->AddCurve({v2D(screenMidx + 50.0f, -10.0f), v2D(screenMidx + 50.0f, -20.f), v2D(screenMidx + 50.0f, 30.0f), v2D(screenMidx + 50.0f, 20.0f)}, 10);
		path->AddCurve({v2D(screenMidx + 50.0f, 20.0f), v2D(screenMidx + 50.0f, 100.f), v2D(75.0f, 325.0f), v2D(75.0f, 425.0f)}, 300);
		path->AddCurve({v2D(75.0f, 425.0f), v2D(75.0f, 650.f), v2D(350.0f, 650.0f), v2D(350.0f, 425.0f)}, 300);
		path->AddCurve({v2D(350.0f, 425.0f), v2D(350.0f, 250.0f), v2D(850.0f, 430.0f), v2D(750.0f, 600.0f)}, 300);

		path->SampleEqui(points);
		delete path;

	}


	
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
	shooter bird;

	void LoadMedia()
	{
		std::string s;
		for(int i = 0; i < MARBLE_MAX; i++)
		{
			s = std::to_string(i) + ".png";
			if( !mableTextures[i].loadFromFile(s.c_str()) )
			{	
				printf( "Failed to load dot texture!\n" );
				exit(-1);
			}
		}

		if( !shooterTexture.loadFromFile( "shooter.png") )
		{	
			printf( "Failed to load dot texture!\n" );
			exit(-1);
		}

		if( !backgroundTexture.loadFromFile( "bg.png") )
		{
			printf( "Failed to load dot texture!\n" );
			exit(-1);
		}

	}


	void handleCollision(int flying_index, int path_index)
	{
	}

	int marblePop(int k)
	{
		int upper, lower;
		upper = k;
		lower = k;
		while(upper < path_marbles.size() - 1)
		{
			if(path_marbles[upper]->texture == path_marbles[upper+1]->texture && path_marbles[upper+1]->mCurrentWaypoint >= 0)
				upper++;
			else
				break;																																																																									
		}

		while(lower >= 1)
		{
			if(path_marbles[lower]->texture == path_marbles[lower-1]->texture && path_marbles[lower+1]->mCurrentWaypoint >= 0)
				lower--;
			else
				break;
		}
		if(upper -  lower >= 2)
		{
						path_marbles.erase(path_marbles.begin() + lower, path_marbles.begin() + upper + 1);
						for(auto it = path_marbles.begin() + lower; it != path_marbles.end(); it++)
							(*it)->connected = 0;
		}

		if(upper - lower < 2)
			return 0;
		//else if(lower >= 0)
			//return marblePop(lower) + upper - lower  + 1;
		else
			return upper - lower + 1;
	}


	void SampleGameLoop()
	{
		Uint32 prevT, currentT, deltaT;	// current loop and previous loop time
		SDL_Event e;
		bool quit = false;
		clock.start();
		//prevT = clock.getTicks();
		prevT = SDL_GetTicks();
		CreatePaths(&points);


		/*path_marbles.push_back(marble( &(mableTextures[rand() % MARBLE_MAX]), STATES::in_path, points[1 + DIST].x, points[1 + DIST].y));
		path_marbles[0].mCurrentWaypoint = 2 + DIST;
		path_marbles.push_back(marble( &(mableTextures[rand() % MARBLE_MAX]), STATES::in_path, points[1].x, points[1].y));
		path_marbles[1].mCurrentWaypoint = 2;*/
		Texture *tmpTexture, *prevTexture = &(mableTextures[rand() % MARBLE_MAX]);
		for(int i = 0; i<50; i++)
		{
			if(rand() % 2 == 0)
				tmpTexture = prevTexture;
			else
				tmpTexture = prevTexture = &(mableTextures[rand() % MARBLE_MAX]);
			if(DIST * i>= 0)
				path_marbles.push_back(new marble( tmpTexture, in_path, points[DIST * i].x, points[DIST * i].y));
				
			else
				path_marbles.push_back(new marble( &(mableTextures[/*rand() % MARBLE_MAX]*/ 1]), in_path, -100,-100));

			path_marbles[i]->mCurrentWaypoint = 1 + i * DIST;
		}

		while(!quit)
		{
			while( SDL_PollEvent( &e ) != 0)
			{
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
				else bird.handleEvent( &e );
			}
			
			//currentT = clock.getTicks();
			currentT = SDL_GetTicks();
			deltaT = currentT - prevT;
			prevT = currentT;
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);
			backgroundTexture.render(0, 0);
			
			int x = points.size();
			
			for(int i = 0; i < points.size() - 1; i++)
			{
				SDL_SetRenderDrawColor(gRenderer, i * 100, 0, 0, 0xFF);
				SDL_RenderDrawLine(gRenderer, points[i].x, points[i].y, points[i+1].x, points[i+1].y);
			}
			
			bird.render();

			int state;
			for(auto it = flying_marbles.begin(); it != flying_marbles.end(); it++)
			{
				state = (*it)->handleStates(deltaT);
				if( state == -1 )
				{
					flying_marbles.erase(it);
					it--;
					continue;
				}
				else if ( state == -2)
				{
					(*it)->render();
					continue;
				}
				else
				{
					flying_marbles.erase(it);
					marblePop(state);
					it--;
					continue;
				}
			}

			
			for(auto it = path_marbles.begin(); it != path_marbles.end(); it++)
			{
				state = (*it)->handlePathState(deltaT);
				if( state == -1 )
				{
					path_marbles.erase(it);
					it--;
					continue;
				}

				if(it + 1 != path_marbles.end() && (*it)->mCurrentWaypoint + DIST >= (*(it+1))->mCurrentWaypoint && (*it)->connected == 1)
					(*(it+1))->connected = 1;
				(*it)->render();
			}



			SDL_RenderPresent(gRenderer);
		}
	}
}



#endif

