
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>

constexpr int SCREEN_WIDTH{800};
constexpr int SCREEN_HEIGHT{600};
constexpr int maxFrameRate{60}; // Maximum desired frame rate (FPS)
constexpr int maxFrameTime{1000 / maxFrameRate}; // Maximum time allowed per frame (in milliseconds)

struct Tile {
  SDL_Rect rect;
  bool isSolid; // Indicates whether the tile is solid (for collision detection)
};

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL video is not initialized." << SDL_GetError() << '\n';
    return 1;
  } else {
    std::cout << "SDL video system is ready to go.\n";
  }

  SDL_Window *window{nullptr};
  window = SDL_CreateWindow("Sprite animation", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (window == nullptr) {
    std::cerr << "Failed to create window: " << SDL_GetError() << '\n';
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer{nullptr};
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == nullptr) {
    std::cerr << "Failed to create renderer: " << SDL_GetError() << '\n';
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_Surface *playerSurface{nullptr};
  playerSurface = IMG_Load("../assets/pixel_crawler/Heroes/Knight/Idle/Idle-Sheet.png");

  if (playerSurface == nullptr) {
    std::cerr << "Failed to load player surface: " << IMG_GetError() << '\n';
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_Texture *playerTexture{nullptr};
  playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);

  if (playerTexture == nullptr) {
    std::cerr << "Failed to create player texture: " << SDL_GetError() << '\n';
    SDL_FreeSurface(playerSurface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_FreeSurface(playerSurface);

  int numFrames = 4;
  int frameTime = 100; // in milliseconds
  int currentFrame{0};
  Uint32 lastFrameTime{0};

  SDL_Rect playerSourceRect{0, 0, 32, 32};
  SDL_Rect playerDestRect{100, 100, 100, 100};

  int playerVelocityX{0};
  int playerVelocityY{0};
  bool isJumping{false};
  int jumpForce{30};
  int gravity{1};

  std::vector<Tile> tileMap;

  tileMap.push_back({{0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50}, true});
  tileMap.push_back({{100, 370, 200, 20}, true});
  tileMap.push_back({{400, 400, 200, 20}, true});

  bool gameIsRunning{true};

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
          case SDLK_w:
            if (!isJumping) {
              isJumping = true;
              playerVelocityY = -jumpForce;
            }
            break;
          case SDLK_a:
            playerVelocityX = -5;
            break;
          case SDLK_d:
            playerVelocityX = 5;
            break;
          default:
            break;
        }
      } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
          case SDLK_a:
          case SDLK_d:
            playerVelocityX = 0;
            break;
          default:
            break;
        }
      }
    }

    playerVelocityY += gravity;
    while (lag >= maxFrameTime) {
      if (isJumping) {
        playerVelocityY += gravity;
      }
      playerDestRect.y += playerVelocityY;

      for (const auto &tile : tileMap) {
        if (SDL_HasIntersection(&playerDestRect, &tile.rect)) {
          if (playerVelocityY > 0) {
            playerDestRect.y = tile.rect.y - playerDestRect.h;
            playerVelocityY = 0;
            isJumping = false;
          } else if (playerVelocityY < 0) {
            playerDestRect.y = tile.rect.y + tile.rect.h;
            playerVelocityY = 0;
          }
        }
      }

      playerDestRect.x += playerVelocityX;
      if (playerDestRect.x < 0) {
        playerDestRect.x = 0;
      } else if (playerDestRect.x + playerDestRect.w > SCREEN_WIDTH) {
        playerDestRect.x = SCREEN_WIDTH - playerDestRect.w;
      }
      lag -= maxFrameTime;
    }

    if (currentTime - lastFrameTime > frameTime) {
      currentFrame = (currentFrame + 1) % numFrames;
      lastFrameTime = currentTime;
      playerSourceRect.x = currentFrame * playerSourceRect.w;
    }

    playerVelocityY += gravity;
    playerDestRect.y += playerVelocityY;
    for (const auto &tile : tileMap) {
      if (SDL_HasIntersection(&playerDestRect, &tile.rect)) {
        if (playerVelocityY > 0) {
          playerDestRect.y = tile.rect.y - playerDestRect.h;
          playerVelocityY = 0;
          isJumping = false;
        } else if (playerVelocityY < 0) {
          playerDestRect.y = tile.rect.y + tile.rect.h;
          playerVelocityY = 0;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, playerTexture, &playerSourceRect, &playerDestRect);

    for (const auto &tile : tileMap) {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
      SDL_RenderFillRect(renderer, &tile.rect);
    }

    SDL_RenderPresent(renderer);

    Uint32 frameTime = SDL_GetTicks() - currentTime;
    if (frameTime < maxFrameTime) {
      SDL_Delay(maxFrameTime - frameTime);
    }
  }

  SDL_DestroyTexture(playerTexture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
