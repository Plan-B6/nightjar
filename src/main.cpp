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

	/*makeMove(b, notationToSquare("H8"));
	makeMove(b, notationToSquare("H7"));
	makeMove(b, notationToSquare("F6"));
	makeMove(b, notationToSquare("G7"));
	makeMove(b, notationToSquare("E7"));
	makeMove(b, notationToSquare("D8"));
	makeMove(b, notationToSquare("F8"));
	makeMove(b, notationToSquare("G9"));
	makeMove(b, notationToSquare("E6"));
	makeMove(b, notationToSquare("G8"));
	makeMove(b, notationToSquare("G6"));
	makeMove(b, notationToSquare("D6"));
	makeMove(b, notationToSquare("E8"));
	makeMove(b, notationToSquare("E9"));
	makeMove(b, notationToSquare("F7"));*/

	/*(b, notationToSquare("H8"));
	makeMove(b, notationToSquare("A8"));
	makeMove(b, notationToSquare("H9"));
	makeMove(b, notationToSquare("D3"));
	makeMove(b, notationToSquare("H11"));*/
	// std::cout << matchPattern(b, notationToSquare("H12"), threeThreats[1], BLACK, directions[2]) << std::endl;
	// std::cout << matchPattern(b, notationToSquare("H7"), threeThreats[2], BLACK, directions[3]) << std::endl;

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
			iterativeDeepening(b, si);
			makeMove(b, si.bestMove);
			delInvalidEntries(b);
			printBoard(b);
		}
	}
	return 0;
}