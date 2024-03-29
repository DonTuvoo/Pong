#include <stdio.h>
#include <SDL.h>
#include <stdbool.h> 

#include ".\constants.h"

int game_is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;


int last_frame_time = 0;




struct game_object
{
	float x;
	float y;
	float width;
	float height;
	float vel_x;
	float vel_y;

} ball, paddle;

int initialize_window(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error inicializando SDL. \n");
		return false;
	}
	window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);

	if (!window)
	{
		fprintf(stderr, "Error creando ventana SDL. \n");
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		fprintf(stderr, "Error creando render SDL. \n");
		return false;
	}

	return true;
}

void process_input(void)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		game_is_running = false;
		break;

	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE)
		{
			game_is_running = false;
			break;
		}
		if (event.key.keysym.sym == SDLK_LEFT)
		{
			paddle.vel_x = -400; 
		}
		if (event.key.keysym.sym == SDLK_RIGHT)
		{
			paddle.vel_x = +400;
		}
		break;

	case SDL_KEYUP:
		if (event.key.keysym.sym == SDLK_LEFT)
		{
			paddle.vel_x = 0;
		}
		if (event.key.keysym.sym == SDLK_RIGHT)
		{
			paddle.vel_x = 0;
		}
		break;



	}

}

void setup(void)
{

	ball.width = 15;
	ball.height = 15;
	ball.x = 20;
	ball.y = 20;
	ball.vel_x = 300;
	ball.vel_y = 300;



	paddle.width = 100;
	paddle.height = 20;
	paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
	paddle.y = WINDOW_HEIGHT - 40;
	paddle.vel_x = 0;
	paddle.vel_y = 0;

}

void update(void) {
	// Calculate how much we have to wait until we reach the target frame time
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

	// Only delay if we are too fast too update this frame
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
		SDL_Delay(time_to_wait);

	// Get a delta time factor converted to seconds to be used to update my objects
	float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;

	// Store the milliseconds of the current frame
	last_frame_time = SDL_GetTicks();

	// update ball and paddle position
	ball.x += ball.vel_x * delta_time;
	ball.y += ball.vel_y * delta_time;
	paddle.x += paddle.vel_x * delta_time;
	paddle.y += paddle.vel_y * delta_time;

	// Check for ball collision with the walls
	if (ball.x <= 0 || ball.x + ball.width >= WINDOW_WIDTH)
		ball.vel_x = -ball.vel_x;
	if (ball.y < 0)
		ball.vel_y = -ball.vel_y;

	// Check for ball collision with the paddle
	if (ball.y + ball.height >= paddle.y && ball.x + ball.width >= paddle.x && ball.x <= paddle.x + paddle.width)
		ball.vel_y = -ball.vel_y;

	// Prevent paddle from moving outside the boundaries of the window
	if (paddle.x <= 0)
		paddle.x = 0;
	if (paddle.x >= WINDOW_WIDTH - paddle.width)
		paddle.x = WINDOW_WIDTH - paddle.width;

	// Check for game over
	if (ball.y + ball.height > WINDOW_HEIGHT) {
		ball.x = WINDOW_WIDTH / 2;
		ball.y = 0;
	}
}

void render(void)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //color ventana
	SDL_RenderClear(renderer);


	SDL_Rect ball_rect = { (int)ball.x, (int)ball.y, (int)ball.width, (int)ball.height };

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //color pelotita
	SDL_RenderFillRect(renderer, &ball_rect); //por que tengo que poner una &?

	//para dibujar el rectangulito
	SDL_Rect paddle_rect = {
		(int)paddle.x,
		(int)paddle.y,
		(int)paddle.width,
		(int)paddle.height
	};
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &paddle_rect);

	SDL_RenderPresent(renderer);
}

void destroy_window(void)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	game_is_running = initialize_window();

	setup();

	while (game_is_running)
	{
		process_input();
		update();
		render();
	}

	destroy_window();

	return 0;
}