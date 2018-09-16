#ifndef SEARCH_INCLUDED
#define SEARCH_INCLUDED

#include "board.h"
#include "evaluate.h"
#include "threats.h"
#include "types.h"

#include <iostream>
#include <vector>
#include <time.h>

struct SearchInfo {
	int depth = 0;
	int seldepth = 0;
	int nodes = 0;
	int score = 0;
	int fh = 0;
	int fhf = 0;
	double startTime = clock();
	Coord bestMove = NO_MOVE;

	void clear() {
		depth = 0;
		seldepth = 0;
		nodes = 0;
		score = 0;
		fh = 0;
		fhf = 0;
		startTime = clock();
		bestMove = NO_MOVE;
	}

	void print() {
		std::cout << std::endl;
		std::cout << "Depth: " << depth << "/" << seldepth << std::endl;
		std::cout << "Nodes: " << nodes << std::endl;
		std::cout << "Score: " << score << std::endl;
		if (fh > 0) {
			std::cout << "Move ordering: " << float(fhf) / float(fh) * 100 << std::endl;
		}
		std::cout << "Time: " << double(clock() - startTime) / (CLOCKS_PER_SEC / 1000) << std::endl;
		std::cout << "Best move: " << squareToNotation(bestMove) << std::endl;
	}
};

static constexpr int ttBonus = 10000;

static constexpr int adjacentBonus = 2;
static constexpr int killerBonus[4] = { 5000, 4900, 4800, 4700 };

static constexpr int lateMoveReductionDepth = 3;
static constexpr int lateMoveCount = 10;
static constexpr int lateMoveR = 1;

static constexpr int aspirationWindow = 10;

static Coord killers[2][MAX_PLY + 1];

void initSearch();

int probeTT(const Board& b, const int depth, const int ply, const int alpha, const int beta, SearchInfo& si);
void storeTT(Board& b, const int depth, const int score, const int flag);
void delInvalidEntries(Board& b);

std::vector<ScoredMove> getThreatMoves(const Board& b, const int ply);
std::vector<ScoredMove> getMoves(const Board& b, const int ply);

void deleteDuplicateMoves(std::vector<ScoredMove>& moves);

int scoreMove(const Board& b, const Coord& c, const int ply, const Coord& ttMove);

int negamax(Board& b, int depth, int ply, int alpha, int beta, int color, SearchInfo& si);

void iterativeDeepening(Board& b, SearchInfo& si);

#endif
