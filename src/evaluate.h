#ifndef EVALUATE_INCLUDED
#define EVALUATE_INCLUDED

#include "board.h"
#include "types.h"

struct ScoredMove {
	ScoredMove(Coord a, int b) { move = a, score = b; }
	Coord move = NO_MOVE;
	int score;

	bool operator==(const ScoredMove& s) const {
		return (move == s.move);
	}

	bool operator!=(const ScoredMove& s) const {
		return (move != s.move);
	}
};

static constexpr int evalScores[6] = { 3, 7, 11, 15, 19, WIN_SCORE };
static constexpr int rightToMoveBonus = 20;

int eval(const Board& b, const int color, const int ply);
int evalStone(const Board& b, const Coord& c, const int color);
int evalStone(const Board& b, const int row, const int col, const int color);

#endif