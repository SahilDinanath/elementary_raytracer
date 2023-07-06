#include "../include/scene_structs.h"
#include "../include/vector_math.h"
#include <math.h>
#include <stdio.h>
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

Point clampColor(Point *color) {
  Point clampedColor = *color;
  if (clampedColor.x > 255) {
    clampedColor.x = 255;
  } else if (clampedColor.y > 255) {
    clampedColor.y = 255;
  } else if (clampedColor.z > 255) {
    clampedColor.z = 255;
  }
  return clampedColor;
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
void closestIntersection(Point *point_1, Point *point_2, double minRange,
                         double maxRange, Scene *scene, Sphere **closestSphere,
                         double *closestRange) {
  for (int i = 0; i < scene->amountOfObjectsInScene; i++) {
    double range_1, range_2;
    intersectRay(point_1, point_2, &scene->objectsInScene[i], &range_1,
                 &range_2);
    if ((range_1 >= minRange && range_1 <= maxRange) &&
        range_1 < *closestRange) {
      *closestRange = range_1;
      *closestSphere = &scene->objectsInScene[i];
    }
    if ((range_2 >= minRange && range_2 <= maxRange) &&
        range_2 < *closestRange) {
      *closestRange = range_2;
      *closestSphere = &scene->objectsInScene[i];
    }
  }
}

Point reflectRay(Point *ray, Point *normal) {
  Point reflectedPoint = multiplyVectorByConstant(normal, 2);
  reflectedPoint =
      multiplyVectorByConstant(&reflectedPoint, dotProduct(ray, normal));
  reflectedPoint = minusVectors(&reflectedPoint, ray);
  return reflectedPoint;
}
double computeLighting(Point *point, Point *normal, Scene *scene,
                       Point *pointToCamera, double specular) {
  double intensity = 0;
  for (int i = 0; i < scene->amountOfLightsInScene; i++) {
    Point length;
    double tMax = 0;

    switch (scene->lightsInScene[i].type) {
      // ambient lighting
    case 0:
      intensity += scene->lightsInScene[i].intensity;
      break;
      // point lighting
    case 1:
      length = minusVectors(&scene->lightsInScene[i].position, point);
      tMax = 1;
      break;
      // directional lighting
    case 2:
      length = scene->lightsInScene[i].position;
      tMax = INFINITY;
      break;
    }
    Sphere *shadowSphere = NULL;
    double shadowT = INFINITY;
    closestIntersection(point, &length, 0.001, tMax, scene, &shadowSphere,
                        &shadowT);

    if (shadowSphere != NULL) {
      continue;
    }
    // diffuse
    double normalDot = dotProduct(normal, &length);
    if (normalDot > 0) {
      intensity += scene->lightsInScene[i].intensity * normalDot /
                   (lengthOfVector(normal) * lengthOfVector(&length));
    }
    // specular
    if (scene->objectsInScene[i].specular != -1) {
      Point R = multiplyVectorByConstant(normal, 2);
      R = multiplyVectorByConstant(&R, normalDot);
      R = minusVectors(&R, &length);
      double rDot = dotProduct(&R, pointToCamera);
      if (rDot > 0) {
        intensity += scene->lightsInScene[i].intensity *
                     pow(rDot / (double)(lengthOfVector(&R) *
                                         lengthOfVector(pointToCamera)),
                         specular);
      }
    }
  }
  return intensity;
}

Point traceRay(Point *camera, Point *viewport, Scene *scene, double minRange,
               double maxRange, int recursionDepth) {
  double closestRange = INFINITY;
  Sphere *closestSphere = NULL;
  closestIntersection(camera, viewport, minRange, maxRange, scene,
                      &closestSphere, &closestRange);

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
  Point negativeViewPort = multiplyVectorByConstant(viewport, -1);
  color = multiplyVectorByConstant(
      &color, computeLighting(&point, &normal, scene, &negativeViewPort,
                              closestSphere->specular));

  color = clampColor(&color);

  double reflection = closestSphere->reflective;

  if (recursionDepth <= 0 || reflection <= 0) {
    return color;
  }

  Point reflectedRay = reflectRay(&negativeViewPort, &normal);
  Point reflectedColor = traceRay(&point, &reflectedRay, scene, 0.001, INFINITY,
                                  recursionDepth - 1);

  reflectedRay = multiplyVectorByConstant(&reflectedColor, reflection);
  temp = multiplyVectorByConstant(&color, (1 - reflection));
  return addVectors(&temp, &reflectedRay);
}
