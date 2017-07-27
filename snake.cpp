#include "snake.h"
#include <stdlib.h>

snake::snake(int startLength, const vector2d &startPos, const vector2d &screenSize)
  : m_startLength(startLength), m_startPos(startPos), m_screenSize(screenSize), m_direction(Up), m_food(0, 0), m_foodSpawned(false)
{
  Serial.println("Constructor");
  m_head = new snake_part(startPos, nullptr, nullptr);
  m_tail = nullptr;

  for (int i = 1; i < startLength; i++) {
    addPart();
  }

  tryFoodSpawn();
}

void snake::update()
{
  // Rearrange the snake
  if (m_tail != nullptr) {
    snake_part *old_head = m_head;
    snake_part *new_tail = m_tail->prev;
    m_head = m_tail;
    m_head->next = old_head;
    old_head->prev = m_head;
    m_head->pos.x = old_head->pos.x;
    m_head->pos.y = old_head->pos.y;
    new_tail->next = nullptr;
    m_tail = new_tail;
  }

  // Set new head position
  switch (m_direction)
  {
    case Right:
      m_head->pos.x++;
      break;
    case Left:
      m_head->pos.x--;
      break;
    case Up:
      m_head->pos.y++;
      break;
    case Down:
      m_head->pos.y--;
      break;
  }

  // Wrap x coordinate
  if (m_head->pos.x >= m_screenSize.x) {
    m_head->pos.x = 0;
  } else if (m_head->pos.x < 0) {
    m_head->pos.x = m_screenSize.x - 1;
  }

  // Wrap y coordinate
  if (m_head->pos.y >= m_screenSize.y) {
    m_head->pos.y = 0;
  } else if (m_head->pos.y < 0) {
    m_head->pos.y = m_screenSize.y - 1;
  }

  tryFoodSpawn();

  // Check collision with self
  snake_part *body_part = m_head->next;
  while (body_part != nullptr) {
    if (body_part->pos.x == m_head->pos.x && body_part->pos.y == m_head->pos.y) {
      // GAME OVER!!
      reset();
      break;
    }
    body_part = body_part->next;
  }

  // Check collision with apple
  if (m_head->pos.x == m_food.x && m_head->pos.y == m_food.y) {
    m_food = vector2d(rand() % m_screenSize.x, rand() % m_screenSize.y);
    addPart();
    m_foodSpawned = false;
  }
}

void snake::changeDirection(const Direction dir)
{
  switch (dir)
  {
    case Up:
      if (m_direction != Down)
        m_direction = dir;
      break;
    case Down:
      if (m_direction != Up)
        m_direction = dir;
      break;
    case Left:
      if (m_direction != Right)
        m_direction = dir;
      break;
    case Right:
      if (m_direction != Left)
        m_direction = dir;
      break;
  }
}

void snake::addPart()
{
  snake_part *new_part = new snake_part(m_head->pos, m_head, nullptr);
  m_head->prev = new_part;
  if (!m_tail) {
    m_tail = m_head;
  }
  m_head = new_part;
}

void snake::reset()
{
  snake_part *curr = m_head;
  while (curr->next != nullptr) {
    snake_part *next = curr->next;
    delete curr;
    curr = next;
  }
  delete curr;

  m_head = new snake_part(m_startPos, nullptr, nullptr);
  m_tail = nullptr;
  m_food = vector2d(rand() % m_screenSize.x, rand() % m_screenSize.y);

  for (int i = 1; i < m_startLength; i++) {
    addPart();
  }
}

void snake::tryFoodSpawn()
{
  if (m_foodSpawned)
    return;

  int x = rand() % m_screenSize.x;
  int y = rand() % m_screenSize.y;
  bool spawn = true;

  snake_part *body_part = m_head->next;
  while (body_part != nullptr) {
    if (body_part->pos.x == x && body_part->pos.y == y) {
      spawn = false;
      break;
    }
    body_part = body_part->next;
  }

  if (spawn) {
    m_food = vector2d(x, y);
    m_foodSpawned = true;
  }
}

