#include <SDL2/SDL.h>
#include <time.h>
#include <stdio.h>

#define WHITE 0x987f
#define BLACK 0x0000

int main() {
	int MAP_W = 150;
	int MAP_H = 150;

	int WIN_W = 600;
	int WIN_H = 600;

	// init sdl graphics module
	SDL_Init(SDL_INIT_VIDEO);

	// create sdl window
	SDL_Window* window = SDL_CreateWindow("The Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

	// create sdl renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// create texture for storing game situation picture
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA4444, SDL_TEXTUREACCESS_STREAMING, MAP_W, MAP_H);

	// init random
	srand(time(0));

	// previous step map
	Uint16 *map0 = (Uint16*)malloc(MAP_W * MAP_H * sizeof(Uint16));

	// current step map
	Uint16 *map1 = (Uint16*)malloc(MAP_W * MAP_H * sizeof(Uint16));

	void *pixels;
	int pitch = MAP_W;

	// set previous map to 0
	//memset(map0, 0, MAP_W * MAP_H * sizeof(Uint16));
	memset(map1, 0, MAP_W * MAP_H * sizeof(Uint16));

	// initialize the map with either white or black randomly
	for (int i = 0; i < MAP_W * MAP_H; i++) {
		map0[i] = rand() % 6 ? WHITE : BLACK;
	}

	// sdl event handle
	SDL_Event e;

	// quit flag
	int quit = 0;

	// previous entropy
	int prev_transitions = 1;

	while (!quit) {
		// process through window events
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			}

			if (e.type == SDL_KEYDOWN) {
				for (int i = 0; i < MAP_W * MAP_H; i++) {
					map0[i] = rand() % 2 ? WHITE : BLACK;
				}
			}
		}

		// number of transitions
		int transitions = 0;

		// process the next step
		for (int i = 1; i < MAP_W - 1; i++) {
			for (int j = 1; j < MAP_H - 1; j++) {
				// number of neighbors
				int neighbors = 0;

				// count the black neigbors
				for (int k = -1; k <= 1; k++) {
					for (int l = -1; l <= 1; l++) {
						if (k == 0 && l == 0)
							continue;
						if (map0[(i + k) + (j + l) * MAP_W] == BLACK)
							neighbors++;
					}
				}

				// apply the rules
				if (neighbors == 3) {
					map1[i + j * MAP_W] = BLACK;
					transitions++;
				}
				if (neighbors < 2 || neighbors > 3) {
					map1[i + j * MAP_W] = WHITE;
				}
			}
		}

		int entropy = transitions - prev_transitions;

		prev_transitions = transitions;

		//printf("Current entropy: %d \n", entropy);

		SDL_Rect rect;
		rect.x = 20;
		rect.y = 600 - transitions / 20;
		rect.w = 70;
		rect.h = 30;

		SDL_Rect rect2;
		rect2.x = 20;
		rect2.y = 600;
		rect2.w = 70;
		rect2.h = 30;

		// copy map values to the texture
		SDL_LockTexture(texture, NULL, &pixels, &pitch);

		memcpy(pixels, map1, MAP_W * MAP_H * sizeof(Uint16));

		SDL_UnlockTexture(texture);

		// copy current step map to previous step map
		memcpy(map0, map1, MAP_W * MAP_H * sizeof(Uint16));

		/* render map */

		// copy pixel data to the videocard
		SDL_RenderCopy(renderer, texture, NULL, NULL);

		SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0xff);
		SDL_RenderFillRect(renderer, &rect2);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0xff, 0xff);
		SDL_RenderFillRect(renderer, &rect);


		// draw
		SDL_RenderPresent(renderer);

		//SDL_Delay(3000);
	}

	return 0;
}