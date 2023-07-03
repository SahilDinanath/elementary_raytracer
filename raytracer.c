#include "include/scene_structs.h"
#include "include/vector_math.h"
#include <math.h>
#include <stdlib.h>

// rendering scene functions
Point convertPointFromViewportToScreenCoordSystem(Point *viewPointCoord,
                                                  double canvasWidth,
                                                  double canvasHeight) {
  Point screenCoordPoint = {.x = viewPointCoord->x + canvasWidth / 2.0,
                            .y = canvasHeight / 2.0 - viewPointCoord->y - 1};
  return screenCoordPoint;
}

Point convertCanvasToViewport(Point *canvasPoint, double viewPortWidth,
                              double viewPortHeight, double canvasWidth,
                              double canvasHeight) {
  Point viewPortCoord = {
      .x = canvasPoint->x * (viewPortWidth / (double)canvasWidth),
      .y = canvasPoint->y * (viewPortHeight / (double)canvasHeight),
      .z = 1};
  return viewPortCoord;
}

Point convertViewportToCanvas(Point *canvasPoint, double viewPortWidth,
                              double viewPortHeight, double canvasWidth,
                              double canvasHeight) {
  Point canvasCoord = {
      .x = canvasPoint->x / (viewPortWidth / (double)canvasWidth),
      .y = canvasPoint->y / (viewPortHeight / (double)canvasHeight),
      .z = 1};
  return canvasCoord;
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
