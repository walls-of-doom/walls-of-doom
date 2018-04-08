#ifndef POINT_H
#define POINT_H

class Point {
public:
  Point() = default;
  Point(S32 x, S32 y) : x(x), y(y) {
  }

  S32 x = 0;
  S32 y = 0;
};

#endif
