#ifndef THREATS_INCLUDED
#define THREATS_INCLUDED

#include "board.h"
#include "evaluate.h"
#include "types.h"

#include <vector>

struct Threat {
	Threat(std::vector<int> pat, std::vector<int> b, std::vector<int> w) { pattern = pat, blocks = b, wins = w; }
	std::vector<int> pattern, blocks, wins;
};

static const std::vector<Threat> threeThreats =
{
	{{EMPTY, BLACK, BLACK, BLACK, EMPTY, EMPTY}, {0, 4}, {4}},
	{{EMPTY, EMPTY, BLACK, BLACK, BLACK, EMPTY}, {1, 5}, {1}},
	{{EMPTY, BLACK, EMPTY, BLACK, BLACK, EMPTY}, {0, 2, 5}, {2}},
	{{EMPTY, BLACK, BLACK, EMPTY, BLACK, EMPTY}, {0, 3, 5}, {3}},
};
static const std::vector<Threat> fourThreats =
{
	{{EMPTY, BLACK, BLACK, BLACK, BLACK}, {0}, {0}},
	{{BLACK, EMPTY, BLACK, BLACK, BLACK}, {1}, {1}},
	{{BLACK, BLACK, EMPTY, BLACK, BLACK}, {2}, {2}},
	{{BLACK, BLACK, BLACK, EMPTY, BLACK}, {3}, {3}},
	{{BLACK, BLACK, BLACK, BLACK, EMPTY}, {4}, {4}},
};

bool matchPattern(const Board& b, const Coord& c, const Threat& threat, const int color, const int direction[]);

void getThreatResponses(const Board& b, const Coord& c, const std::vector<Threat>& threats, std::vector<ScoredMove>& threatMoves, const int color, const int ply);
#endif
