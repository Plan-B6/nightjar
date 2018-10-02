#include "board.h"
#include "evaluate.h"
#include "search.h"
#include "threats.h"
#include "types.h"

#include <iostream>
#include <string>

int main() {
	Board b;
	std::string input;

	printBoard(b);
	std::cout << "Select AI color (b/w/ai/none)" << std::endl;

	std::getline(std::cin, input);
	int aiTurn = EMPTY;
	if (input == "b") {
		aiTurn = BLACK;
	}
	else if (input == "w") {
		aiTurn = WHITE;
	}
	else if (input == "ai") {
		aiTurn = 3;
	}

	while (1) {
		if (b.turn != aiTurn && aiTurn != 3 || aiTurn == EMPTY) {
			std::getline(std::cin, input);
			makeMove(b, notationToSquare(input));
			printBoard(b);
		}
		else {
			SearchInfo si;
			iterativeDeepening(b, si, 5);
			makeMove(b, si.bestMove);
			delInvalidEntries(b);
			printBoard(b);
		}
	}
	return 0;
}