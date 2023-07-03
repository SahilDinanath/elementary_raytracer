#include "scene_structs.h"
#ifndef RAYTRACER_H
#define RAYTRACER_H
Point convertPointFromViewportToScreenCoordSystem(Point *, double, double);
Point convertCanvasToViewport(Point *, double, double, double, double);
Point convertViewportToCanvas(Point *, double, double, double, double);
Point traceRay(Point *, Point *, Scene *, double, double);
void intersectRay(Point *, Point *, Sphere *, double *, double *);
double computeLighting(Point *, Point *, Scene *);

#endif // !RAYTRACER_H
