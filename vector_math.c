#include "include/vector_math.h"
#include "include/scene_structs.h"
#include <math.h>

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
