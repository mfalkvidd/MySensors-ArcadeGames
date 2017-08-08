/*
   LICENSE: GPLv2
   Source: https://github.com/hoshi89/MySensors-Snake
   Author https://github.com/hoshi89
*/

#ifndef INCLUDED_SNAKE
#define INCLUDED_SNAKE

#include <FastLED.h>

enum Direction
{
  Up = 0,
  Down = 1,
  Left = 2,
  Right = 3
};

struct vector2d
{
  int x;
  int y;

  vector2d(int xVal, int yVal)
    : x(xVal), y(yVal)
  {
  }

  double dot(const vector2d& rhs)
  {
    return ((x * rhs.x) + (y * rhs.y));
  }

  vector2d add(const vector2d& rhs)
  {
    return vector2d(x + rhs.x, y + rhs.y);
  }

  vector2d operator+(const vector2d& rhs)
  {
    vector2d lhs(x, y);
    lhs.x = rhs.x;
    lhs.y = rhs.y;
    return lhs;
  }
};

struct snake_part
{
  vector2d pos;
  snake_part *next;
  snake_part *prev;
  CRGB color;

  snake_part(vector2d coordinates, snake_part *next_part, snake_part *prev_part)
    : pos(coordinates), next(next_part), prev(prev_part), color(rand() % 256, rand() % 256, rand() % 256)
  {
  }
};

class snake {
  public:
    snake(int startLength, const vector2d &startPos, const vector2d &screenSize);
    void update();
    void changeDirection(const Direction dir);
    snake_part* getHead() {
      return m_head;
    };
    vector2d getFood() {
      return m_food;
    };
  private:
    void addPart();
    void reset();
    void tryFoodSpawn();
    vector2d m_startPos;
    vector2d m_screenSize;
    Direction m_direction;
    snake_part *m_head;
    snake_part *m_tail;
    int m_startLength;

    vector2d m_food;
    boolean m_foodSpawned;
};

#endif

