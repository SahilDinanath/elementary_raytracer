#include <SDL2/SDL_render.h>
#include <stdlib.h>
#ifdef _WIN32
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <math.h>
#include <stdio.h>
#define CANVAS_WIDTH 600
#define CANVAS_HEIGHT 600
#define VIEWPORT_WIDTH 1
#define VIEWPORT_HEIGHT 1
#define DELAY 3000

typedef struct {
  long double x, y, z;
} Point;

typedef struct {
  // type ambient = 0; point = 1; directional = 2;
  int type;
  float intensity;
  Point position;
} Light;

typedef struct {
  Point center;
  double radius;
  Point color;
} Sphere;

typedef struct {
  int amountOfObjectsInScene;
  Sphere *objectsInScene;
  int amountOfLightsInScene;
  Light *lightsInScene;
} Scene;

void convertViewportToCanvas(Point *);
void convertPointFromViewportToScreenCoordSystem(Point *);
void convertCanvasToViewport(Point *);
Point traceRay(Point *, Point *, Scene *, double, double);
void intersectRay(Point *, Point *, Sphere *, double *, double *);
double computeLighting(Point *, Point *, Scene *);
double lengthOfVector(Point *);
Point minusVectors(Point *, Point *);
Point addVectors(Point *, Point *);
Point multiplyVectorByConstant(Point *, double);

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
      convertCanvasToViewport(&point);
      Point color = traceRay(&camera, &point, &scene, 1, INFINITY);
      convertViewportToCanvas(&point);
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

// rendering scene functions
void convertPointFromViewportToScreenCoordSystem(Point *viewPointCoord) {
  viewPointCoord->x += CANVAS_WIDTH / 2.0;
  viewPointCoord->y = CANVAS_HEIGHT / 2.0 - viewPointCoord->y - 1;
}
void convertCanvasToViewport(Point *canvasPoint) {
  canvasPoint->x *= VIEWPORT_WIDTH / (double)CANVAS_WIDTH;
  canvasPoint->y *= VIEWPORT_HEIGHT / (double)CANVAS_HEIGHT;
  canvasPoint->z = 1;
}
void convertViewportToCanvas(Point *canvasPoint) {
  canvasPoint->x /= VIEWPORT_WIDTH / (double)CANVAS_WIDTH;
  canvasPoint->y /= VIEWPORT_HEIGHT / (double)CANVAS_HEIGHT;
  canvasPoint->z = 1;
}
Point traceRay(Point *camera, Point *viewport, Scene *scene, double minRange,
               double maxRange) {
  double closestRange = INFINITY;
  Sphere *closestSphere = NULL;

  for (int i = 0; i < scene->amountOfObjectsInScene; i++) {
    double range_1, range_2;
    intersectRay(camera, viewport, &scene->objectsInScene[i], &range_1,
                 &range_2);

    if ((range_1 >= minRange && range_1 <= maxRange) &&
        range_1 < closestRange) {
      closestRange = range_1;
      closestSphere = &scene->objectsInScene[i];
    }
    if ((range_2 >= minRange && range_2 <= maxRange) &&
        range_2 < closestRange) {
      closestRange = range_2;
      closestSphere = &scene->objectsInScene[i];
    }
  }
  // background color
  Point color = {.x = 255, .y = 255, .z = 255};

  if (closestSphere == NULL) {
    return color;
  }

  color = closestSphere->color;
  Point temp = multiplyVectorByConstant(viewport, closestRange);
  Point point = addVectors(camera, &temp);
  Point normal = minusVectors(&point, &closestSphere->center);

  normal = multiplyVectorByConstant(&normal, 1 / lengthOfVector(&normal));
  return multiplyVectorByConstant(&color,
                                  computeLighting(&point, &normal, scene));
};

Point minusVectors(Point *vector_1, Point *vector_2) {
  Point vectorResult = {.x = vector_1->x - vector_2->x,
                        .y = vector_1->y - vector_2->y,
                        .z = vector_1->z - vector_2->z};
  return vectorResult;
}
double dotProduct(Point *vector_1, Point *vector_2) {
  double result = vector_1->x * vector_2->x + vector_1->y * vector_2->y +
                  vector_1->z * vector_2->z;

  return result;
}

void intersectRay(Point *camera, Point *viewport, Sphere *sphere,
                  double *range_1, double *range_2) {
  double r = sphere->radius;
  Point CO = minusVectors(camera, &sphere->center);

  double a, b, c, discriminant;
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
  *range_2 = (-b - sqrt(discriminant)) / (2 * a);
}

// lighting functions
double lengthOfVector(Point *vector) {
  return sqrt(dotProduct(vector, vector));
}
Point addVectors(Point *vector_1, Point *vector_2) {
  Point temp = {.x = vector_1->x + vector_2->x,
                .y = vector_1->y + vector_2->y,
                .z = vector_1->z + vector_2->z};

  return temp;
};

Point multiplyVectorByConstant(Point *vector_1, double constant) {
  Point temp = {.x = vector_1->x * constant,
                .y = vector_1->y * constant,
                .z = vector_1->z * constant};
  return temp;
};
double computeLighting(Point *point, Point *normal, Scene *scene) {
  double intensity = 0;
  for (int i = 0; i < scene->amountOfLightsInScene; i++) {
    Point length;

    switch (scene->lightsInScene[i].type) {
    case 0:
      intensity += scene->lightsInScene[i].intensity;
      break;
    case 1:
      length = minusVectors(&scene->lightsInScene[i].position, point);
      break;
    case 2:
      length = scene->lightsInScene[i].position;
      break;
    }
    double normalDot = dotProduct(normal, &length);
    if (normalDot > 0) {
      intensity += scene->lightsInScene[i].intensity * normalDot /
                   (lengthOfVector(normal) * lengthOfVector(&length));
    }
  }
  return intensity;
}
