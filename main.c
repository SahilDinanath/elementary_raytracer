#include <SDL2/SDL_render.h>
#include <stdlib.h>
#ifdef _WIN32
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include "include/raytracer.h"
#include "include/scene_structs.h"
#include "include/vector_math.h"
#include <math.h>
#include <stdio.h>
#define CANVAS_WIDTH 600
#define CANVAS_HEIGHT 600
#define VIEWPORT_WIDTH 1
#define VIEWPORT_HEIGHT 1
#define DELAY 3000

int main(int argc, char *argv[]) { // code to render window
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL failed to initialise %s\n", SDL_GetError());
    return 1;
  }

  window =
      SDL_CreateWindow("RayTracer", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, CANVAS_WIDTH, CANVAS_HEIGHT, 0);
  renderer = SDL_CreateRenderer(
      window, 0, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);

  if (window == NULL) {
    fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
    return 1;
  }

  if (renderer == NULL) {
    fprintf(stderr, "SDL renderer failed to initialise: %s\n", SDL_GetError());
    return 1;
  }

  // sets the render colour to black then clears the renderer
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // sets the render colour to white to draw

  // initialises scenes
  Point camera = {.x = 0, .y = 0, .z = 0};
  Sphere sphere_1 = {.center = {.x = 0, .y = -1, .z = 3},
                     .radius = 1,
                     .color = {.x = 255, .y = 0, .z = 0}};

  Sphere sphere_2 = {.center = {.x = 2, .y = 0, .z = 4},
                     .radius = 1,
                     .color = {.x = 0, .y = 0, .z = 255}};
  Sphere sphere_3 = {.center = {.x = -2, .y = 0, .z = 4},
                     .radius = 1,
                     .color = {.x = 0, .y = 255, .z = 0}};
  Sphere sphere_4 = {.center = {.x = 0, .y = -5001, .z = 0},
                     .radius = 5000,
                     .color = {.x = 255, .y = 255, .z = 0}};
  Scene scene = {.amountOfObjectsInScene = 4,
                 .objectsInScene = (Sphere *)malloc(4 * sizeof(Sphere)),
                 .amountOfLightsInScene = 4,
                 .lightsInScene = (Light *)malloc(4 * sizeof(Light))};

  scene.objectsInScene[0] = sphere_1;
  scene.objectsInScene[1] = sphere_2;
  scene.objectsInScene[2] = sphere_3;
  scene.objectsInScene[3] = sphere_4;

  // initialises lighting in scene
  Light light_1 = {.type = 0, .intensity = 0.2};

  Light light_2 = {
      .type = 1, .intensity = 0.6, .position = {.x = 2, .y = 1, .z = 0}};

  Light light_3 = {
      .type = 2, .intensity = 0.2, .position = {.x = 1, .y = 4, .z = 4}};

  scene.lightsInScene[0] = light_1;
  scene.lightsInScene[1] = light_2;
  scene.lightsInScene[2] = light_3;

  for (int x = -CANVAS_WIDTH / 2; x < CANVAS_WIDTH / 2; x++) {
    for (int y = -CANVAS_HEIGHT / 2; y < CANVAS_HEIGHT / 2; y++) {
      Point point = {.x = x, .y = y};
      point = convertCanvasToViewport(&point, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
                                      CANVAS_WIDTH, CANVAS_HEIGHT);
      Point color = traceRay(&camera, &point, &scene, 1, INFINITY);
      point = convertViewportToCanvas(&point, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
                                      CANVAS_WIDTH, CANVAS_HEIGHT);
      point = convertPointFromViewportToScreenCoordSystem(&point, CANVAS_WIDTH,
                                                          CANVAS_HEIGHT);
      SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);
      SDL_RenderDrawPoint(renderer, point.x, point.y);
    }
  }

  SDL_RenderPresent(renderer);
  SDL_Delay(DELAY);
  SDL_DestroyWindow(window);
  SDL_Quit();
  // rendering window ends here
  return 0;
}
