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

	while (1) {

		// std::getline(std::cin, input);
		// makeMove(b, notationToSquare(input));
		// if (terminal(b) != 0) { break; }

		SearchInfo si;
		iterativeDeepening(b, si);
		makeMove(b, si.bestMove);
		delInvalidEntries(b);
		printBoard(b);
		if (terminal(b) != 0) { break; }

	}
	while (1) {}
	return 0;
}