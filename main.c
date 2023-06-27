#include <SDL2/SDL_render.h>
#include <stdlib.h>
#ifdef _WIN32
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <math.h>
#include <stdio.h>
#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600
#define VIEWPORT_WIDTH 800
#define VIEWPORT_HEIGHT 600
#define DELAY 3000
struct Point {
  int x, y, z;
};
typedef struct Point Point;

struct Sphere {
  Point center;
  double radius;
  Point color;
};
typedef struct Sphere Sphere;

struct Scene {
  int amountOfObjectsInScene;
  Sphere *objectsInScene;
};
typedef struct Scene Scene;

void convertViewportToCanvas(Point *);
void convertPointFromViewportToScreenCoordSystem(Point *);
void convertCanvasToViewport(Point *);
Point traceRay(Point *, Point *, Scene *, int, int);
void intersectRay(Point *, Point *, Sphere *, int *, int *);

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
  renderer = SDL_CreateRenderer(window, 0, 0);

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

  Point camera;
  camera.x = 0;
  camera.y = 0;
  camera.z = 0;
  Scene scene;

  scene.amountOfObjectsInScene = 1;
  scene.objectsInScene =
      (Sphere *)malloc(scene.amountOfObjectsInScene * sizeof(Sphere));
  Sphere sphere;
  Point sphereCenter;
  sphereCenter.x = 0;
  sphereCenter.y = -1;
  sphereCenter.z = 3;

  sphere.center = sphereCenter;
  sphere.radius = 1;
  Point sphereColor;
  sphereColor.x = 255;
  sphereColor.y = 0;
  sphereColor.z = 0;

  sphere.color = sphereColor;
  scene.objectsInScene[0] = sphere;

  for (int x = -CANVAS_WIDTH / 2; x < CANVAS_WIDTH / 2; x++) {
    for (int y = -CANVAS_WIDTH / 2; y < CANVAS_WIDTH / 2; y++) {
      Point point;
      point.x = x;
      point.y = y;
      convertCanvasToViewport(&point);
      Point color = traceRay(&camera, &point, &scene, 1, INFINITY);
      // convertViewportToCanvas(&point);
      convertPointFromViewportToScreenCoordSystem(&point);
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

void convertPointFromViewportToScreenCoordSystem(Point *viewPointCoord) {
  viewPointCoord->x += CANVAS_WIDTH / 2;
  viewPointCoord->y += CANVAS_HEIGHT / 2;
}
void convertCanvasToViewport(Point *canvasPoint) {
  canvasPoint->x *= VIEWPORT_WIDTH / CANVAS_WIDTH;
  canvasPoint->y *= VIEWPORT_HEIGHT / CANVAS_WIDTH;
  canvasPoint->z = 1;
}
void convertViewportToCanvas(Point *canvasPoint) {
  canvasPoint->x /= VIEWPORT_WIDTH / CANVAS_WIDTH;
  canvasPoint->y /= VIEWPORT_HEIGHT / CANVAS_WIDTH;
  canvasPoint->z = 1;
}
Point traceRay(Point *camera, Point *viewport, Scene *scene, int minRange,
               int maxRange) {
  int closestRange = INFINITY;
  Sphere *closestSphere = NULL;

  for (int i = 0; i < scene->amountOfObjectsInScene; i++) {
    int range_1;
    int range_2;
    intersectRay(camera, viewport, &scene->objectsInScene[i], &range_1,
                 &range_2);

    if ((range_1 > minRange || range_1 < maxRange) && range_1 < closestRange) {
      closestRange = range_1;
      closestSphere = &scene->objectsInScene[i];
    }
    if ((range_2 > minRange || range_2 < maxRange) && range_2 < closestRange) {
      closestRange = range_2;
      closestSphere = &scene->objectsInScene[i];
    }
  }
  Point color;
  if (closestSphere == NULL) {

    color.x = 255;
    color.y = 255;
    color.z = 255;
    return color;
  }
  color = closestSphere->color;
  return color;
};

Point minusVectors(Point *vector_1, Point *vector_2) {
  Point vectorResult;
  vectorResult.x = vector_1->x - vector_2->x;
  vectorResult.y = vector_1->y - vector_2->y;
  vectorResult.z = vector_1->z - vector_2->z;
  return vectorResult;
}
int dotProduct(Point *vector_1, Point *vector_2) {
  return vector_1->x * vector_2->x + vector_1->y * vector_2->y +
         vector_1->z * vector_2->z;
}

void intersectRay(Point *camera, Point *viewport, Sphere *sphere, int *range_1,
                  int *range_2) {
  int r = sphere->radius;
  Point CO = minusVectors(camera, &sphere->center);

  int a, b, c, discriminant;
  a = dotProduct(viewport, viewport);
  b = 2 * dotProduct(&CO, viewport);
  c = dotProduct(&CO, &CO) - r * r;

  discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    *range_1 = INFINITY;
    *range_2 = INFINITY;
    return;
  }
  *range_1 = (-b + sqrt(discriminant)) / (2 * a);
  *range_1 = (-b + sqrt(discriminant)) / (2 * a);
}

void render() {}
