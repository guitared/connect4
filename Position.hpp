#ifndef POSITION_HPP
#define POSITION_HPP

#include <string>
#include <cstdint>
#include <cassert>

namespace GameSolver { namespace Connect4 {

class Position {
 public:
  static constexpr int WIDTH = 7;
  static constexpr int HEIGHT = 6;
  using position_t = typename std::conditional < WIDTH * (HEIGHT + 1) <= 64, uint64_t, __int128>::type;
  static constexpr int MIN_SCORE = -(WIDTH*HEIGHT) / 2 + 3;
  static constexpr int MAX_SCORE = (WIDTH * HEIGHT + 1) / 2 - 3;
  static_assert(WIDTH < 10, "Board's width must be less than 10");
  static_assert(WIDTH * (HEIGHT + 1) <= sizeof(position_t)*8, "Board does not fit into position_t bitmask");
  
  void play(position_t move) {current_position ^= mask; mask |= move; moves++;}
  
  unsigned int play(const std::string &seq) {
    for(unsigned int i = 0; i < seq.size(); i++) {
      int col = seq[i] - '1';
      if(col < 0 || col >= Position::WIDTH || !canPlay(col) || isWinningMove(col)) return i; // invalid move
      playCol(col);
    }
    return seq.size();
  }

  bool canWinNext() const {return winning_position() & possible();}
  int nbMoves() const {return moves;}
  position_t key() const {return current_position + mask;}

  uint64_t key3() const {
    uint64_t key_forward = 0;
    for(int i = 0; i < Position::WIDTH; i++) partialKey3(key_forward, i);

    uint64_t key_reverse = 0;
    for(int i = Position::WIDTH; i--;) partialKey3(key_reverse, i);

    return key_forward < key_reverse ? key_forward / 3 : key_reverse / 3;
  }

  position_t possibleNonLosingMoves() const {
    assert(!canWinNext());
    position_t possible_mask = possible();
    position_t opponent_win = opponent_winning_position();
    position_t forced_moves = possible_mask & opponent_win;
    if(forced_moves) {
      if(forced_moves & (forced_moves - 1)) return 0;
      else possible_mask = forced_moves;
    }
    return possible_mask & ~(opponent_win >> 1);
  }

  int moveScore(position_t move) const { return popcount(compute_winning_position(current_position | move, mask));}
  Position() : current_position{0}, mask{0}, moves{0} {}
  bool canPlay(int col) const {return (mask & top_mask_col(col)) == 0;}
  void playCol(int col) {play((mask + bottom_mask_col(col)) & column_mask(col));}
  bool isWinningMove(int col) const {return winning_position() & possible() & column_mask(col);}

 private:
  position_t current_position;
  position_t mask;
  unsigned int moves;

  void partialKey3(uint64_t &key, int col) const {
    for(position_t pos = UINT64_C(1) << (col * (Position::HEIGHT + 1)); pos & mask; pos <<= 1) {
      key *= 3;
      if(pos & current_position) key += 1;
      else key += 2;
    }
    key *= 3;
  }

  position_t winning_position() const {return compute_winning_position(current_position, mask);}
  position_t opponent_winning_position() const {return compute_winning_position(current_position ^ mask, mask);}
  position_t possible() const {return (mask + bottom_mask) & board_mask;}

  static unsigned int popcount(position_t m) {unsigned int c = 0; for(c = 0; m; c++) m &= m - 1; return c;}
  static position_t compute_winning_position(position_t position, position_t mask) {
    position_t r = (position << 1) & (position << 2) & (position << 3);
    position_t p = (position << (HEIGHT + 1)) & (position << 2 * (HEIGHT + 1));
    r |= p & (position << 3 * (HEIGHT + 1));
    r |= p & (position >> (HEIGHT + 1));
    p = (position >> (HEIGHT + 1)) & (position >> 2 * (HEIGHT + 1));
    r |= p & (position << (HEIGHT + 1));
    r |= p & (position >> 3 * (HEIGHT + 1));
    p = (position << HEIGHT) & (position << 2 * HEIGHT);
    r |= p & (position << 3 * HEIGHT);
    r |= p & (position >> HEIGHT);
    p = (position >> HEIGHT) & (position >> 2 * HEIGHT);
    r |= p & (position << HEIGHT);
    r |= p & (position >> 3 * HEIGHT);
    p = (position << (HEIGHT + 2)) & (position << 2 * (HEIGHT + 2));
    r |= p & (position << 3 * (HEIGHT + 2));
    r |= p & (position >> (HEIGHT + 2));
    p = (position >> (HEIGHT + 2)) & (position >> 2 * (HEIGHT + 2));
    r |= p & (position << (HEIGHT + 2));
    r |= p & (position >> 3 * (HEIGHT + 2));
    return r & (board_mask ^ mask);
  }

  template<int width, int height> struct bottom {static constexpr position_t mask = bottom<width-1, height>::mask | position_t(1) << (width - 1) * (height + 1);};
  template <int height> struct bottom<0, height> {static constexpr position_t mask = 0;};

  static constexpr position_t bottom_mask = bottom<WIDTH, HEIGHT>::mask;
  static constexpr position_t board_mask = bottom_mask * ((1LL << HEIGHT) - 1);
  static constexpr position_t top_mask_col(int col) {
    return UINT64_C(1) << ((HEIGHT - 1) + col * (HEIGHT + 1));
  }
  static constexpr position_t bottom_mask_col(int col) {
    return UINT64_C(1) << col * (HEIGHT + 1);
  }

 public:
  static constexpr position_t column_mask(int col) {return ((UINT64_C(1) << HEIGHT) - 1) << col * (HEIGHT + 1);}
};

}}
#endif
