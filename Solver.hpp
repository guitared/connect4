#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <vector>
#include <string>
#include "Position.hpp"
#include "TranspositionTable.hpp"
#include "OpeningBook.hpp"

namespace GameSolver { namespace Connect4 {

class Solver {
 private:
  static constexpr int TABLE_SIZE = 24;
  TranspositionTable < uint_t < Position::WIDTH*(Position::HEIGHT + 1) - TABLE_SIZE >, Position::position_t, uint8_t, TABLE_SIZE > transTable;
  OpeningBook book{Position::WIDTH, Position::HEIGHT};
  unsigned long long nodeCount;
  int columnOrder[Position::WIDTH];
  int negamax(const Position &P, int alpha, int beta);

 public:
  static const int INVALID_MOVE = -1000;
  int solve(const Position &P, bool weak = false);
  std::vector<int> analyze(const Position &P, bool weak = false);

  unsigned long long getNodeCount() const {return nodeCount;}

  void reset() {nodeCount = 0; transTable.reset();}
  void loadBook(std::string book_file) {book.load(book_file);}

  Solver();
};

}}
#endif
