/* Sami Nasry - Pong Game - 2024*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 100
#define BALL_SIZE 10 
#define PADDLE_SPEED 5
#define BALL_SPEED 5


// PADDLE STRUCTURE
typedef struct {
	SDL_Rect rect;
	int dy;
} Paddle;


// BALL STRUCTURE
typedef struct {
	SDL_Rect rect;
	int dx,dy;
} Ball;


// RESET BALL POSITION TO THE CENTER OF THE SCREEN AND RANDOME START DIRECTION
void reset_ball(Ball* ball)
{
	ball->rect.x = (WINDOW_WIDTH / 2) - (BALL_SIZE / 2);
	ball->rect.y = (WINDOW_HEIGHT / 2) - (BALL_SIZE / 2);
	ball->dx = BALL_SPEED * (rand() % 2 == 0 ? 1 : -1);
	ball->dy = BALL_SPEED * (rand() % 2 == 0 ? 1 : -1);
}


// MAIN GAME LOGIC

int main(int argc, char* argv[])
{

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	TTF_Font* font = NULL;
	SDL_Surface* surface = NULL;
	SDL_Texture* texture = NULL;
	bool quit = false;
	SDL_Event event;

	// PADDLE, BALL AND SCORE INITIALIZATION
	Paddle paddle1 = {{20, (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT/2), PADDLE_WIDTH, PADDLE_HEIGHT}, 0};
	Paddle paddle2 = {{WINDOW_WIDTH - 30, (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2), 
	PADDLE_WIDTH, PADDLE_HEIGHT}, 0};
	Ball ball = {{WINDOW_WIDTH / 2 - BALL_SIZE / 2, WINDOW_HEIGHT / 2 - BALL_SIZE / 2, BALL_SIZE, BALL_SIZE}, BALL_SPEED, BALL_SPEED};
	int score1 = 0, score2 = 0;
	
	// INITIALIZATION OF VIDEO SUBSYTEM AND SDL_TTF LIBRARY

	if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0 )
	{
		printf("SDL/TTF could not initialize! ERROR : %s\n", SDL_GetError());
		return 1;
	}

	// WINDOW CREATION

	window = SDL_CreateWindow("Pong" , SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED
			, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if(!window)
	{
		printf("Window could not be created! Error : %s\n" , SDL_GetError());
		return 1;
	}

	// RENDERER CREATION 
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if(!renderer)
	{
		printf("Renderer could not be created! Error: %s\n", SDL_GetError());
		return 1;
	}

	// FONT LOADING

	font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24);
	if (!font)
	{
		printf("Failed to load font! Error : %s\n", TTF_GetError());
		return 1;

	}


	// Main Game Logic
	
	// POLL FOR PENDING EVENTS. HANDLEE QUIT EVENTS AND KEYBOARD EVENTS FOR PADDLE MOVEMENTS
	while(!quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		 
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_w: paddle1.dy = -PADDLE_SPEED; break;
					case SDLK_s: paddle1.dy = PADDLE_SPEED; break;
					case SDLK_UP: paddle2.dy = -PADDLE_SPEED; break;
					case SDLK_DOWN: paddle2.dy = PADDLE_SPEED; break;
				}	
			}		 
			else if (event.type == SDL_KEYUP)
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_w:
					case SDLK_s: paddle1.dy = 0;break;
					case SDLK_UP:
					case SDLK_DOWN: paddle2.dy = 0; break;
				}
			}


		}

	// MOVE THE PADDLES 
	paddle1.rect.y += paddle1.dy;
	paddle2.rect.y += paddle2.dy;

	// CONSTRAIN PADDLES TO THE WINDOW	 

	if (paddle1.rect.y < 0) paddle1.rect.y = 0;
	if (paddle1.rect.y > (WINDOW_HEIGHT - PADDLE_HEIGHT)) paddle1.rect.y = (WINDOW_HEIGHT - PADDLE_HEIGHT);
	if (paddle2.rect.y < 0) paddle2.rect.y = 0;
	if (paddle2.rect.y > (WINDOW_HEIGHT - PADDLE_HEIGHT)) paddle2.rect.y = (WINDOW_HEIGHT - PADDLE_HEIGHT);


	// MOVE THE BALL 
	ball.rect.x += ball.dx;
	ball.rect.y += ball.dy;


	// HANDLE BALL COLLISION WITH TOP AND BOTTOM OF THE SCREEN 
	if (ball.rect.y <= 0 || ball.rect.y >= WINDOW_HEIGHT - BALL_SIZE)
	{
		ball.dy = -ball.dy;
	}

	// HANDLE BALL COLLISION WITH THE PADDLES
	if (SDL_HasIntersection(&ball.rect, &paddle1.rect) || SDL_HasIntersection(&ball.rect, &paddle2.rect))
	{
		ball.dx = -ball.dx;
	}

	// BALL GOAL HANDLING
	if (ball.rect.x <= 0)
	{
		score2++;
		reset_ball(&ball);
	}
	else if (ball.rect.x >= WINDOW_WIDTH - BALL_SIZE)
	{
		score1++;
		reset_ball(&ball);
	}

	// RENDERING PROCESS
			// SET DRAW COLOR TO BLACK AND CLEAR THE SCREEN 
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

			// SET THE DRAW COLOR TO WHITE AND THEN DRAWW THE PADDLES AND THE BALLS
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &paddle1.rect);
	SDL_RenderFillRect(renderer, &paddle2.rect);
	SDL_RenderFillRect(renderer, &ball.rect);

			// RENDER THE SCORE TEXT  
	char score_text[64];
	snprintf(score_text, sizeof(score_text), "%d - %d" , score1 , score2);
	SDL_Color white = {255, 255, 255};
	surface = TTF_RenderText_Solid(font, score_text, white);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect score_rect = {WINDOW_WIDTH / 2 - surface->w / 2, 20, surface->w,
		surface->h};
	SDL_RenderCopy(renderer, texture, NULL, &score_rect);

	// UPDATE SCREEN WITH RENDERED STUFF 
	SDL_RenderPresent(renderer);

	// FREE THE SURFACE AND DESTROY TEXTURE AFTER EACH FRAME TO PREVENT MEMORY LEAKS
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);

	// FPS CONTROL
	SDL_Delay(16); // Cap at 60 fps

	}	
	 
	

// CLEAN UP 

TTF_CloseFont(font);
SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
TTF_Quit();
SDL_Quit();

return 0;

}












