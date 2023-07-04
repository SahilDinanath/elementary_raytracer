#ifndef SCENE_STRUCTS_H
#define SCENE_STRUCTS_H
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
  double specular;
} Sphere;

typedef struct {
  int amountOfObjectsInScene;
  Sphere *objectsInScene;
  int amountOfLightsInScene;
  Light *lightsInScene;
} Scene;

#endif // !SCENE_STRUCTS_H
