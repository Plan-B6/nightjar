#include "board.h"
#include "search.h"
#include "types.h"

#include <iostream>
#include <string>
#include <assert.h>

U64 rand64() {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<long long int> dist(std::llround(std::pow(2, 61)), std::llround(std::pow(2, 62)));
	return dist(gen);
}

void makeMove(Board& b, const Coord& c) {
	assert(b.state[c.row][c.col] == EMPTY);
	assert(c != NO_MOVE);
	b.state[c.row][c.col] = b.turn;
	b.zobristHash ^= b.zobristKeys[c.row][c.col][b.turn  - 1];
	b.turn ^= COLOR_FLIP;
	b.last = c;
	b.empty--;
}

void undoMove(Board& b, const Coord& c, const Coord& last) {
	assert(b.state[c.row][c.col] != EMPTY);
	assert(c != NO_MOVE);
	b.state[c.row][c.col] = EMPTY;
	b.turn ^= COLOR_FLIP;
	b.zobristHash ^= b.zobristKeys[c.row][c.col][b.turn - 1];
	b.last = last;
	b.empty++;
}

bool nearStone(const Board& b, const Coord& c, int dist) {
	for (int i = 0; i < 8; ++i) {
		for (int j = 1; j <= dist; ++j) {
			int row = c.row + directions[i][0] * j;
			int col = c.col + directions[i][1] * j;
			if (outOfBounds(row, col)) { break; }
			if (b.state[row][col] != EMPTY) {
				return true;
			}
		}
	}
	return false;
}

bool nearStone(const Board& b, const int row, const int col, int dist) {
	for (int i = 0; i < 8; ++i) {
		for (int j = 1; j <= dist; ++j) {
			int x = row + directions[i][0] * j;
			int y = col + directions[i][1] * j;
			if (outOfBounds(x, y)) { break; }
			if (b.state[x][y] != EMPTY) {
				return true;
			}
		}
	}
	return false;
}

int terminal(const Board& b) {
	// There cannot be a win when there are 8 stones or less
	if (b.empty >= ROWS * COLS - 8) { return 0; }

	int color = b.turn ^ COLOR_FLIP;

	for (int i = 0; i < 8; ++i) {
		int count = 0;
		for (int j = 1; j <= 4; ++j) {
			int row = b.last.row + directions[i][0] * j;
			int col = b.last.col + directions[i][1] * j;
			if (outOfBounds(row, col) || b.state[row][col] != color) { break; }  // Relies on short-circuit evaluation
			count++;
			if (count == 4) { return color; }
		}
	}
	return (b.empty == 0) ? 3 : 0;
}

std::string squareToNotation(const Coord& c) {
	std::string s = "   ";
	s[0] = c.col + 65;
	if (c.row <= ROWS - 10) {
		s[1] = '1';
		s[2] = 53 - c.row;
	}
	else {
		s[1] = 57 - (c.row - 6);
		s.resize(2);
	}
	return s;
}

Coord notationToSquare(const std::string& s) {
	int col = s[0] - 65;
	int row = (s.length() == 2) ? ROWS - (s[1] - 48) : ROWS - 10 - (s[2] - 48);
	return Coord(row, col);
}

void printBoard(const Board& b) {
	std::cout << std::endl << std::endl;
	for (int row = 0; row < ROWS; ++row) {
		if (row == 0) {
			std::cout << "     A B C D E F G H I J K L M N O" << std::endl;
			std::cout << "    -------------------------------" << std::endl;
		}
		for (int col = 0; col < COLS; ++col) {
			if (col == 0) {
				if ((ROWS - row) < 10) { std::cout << " "; }
				std::cout << ROWS - row << " " << "| ";
			}
			switch (b.state[row][col]) {
			case EMPTY:
				std::cout << "* ";
				break;
			case BLACK:
				std::cout << "X ";
				break;
			case WHITE:
				std::cout << "O ";
				break;
			}
			if (col == COLS - 1) {
				std::cout << "| " << ROWS - row;
			}
		}
		std::cout << std::endl;
		if (row == ROWS - 1) {
			std::cout << "    -------------------------------" << std::endl;
			std::cout << "     A B C D E F G H I J K L M N O" << std::endl;
		}
	}
	std::cout << std::endl << "Hash: " << b.zobristHash << std::endl << std::endl;
}