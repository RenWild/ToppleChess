//
// Created by Vincent on 23/06/2019.
//

#include <random>

#include "../catch.hpp"
#include "../util.h"
#include "../../board.h"
#include "../../movegen.h"
#include "../../eval.h"
#include "../../hash.h"
#include "../../move.h"

TEST_CASE("Hash entry") {
    init_tables();
    zobrist::init_hashes();
    evaluator_t::eval_init();
    
    std::mt19937 gen(0);
    std::uniform_int_distribution<U64> dist;
    
    U64 hash = dist(gen);
    tt::entry_t entry;
    entry.data = dist(gen);
    entry.coded_hash = hash ^ entry.data;
    
    packed_move_t move = entry.info.move;
    int16_t internal_value = entry.info.internal_value;
    int16_t static_eval = entry.info.static_eval;
    int depth = entry.depth();
    tt::Bound bound = entry.bound();
    
    entry.refresh(0);
    
    REQUIRE((entry.coded_hash ^ entry.data) == hash);
    REQUIRE(entry.generation() == 0);
    REQUIRE(entry.info.move.from == move.from);
    REQUIRE(entry.info.move.to == move.to);
    REQUIRE(entry.info.move.type == move.type);
    REQUIRE(entry.info.internal_value == internal_value);
    REQUIRE(entry.info.static_eval == static_eval);
    REQUIRE(entry.depth() == depth);
    REQUIRE(entry.bound() == bound);
}