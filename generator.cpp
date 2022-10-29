#include "Position.hpp"
#include "OpeningBook.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>

using namespace GameSolver::Connect4;

std::unordered_set<uint64_t> visited;

void explore(const Position &P, char* pos_str, const int depth) {
  uint64_t key = P.key3();
  if(!visited.insert(key).second) return;

  int nb_moves = P.nbMoves();
  if(nb_moves <= depth)
  std::cout << pos_str << std::endl;
  if(nb_moves >= depth) return;

  for(int i = 0; i < Position::WIDTH; i++)
    if(P.canPlay(i) && !P.isWinningMove(i)) {
      Position P2(P);
      P2.playCol(i);
      pos_str[nb_moves] = '1' + i;
      explore(P2, pos_str, depth);
      pos_str[nb_moves] = 0;
    }
}

void generate_opening_book() {
  static constexpr int BOOK_SIZE = 23;
  static constexpr int DEPTH = 14;
  static constexpr double LOG_3 = 1.58496250072;
  TranspositionTable<uint_t<int((DEPTH + Position::WIDTH -1) * LOG_3) + 1 - BOOK_SIZE>, Position::position_t, uint8_t, BOOK_SIZE> *table =
    new TranspositionTable<uint_t<int((DEPTH + Position::WIDTH -1) * LOG_3) + 1 - BOOK_SIZE>, Position::position_t, uint8_t, BOOK_SIZE>();

  long long count = 1;
  for(std::string line; getline(std::cin, line); count++) {
    if(line.length() == 0) break;
    std::istringstream iss(line);
    std::string pos;
    getline(iss, pos, ' ');
    int score;
    iss >> score;

    Position P;
    if(iss.fail() || !iss.eof()
        || P.play(pos) != pos.length()
        || score < Position::MIN_SCORE || score > Position::MAX_SCORE) {
      std::cerr << "Invalid line (line ignored): " << line << std::endl;
      continue;
    }
    table->put(P.key3(), score - Position::MIN_SCORE + 1);
    if(count % 1000000 == 0) std::cerr << count << std::endl;
  }

  OpeningBook book{Position::WIDTH, Position::HEIGHT, DEPTH, table};

  std::ostringstream book_file;
  book_file << Position::WIDTH << "x" << Position::HEIGHT << ".book";
  book.save(book_file.str());
}

int main(int argc, char** argv) {
  if(argc > 1) {
    int depth = atoi(argv[1]);
    char pos_str[depth + 1] = {0};
    explore(Position(), pos_str, depth);
  } else generate_opening_book();
}
