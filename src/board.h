#ifndef BOARD_INCLUDED
#define BOARD_INCLUDED

#include "types.h"

#include <string>
#include <unordered_map>

U64 rand64();


struct Coord {
	Coord() {};
	Coord(int a, int b) { row = a, col = b; }
	int row, col;

	void operator=(const Coord& c) {
		row = c.row;
		col = c.col;
	}

	bool operator==(const Coord& c) const {
		return (row == c.row && col == c.col);
	}

	bool operator!=(const Coord& c) const {
		return (row != c.row || col != c.col);
	}
};

static const Coord NO_MOVE = Coord(-1, -1);

struct HashEntry {
	HashEntry() {}
	HashEntry(int a, int b, int c, int d) { depth = a, score = b, flag = c, empty = d; }
	int depth, score, flag, empty;
	Coord move = NO_MOVE;
};


struct Board {
	Board() { initZobristKeys(); }

	int state[ROWS][COLS] = {EMPTY};
	int turn = BLACK;
	int empty = ROWS * COLS;

	U64 zobristKeys[15][15][2];
	U64 zobristHash = 0;
	Coord last;

	std::unordered_map<U64, HashEntry> tt;
	
	void initZobristKeys() {
		for (int i = 0; i < 15; ++i) {
			for (int j = 0; j < 15; ++j) {
				zobristKeys[i][j][0] = rand64();
				zobristKeys[i][j][1] = rand64();
			}
		}
	}
};

static constexpr int directions[8][2] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1} };

static inline bool outOfBounds(const int row, const int col) {
	return (row < 0 || row >= ROWS || col < 0 || col >= COLS);
}
static inline bool outOfBounds(const Coord& c) {
	return (c.row < 0 || c.row >= ROWS || c.col < 0 || c.col >= COLS);
}

void makeMove(Board& b, const Coord& c);
void undoMove(Board& b, const Coord& c, const Coord& last);

bool nearStone(const Board& b, const Coord& c, int dist);
bool nearStone(const Board& b, const int row, const int col, int dist);

int terminal(const Board& b);

std::string squareToNotation(const Coord&);
Coord notationToSquare(const std::string& s);

void printBoard(const Board& b);

#endif