#include <vector>
#include <iostream>
#include <array>
#include <map>
#include <algorithm>
#include <random>
#include <chrono>

#define DEPTH 7

typedef unsigned long long U64;
typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char U8;
typedef signed long long S64;
typedef signed int S32;
typedef signed short S16;
typedef signed char S8;

using namespace std;

#include "MagicMoves.h"
#include "data.h"

#include "piece.h"
#include "_move.hpp"
#include "_movelist.hpp"
#include "move.h"
#include "movelist.hpp"
#include "board.h"

#include "printer.h"
#include "interface.h"
#include "transposition.h"
#include "gen-castles.h"
#include "gen-captures.h"
#include "gen-moves.h"
#include "moving.h"
#include "evaluation.h"
#include "search.h"