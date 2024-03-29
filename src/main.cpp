#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_video.h>
#include <iostream>
#include <vector>
constexpr int SCREEN_WIDTH{800};
constexpr int SCREEN_HEIGHT{600};
constexpr int maxFrameRate{60}; // Maximum desired frame rate (FPS)
constexpr int maxFrameTime{
    1000 / maxFrameRate}; // Maximum time allowed per frame (in milliseconds)

constexpr int IDLE_PLAYER_WIDTH {32};
constexpr int IDLE_PLAYER_HEIGHT {32};
constexpr int RUN_PLAYER_WIDTH {64};
constexpr int RUN_PLAYER_HEIGHT{64};
// Define structure for tiles
struct Tile {
  SDL_Rect rect;
  bool isSolid; // Indicates whether the tile is solid (for collision detection)
};
int main() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL video is not initialized." << SDL_GetError() << '\n';
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
  playerSurface =
      IMG_Load("../assets/pixel_crawler/Heroes/Knight/Idle/Idle-Sheet.png");

  SDL_Texture *playerTexture{nullptr};
  playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);

  SDL_FreeSurface(playerSurface);

  // Define the number of frames in the run animation and the time per frame
  int idleFrameNum = 4;
  int runFrameNum = 6;
  int frameTime = 100; // in milliseconds
  int currentFrame {0};
  Uint32 lastFrameTime{0};

  SDL_Surface *playerRunSurface = nullptr;
  playerRunSurface =
      IMG_Load("../assets/pixel_crawler/Heroes/Knight/Run/Run-Sheet.png");

  SDL_Texture *playerRunTexture = nullptr;
  playerRunTexture = SDL_CreateTextureFromSurface(renderer, playerRunSurface);
  SDL_FreeSurface(playerRunSurface);

  SDL_Rect playerSourceRect{0, 0, IDLE_PLAYER_WIDTH, IDLE_PLAYER_HEIGHT};
  SDL_Rect playerDestRect{100, 100,IDLE_PLAYER_WIDTH, IDLE_PLAYER_HEIGHT};

  int playerVelocityX{0};
  int playerVelocityY{0};
  bool isJumping{false};
  int jumpForce{30};
  int gravity{1};


  int currentFrameIdle = 0;
  int currentFrameRun = 0;
  SDL_Rect playerSourceRectRun{0, 0, RUN_PLAYER_WIDTH, RUN_PLAYER_HEIGHT};
  SDL_Rect playerDestRectRun{0, 0, RUN_PLAYER_WIDTH, RUN_PLAYER_HEIGHT};
  bool isMoving = false;
  


SDL_RendererFlip flipType = SDL_FLIP_NONE;
  std::vector<Tile> tileMap;

  // Define ground platform
  tileMap.push_back({{0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50}, true});

  // Define upper platforms
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
        case SDLK_s:
          break;
        case SDLK_a:
          playerVelocityX = -5;
            isMoving = true;

        flipType = SDL_FLIP_HORIZONTAL; // Set to flip horizontally when moving left
          break;
        case SDLK_d:
          playerVelocityX = 5;
            isMoving = true;

        flipType = SDL_FLIP_NONE; // Set to flip horizontally when moving left
          break;
        default:
          break;
        }
      } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
        case SDLK_w:
          break;

        case SDLK_s:
          break;

        case SDLK_a:
        case SDLK_d:
          playerVelocityX = 0;
            isMoving = false;
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
      // Land on the ground and check collision with platforms
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

    // Update animation frame
    // if (currentTime - lastFrameTime > frameTime) {
    //   currentFrame = (currentFrame + 1) % numFrames;
    //   lastFrameTime = currentTime;
    //   playerSourceRect.x = currentFrame * playerSourceRect.w;
    // }
  // Update player position
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


playerDestRect.w = isMoving ? RUN_PLAYER_WIDTH : IDLE_PLAYER_WIDTH;
playerDestRect.h = isMoving ? RUN_PLAYER_HEIGHT : IDLE_PLAYER_HEIGHT;
    if (currentTime - lastFrameTime > frameTime) {
      if (isMoving) {
        currentFrameRun = (currentFrameRun + 1) % runFrameNum;
        playerSourceRectRun.x = currentFrameRun * playerSourceRectRun.w;
      } else {
        currentFrame = (currentFrame + 1) % idleFrameNum;
        playerSourceRect.x = currentFrame * playerSourceRect.w;
      }
      lastFrameTime = currentTime;
    }
float scaleFactor = 2.0f; // Example: double the size
playerDestRect.w *= scaleFactor;
playerDestRect.h *= scaleFactor;


    // Update and render idle animation
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);


    SDL_RenderClear(renderer);

    // if (isMoving) {
    //   SDL_RenderCopy(renderer, playerRunTexture, &playerSourceRectRun,
    //                  &playerDestRect);
    // } else {
    //   SDL_RenderCopy(renderer, playerTexture, &playerSourceRect,
    //                  &playerDestRect);
    // }
if (isMoving) {
    SDL_RenderCopyEx(renderer, playerRunTexture, &playerSourceRectRun,
                     &playerDestRect, 0, NULL, flipType);
} else {
    SDL_RenderCopyEx(renderer, playerTexture, &playerSourceRect,
                     &playerDestRect, 0, NULL, flipType);
}
    // SDL_RenderCopy(renderer, playerTexture, &playerSourceRect, &playerDestRect);

    // Draw the tile map
    for (const auto &tile : tileMap) {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
      SDL_RenderFillRect(renderer, &tile.rect);
    }
    // Present the frame
    SDL_RenderPresent(renderer);

    // Delay to control frame rate
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
