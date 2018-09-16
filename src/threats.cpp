#include "board.h"
#include "evaluate.h"
#include "search.h"
#include "threats.h"
#include "types.h"

#include <iostream>

bool matchPattern(const Board& b, const Coord& c, const Threat& threat, const int color, const int direction[]) {
	for (size_t i = 0, size = threat.pattern.size(); i != size; ++i) {
		int row = c.row + direction[0] * i;
		int col = c.col + direction[1] * i;
		if (outOfBounds(row, col)) { return false; }
		int patternColor = threat.pattern[i];
		if (color == WHITE && patternColor == BLACK) { 
			patternColor = WHITE; 
		}
		if (b.state[row][col] != patternColor) {
			return false;
		}
	}
	return true;
}

void getThreatResponses(const Board& b, const Coord& c, const std::vector<Threat>& threats, std::vector<ScoredMove>& threatMoves, const int color, const int ply) {
	for (int i = 0; i < 8; ++i) {
		for (const auto& threat : threats) {
			if (!matchPattern(b, c, threat, color, directions[i])) { 
				continue; 
			}
			for (const auto& pos : ((color == b.turn) ? threat.wins : threat.blocks)) {
				Coord threatMove(c.row + directions[i][0] * pos, c.col + directions[i][1] * pos);
				if (!outOfBounds(threatMove)) {
					threatMoves.push_back({ threatMove, scoreMove(b, c, ply, NO_MOVE) });
				}
			}
		}
	}
}