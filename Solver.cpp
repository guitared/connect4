#include <cassert>
#include "Solver.hpp"
#include "MoveSorter.hpp"

using namespace GameSolver::Connect4;

namespace GameSolver { namespace Connect4 {

int Solver::negamax(const Position &P, int alpha, int beta) {
  assert(alpha < beta);
  assert(!P.canWinNext());

  nodeCount++;

  Position::position_t possible = P.possibleNonLosingMoves();
  if(possible == 0) return -(Position::WIDTH * Position::HEIGHT - P.nbMoves()) / 2;

  if(P.nbMoves() >= Position::WIDTH * Position::HEIGHT - 2) return 0;

  int min = -(Position::WIDTH * Position::HEIGHT - 2 - P.nbMoves()) / 2;
  if(alpha < min) {alpha = min; if(alpha >= beta) return alpha;}

  int max = (Position::WIDTH * Position::HEIGHT - 1 - P.nbMoves()) / 2;
  if(beta > max) {beta = max; if(alpha >= beta) return beta;}

  const Position::position_t key = P.key();
  if(int val = transTable.get(key)) {
    if(val > Position::MAX_SCORE - Position::MIN_SCORE + 1) {
      min = val + 2 * Position::MIN_SCORE - Position::MAX_SCORE - 2;
      if(alpha < min) {alpha = min; if(alpha >= beta) return alpha;}
    } else {max = val + Position::MIN_SCORE - 1;
      if(beta > max) {beta = max; if(alpha >= beta) return beta;}}
  }

  if(int val = book.get(P)) return val + Position::MIN_SCORE - 1;

  MoveSorter moves;
  for(int i = Position::WIDTH; i--;)
    if(Position::position_t move = possible & Position::column_mask(columnOrder[i]))
      moves.add(move, P.moveScore(move));

  while(Position::position_t next = moves.getNext()) {
    Position P2(P);
    P2.play(next);
    int score = -negamax(P2, -beta, -alpha);

    if(score >= beta) {transTable.put(key, score + Position::MAX_SCORE - 2 * Position::MIN_SCORE + 2); return score;}
    if(score > alpha) alpha = score;
  }

  transTable.put(key, alpha - Position::MIN_SCORE + 1);
  return alpha;
}

int Solver::solve(const Position &P, bool weak) {
  if(P.canWinNext())
    return (Position::WIDTH * Position::HEIGHT + 1 - P.nbMoves()) / 2;
  int min = -(Position::WIDTH * Position::HEIGHT - P.nbMoves()) / 2;
  int max = (Position::WIDTH * Position::HEIGHT + 1 - P.nbMoves()) / 2;
  if(weak) {min = -1;max = 1;}

  while(min < max) {
    int med = min + (max - min) / 2;
    if(med <= 0 && min / 2 < med) med = min / 2;
    else if(med >= 0 && max / 2 > med) med = max / 2;
    int r = negamax(P, med, med + 1);
    if(r <= med) max = r;
    else min = r;
  }
  return min;
}

std::vector<int> Solver::analyze(const Position &P, bool weak) {
  std::vector<int> scores(Position::WIDTH, Solver::INVALID_MOVE);
  for (int col = 0; col < Position::WIDTH; col++)
    if (P.canPlay(col)) {
      if(P.isWinningMove(col)) scores[col] = (Position::WIDTH * Position::HEIGHT + 1 - P.nbMoves()) / 2;
      else {
        Position P2(P);
        P2.playCol(col);
        scores[col] = -solve(P2, weak);
      }
    }
  return scores;
}
  
Solver::Solver() : nodeCount{0} {
  for(int i = 0; i < Position::WIDTH; i++)
    columnOrder[i] = Position::WIDTH / 2 + (1 - 2 * (i % 2)) * (i + 1) / 2;
}

}}
