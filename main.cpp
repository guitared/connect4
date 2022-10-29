
#include "Solver.hpp"
#include <iostream>

using namespace GameSolver::Connect4;

int main(int argc, char** argv) {
  Solver solver;
  bool weak = false;
  bool analyze = false;

  std::string opening_book = "7x6.book";
  for(int i = 1; i < argc; i++) {
    if(argv[i][0] == '-') {
      if(argv[i][1] == 'w') weak = true;
      else if(argv[i][1] == 'b') {if(++i < argc) opening_book = std::string(argv[i]);}
      else if(argv[i][1] == 'a') {analyze = true;}
    }
  }
  solver.loadBook(opening_book);

  std::string line;

  for(int l = 1; std::getline(std::cin, line); l++) {
    Position P;
    if(P.play(line) != line.size()) {
      std::cerr << "Line " << l << ": Invalid move " << (P.nbMoves() + 1) << " \"" << line << "\"" << std::endl;
    } else {
      std::cout << line;
      if(analyze) {
        std::vector<int> scores = solver.analyze(P, weak);
        for(int i = 0; i < Position::WIDTH; i++) std::cout << " " << scores[i];
      }
      else {
        int score = solver.solve(P, weak);
        std::cout << " " << score;
      }
      std::cout << std::endl;
    }
  }
}
