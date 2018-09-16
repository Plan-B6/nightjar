#include <iostream>

#include "board.h"
#include "evaluate.h"
#include "types.h"

int eval(const Board& b, const int color, const int ply) {
	int terminalStatus = terminal(b);
	if (terminalStatus == color) {
		return WIN_SCORE - ply;
	}
	else if (terminalStatus == (color ^ COLOR_FLIP)) {
		return -WIN_SCORE + ply;
	}
	else if (terminalStatus == 3) {
		return 0;
	}

	int score = 0;
	for (int row = 0; row < ROWS; ++row) {
		for (int col = 0; col < COLS; ++col) {
			int stone = b.state[row][col];
			if (stone == EMPTY) { continue; }
			score += (color == stone) ? evalStone(b, row, col, stone) : -evalStone(b, row, col, stone);
		}
	}

	score += (color == b.turn) ? rightToMoveBonus : -rightToMoveBonus;

	return score;
}

int evalStone(const Board& b, const Coord& c, const int color) {

	/*
	  Evaluates a stone's influence on the board by looking at windows of length five for all directions
	  and seeing how many stones are needed to create a five
	  Windows containing opponent's stones are ignored
	*/

	int score = 0;
	for (int i = 0; i < 8; ++i) {
		int empty = 0;
		int stones = 1;

		for (int j = 1; j <= 4; ++j) {
			int x = c.row + directions[i][0] * j;
			int y = c.col + directions[i][1] * j;
			if (outOfBounds(x, y)) { break; }
			int square = b.state[x][y];
			if (square == EMPTY) {
				empty++;
			}
			else if (square == color) {
				stones++;
			}
			else {
				break;
			}
		}

		if (stones + empty == 5) {
			score += evalScores[stones - 1];
		}
	}
	return score;
}
int evalStone(const Board& b, const int row, const int col, const int color) {

	int score = 0;
	for (int i = 0; i < 8; ++i) {
		int empty = 0;
		int stones = 1;

		for (int j = 1; j <= 4; ++j) {
			int x = row + directions[i][0] * j;
			int y = col + directions[i][1] * j;
			if (outOfBounds(x, y)) { break; }
			int square = b.state[x][y];
			if (square == EMPTY) {
				empty++;
			}
			else if (square == color) {
				stones++;
			}
			else {
				break;
			}
		}

		if (stones + empty == 5) {
			score += evalScores[stones - 1];
		}
	}
	return score;
}