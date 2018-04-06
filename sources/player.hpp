#ifndef PLAYER_H
#define PLAYER_H

#include "command.hpp"
#include "constants.hpp"
#include "graphics.hpp"
#include "investment.hpp"
#include "perk.hpp"
#include "score.hpp"

class Player {
public:
  std::string name;

  CommandTable *table;

  int x;
  int y;
  int w;
  int h;
  int speed_x;
  int speed_y;

  /* Whether or not the player is being affected by physics. */
  bool physics;

  int can_double_jump;
  int remaining_jump_height;

  int lives;

  Score score;

  Perk perk;
  U64 perk_end_frame;

  Investment *investments;

  Graphics *graphics;

  inline void add_score(float rarity) { score += 100.0f / UPS * (1.0f + rarity); }

  Player(std::string name, CommandTable *table);

  virtual ~Player();
};

void player_score_add(Player *player, const Score score);

void player_score_sub(Player *player, const Score score);

#endif