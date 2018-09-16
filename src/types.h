#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

#include <random>

enum { EMPTY, BLACK, WHITE };

enum { COLOR_FLIP = BLACK ^ WHITE };

enum { ROWS = 15, COLS = 15 };

enum { WIN_SCORE = 100000 };

enum { MAX_PLY = 50 };

enum { TT_ALPHA, TT_BETA, TT_EXACT };

enum { ENTRY_NOT_FOUND = -WIN_SCORE * 2 };

typedef struct Coord Coord;
typedef struct Board Board;
typedef struct SearchInfo SearchInfo;
typedef struct ScoredMove ScoredMove;
typedef struct Threat Threat;

typedef unsigned long long int U64;

#endif