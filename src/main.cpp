#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

constexpr int SCREEN_WIDTH{800};
constexpr int SCREEN_HEIGHT{600};
constexpr int maxFrameRate{60}; // Maximum desired frame rate (FPS)
constexpr int maxFrameTime{1000 / maxFrameRate}; // Maximum time allowed per frame (in milliseconds)

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL is not initialized." << SDL_GetError() << '\n';
  } else {
    std::cout << "SDL video system is ready to go.\n";
  }

  SDL_Window *window{nullptr};
  window =
      SDL_CreateWindow("Sprite animation", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (window == nullptr) {
    std::cerr << "Failed to create window: " << SDL_GetError() << '\n';
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (renderer == nullptr) {
    std::cerr << "Failed to create renderer: " << SDL_GetError() << '\n';
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  bool gameIsRunning{true};

  // Define tile map
  Uint32 previousTime = SDL_GetTicks();
  Uint32 lag = 0;

  while (gameIsRunning) {
    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsedTime = currentTime - previousTime;
    previousTime = currentTime;
    lag += elapsedTime;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        gameIsRunning = false;
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          gameIsRunning = false;
          std::cout << "Escape key is pressed.\n";
          break;
        }
      }
    }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
        Uint32 frameTime = SDL_GetTicks() - currentTime;
        if (frameTime < maxFrameTime) {
            SDL_Delay(maxFrameTime - frameTime);
        }
  }
  return 0;
}
