#include "board.h"
#include "evaluate.h"
#include "search.h"
#include "threats.h"
#include "types.h"

#include <iostream>
#include <vector>
#include <algorithm>

void initSearch() {
	// Initialize killers
	for (int i = 0; i <= MAX_PLY; ++i) {
		killers[0][i] = NO_MOVE;
		killers[1][i] = NO_MOVE;
	}
}

int probeTT(const Board& b, const int depth, const int ply, const int alpha, const int beta, SearchInfo& si) {
	auto it = b.tt.find(b.zobristHash);
	if (it != b.tt.end()) {
		if (it->second.depth >= depth) {
			if (it->second.flag == TT_EXACT) {
				if (ply == 0) { si.bestMove = it->second.move; }
				return it->second.score;
			}
			else if (it->second.flag == TT_ALPHA && it->second.score <= alpha) {
				return alpha;
			}
			else if (it->second.flag == TT_BETA && it->second.score >= beta) {
				return beta;
			}
		}
	}
	return ENTRY_NOT_FOUND;
}

void storeTT(Board& b, const int depth, const int score, const int flag) {
	auto it = b.tt.find(b.zobristHash);
	if (it != b.tt.end()) {
		if (it->second.depth <= depth) {
			it->second.depth = depth;
			it->second.score = score;
			it->second.flag = flag;
			it->second.empty = b.empty;
		}
	}
	else {
		b.tt.emplace(std::piecewise_construct, std::forward_as_tuple(b.zobristHash), std::forward_as_tuple(depth, score, flag, b.empty));
	}
}

void delInvalidEntries(Board& b) {
	// Removes entries with more empty squares than the current board
	for (auto it = b.tt.begin(); it != b.tt.end();) {
		if (it->second.empty > b.empty) {
			it = b.tt.erase(it);
		}
		else {
			it++;
		}
	}
}

std::vector<ScoredMove> getThreatMoves(const Board& b, const int ply) {
	// There cannot be a threat when there are 4 stones or less
	if (b.empty >= ROWS * COLS - 4) { return {}; }

	std::vector<std::vector<ScoredMove>> threatMoves = { {}, {}, {}, {} };

	// Keep track of which type of threats we have found so far
	bool threatStatus[4] = { false, false, false, false };

	for (int row = 0; row < ROWS; ++row) {
		for (int col = 0; col < COLS; ++col) {
			if (!nearStone(b, row, col, 1)) { continue; }
			int stone = b.state[row][col];
			Coord c(row, col);

			getThreatResponses(b, c, fourThreats, threatMoves[0], b.turn, ply);
			// We can win, play the winning move
			if (!threatMoves[0].empty()) { 
				return threatMoves[0];
			}

			if (!threatMoves[2].empty()) { continue; }
			getThreatResponses(b, c, fourThreats, threatMoves[2], b.turn ^ COLOR_FLIP, ply);
			if (!threatMoves[2].empty()) { continue; }

			if (!threatMoves[1].empty()) { continue; }
			getThreatResponses(b, c, threeThreats, threatMoves[1], b.turn, ply);
			if (!threatMoves[1].empty()) { continue; }

			getThreatResponses(b, c, threeThreats, threatMoves[3], b.turn ^ COLOR_FLIP, ply);
		}
	}

	// Opponent has a four, block it
	if (!threatMoves[2].empty()) {
		return threatMoves[2];
	}

	// We have a three that could lead to a win
	if (!threatMoves[1].empty()) {
		return threatMoves[1];
	}

	// Opponent has a three that needs to be blocked
	if (!threatMoves[3].empty()) {
		return threatMoves[3];
	}

	// Return an empty vector if no threats are found
	return {};
}

std::vector<ScoredMove> getMoves(const Board& b, const int ply) {
	std::vector<ScoredMove> moves;
	moves.reserve(40);
	
	// Place stone in center if board is empty
	if (b.empty == ROWS * COLS) {
		moves.push_back({ Coord(ROWS / 2, COLS / 2), 0 });
		return moves;
	}

	// Get move from TT if there is one
	Coord ttMove = NO_MOVE;
	auto it = b.tt.find(b.zobristHash);
	if (it != b.tt.end()) {
		ttMove = it->second.move;
	}

	for (int row = 0; row < ROWS; ++row) {
		for (int col = 0; col < COLS; ++col) {
			int stone = b.state[row][col];
			Coord c(row, col);
			if (stone != EMPTY || !nearStone(b, c, 2)) { continue; }
			moves.push_back({ c, scoreMove(b, c, ply, ttMove) });
		}
	}

	return moves;
}

void deleteDuplicateMoves(std::vector<ScoredMove>& moves) {
	int size = moves.size();
	for (int i = 0; i < size; ++i) {
		for (int j = i + 1; j < size; ++j) {
			if (moves[i] == moves[j]) {
				moves.erase(moves.begin() + j);
				j--;
				size--;
			}
		}
	}
}

int scoreMove(const Board& b, const Coord& c, const int ply, const Coord& ttMove) {

	// TT move
	if (c == ttMove) return ttBonus;

	// Killer moves
	if (c == killers[0][ply]) return killerBonus[0];
	if (c == killers[1][ply]) return killerBonus[1];
	if (ply - 2 >= 0) {
		if (c == killers[0][ply - 2]) return killerBonus[2];
		if (c == killers[1][ply - 2]) return killerBonus[3];
	}

	int score = 0;
	// Static evaluation of the move
	score += evalStone(b, c, b.turn);
	// Slight bonus if the move is adjacent to a stone
	score += nearStone(b, c, 1) * adjacentBonus;
	return score;
}

int negamax(Board& b, int depth, int ply, int alpha, int beta, int color, SearchInfo& si) {
	bool isRoot = (ply == 0);
	int score;
	Coord bestMove;
	int ttFlag = TT_ALPHA;
	
	// Update selective depth
	if (ply > si.seldepth) { si.seldepth = ply; }

	// Prune irrelevant branches if we are winning/losing
	if (!isRoot) {
		int wAlpha = (alpha > -WIN_SCORE + ply) ? alpha : -WIN_SCORE + ply;
		int wBeta = (beta < WIN_SCORE - ply - 1) ? beta : WIN_SCORE - ply - 1;
		if (wAlpha >= wBeta) return wAlpha;
	}

	// Probe transposition table
	// If this happens at the root, the best move is updated from the TT
	int ttScore = probeTT(b, depth, ply, alpha, beta, si);
	if (ttScore != ENTRY_NOT_FOUND) return ttScore;

	if (depth <= 0) {
		si.nodes++;
		int evalScore = eval(b, color, ply);
		storeTT(b, depth, evalScore, TT_EXACT);
		return evalScore;
	}
	else {
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
	}

	auto moves = getThreatMoves(b, ply);
	bool threatened = true;
	if (moves.empty()) {
		moves = getMoves(b, ply);
		threatened = false;
	}

	// Only threat moves can have duplicates
	if (threatened) deleteDuplicateMoves(moves);

	// Sort moves in descending score
	std::sort(moves.begin(), moves.end(), [](const auto& a, const auto& b) { return a.score > b.score; });

	int moveCount = moves.size();
	if (moveCount == 1 && isRoot) {
		si.bestMove = moves[0].move;
	}

	int extend = 0;
	// Threat extension
	if (threatened && ply - depth + 1 < MAX_PLY) extend++;

	for (int i = 0; i < moveCount; ++i) {

		Coord c = moves[i].move;

		// Prevents not having a best move
		if (isRoot && i == 0) { si.bestMove = moves[i].move; }

		if (isRoot && depth == 5) { 
			std::cout << i + 1 << "/" << moveCount + 1 << " " << squareToNotation(c) << " " << moves[i].score << std::endl; 
		}

		si.nodes++;

		// Save the previous move
		Coord last = b.last;
		makeMove(b, c);

		score = alpha + 1;

		// Late move reduction
		if (!threatened && depth >= lateMoveReductionDepth && i >= lateMoveCount) {
			int R = lateMoveR + i / lateMoveCount * 3;
			if (R > depth - 2) { R = depth - 2; }
			score = -negamax(b, depth - 1 - R, ply + 1, -beta, -alpha, b.turn, si);
		}

		if (score > alpha) {
			score = -negamax(b, depth - 1 + extend, ply + 1, -beta, -alpha, b.turn, si);
		}

		undoMove(b, c, last);

		if (score >= beta) {

			storeTT(b, depth, beta, TT_BETA);

			// Update killers
			if (!threatened && c != killers[0][ply]) {
				killers[1][ply] = killers[0][ply];
				killers[0][ply] = c;
			}

			// Record fail high percentage
			if (i == 0) { si.fhf++; }
			si.fh++;

			return beta;
		}
		if (score > alpha) {
			alpha = score;
			bestMove = c;
			b.tt[b.zobristHash].move = c;
			ttFlag = TT_EXACT;

			if (isRoot) {
				si.score = score;
				si.bestMove = bestMove;
			}
		}
	}

	storeTT(b, depth, alpha, ttFlag);
	return alpha;
}

void iterativeDeepening(Board& b, SearchInfo& si) {
	initSearch();

	int alpha = -WIN_SCORE;
	int beta = WIN_SCORE;

	for (int i = 1; i <= 5; ++i) {
		si.clear();
		si.depth = i;
		int score = negamax(b, i, 0, alpha, beta, b.turn, si);

		if (score <= alpha || score >= beta) {
			alpha = -WIN_SCORE;
			beta = WIN_SCORE;
			i--;
			continue;
		}

		bool nearTerminal = (score <= -WIN_SCORE + MAX_PLY || score >= WIN_SCORE - MAX_PLY);

		if (i > 2 && !nearTerminal) {
			alpha = score - aspirationWindow;
			beta = score + aspirationWindow;
		}
		else {
			alpha = -WIN_SCORE;
			beta = WIN_SCORE;
		}
		
		if (i == 5) si.print();
	}
}