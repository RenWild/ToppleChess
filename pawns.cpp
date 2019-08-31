//
// Created by Vincent on 01/06/2019.
//

#include "pawns.h"
#include "eval.h"

pawns::structure_t::structure_t(const processed_params_t &params, U64 kp_hash, U64 w_pawns, U64 b_pawns, U64 w_king,
                                U64 b_king)
        : hash(kp_hash), eval_mg(0), eval_eg(0) {
    // Find pawns
    U64 open[2] = {pawns::open_pawns<WHITE>(w_pawns, b_pawns), pawns::open_pawns<BLACK>(b_pawns, w_pawns)};
    U64 isolated[2] = {pawns::isolated(w_pawns), pawns::isolated(b_pawns)};
    U64 backwards[2] = {pawns::backward<WHITE>(w_pawns, b_pawns), pawns::backward<BLACK>(b_pawns, w_pawns)};
    U64 semi_backwards[2] = {pawns::semi_backward<WHITE>(w_pawns, b_pawns),
                             pawns::semi_backward<BLACK>(b_pawns, w_pawns)};
    U64 paired[2] = {pawns::paired(w_pawns), pawns::paired(b_pawns)};
    U64 detached[2] = {pawns::detached<WHITE>(w_pawns, b_pawns), pawns::detached<BLACK>(b_pawns, w_pawns)};
    U64 doubled[2] = {pawns::doubled<WHITE>(w_pawns), pawns::doubled<BLACK>(b_pawns)};
    U64 chain[2] = {pawns::attacks<WHITE>(w_pawns) & w_pawns, pawns::attacks<BLACK>(b_pawns) & b_pawns};
    U64 passed[2] = {pawns::passed<WHITE>(w_pawns, b_pawns), pawns::passed<BLACK>(b_pawns, w_pawns)};
    U64 candidate[2] = {pawns::candidates<WHITE>(w_pawns, b_pawns), pawns::candidates<BLACK>(b_pawns, w_pawns)};

    // Count isolated, backwards and doubled pawns
    int isolated_counts[2][2] = {
            {pop_count(isolated[WHITE] & ~open[WHITE]), pop_count(isolated[WHITE] & open[WHITE])},
            {pop_count(isolated[BLACK] & ~open[BLACK]), pop_count(isolated[BLACK] & open[BLACK])}
    }; // [TEAM][OPEN]
    int backwards_counts[2][2] = {
            {pop_count(backwards[WHITE] & ~open[WHITE]), pop_count(backwards[WHITE] & open[WHITE])},
            {pop_count(backwards[BLACK] & ~open[BLACK]), pop_count(backwards[BLACK] & open[BLACK])}
    }; // [TEAM][OPEN]
    int semi_backwards_counts[2][2] = {
            {pop_count(semi_backwards[WHITE] & ~open[WHITE]), pop_count(semi_backwards[WHITE] & open[WHITE])},
            {pop_count(semi_backwards[BLACK] & ~open[BLACK]), pop_count(semi_backwards[BLACK] & open[BLACK])}
    }; // [TEAM][OPEN]
    int paired_counts[2][2] = {
            {pop_count(paired[WHITE] & ~open[WHITE]), pop_count(paired[WHITE] & open[WHITE])},
            {pop_count(paired[BLACK] & ~open[BLACK]), pop_count(paired[BLACK] & open[BLACK])}
    }; // [TEAM][OPEN]
    int detached_counts[2][2] = {
            {pop_count(detached[WHITE] & ~open[WHITE]), pop_count(detached[WHITE] & open[WHITE])},
            {pop_count(detached[BLACK] & ~open[BLACK]), pop_count(detached[BLACK] & open[BLACK])}
    }; // [TEAM][OPEN]
    int doubled_counts[2][2] = {
            {pop_count(doubled[WHITE] & ~open[WHITE]), pop_count(doubled[WHITE] & open[WHITE])},
            {pop_count(doubled[BLACK] & ~open[BLACK]), pop_count(doubled[BLACK] & open[BLACK])}
    }; // [TEAM][OPEN]

    // Add to the scores
    eval_mg += (isolated_counts[WHITE][0] - isolated_counts[BLACK][0]) * params.isolated_mg[0];
    eval_mg += (isolated_counts[WHITE][1] - isolated_counts[BLACK][1]) * params.isolated_mg[1];
    eval_eg += (isolated_counts[WHITE][0] - isolated_counts[BLACK][0]) * params.isolated_eg[0];
    eval_eg += (isolated_counts[WHITE][1] - isolated_counts[BLACK][1]) * params.isolated_eg[1];

    eval_mg += (backwards_counts[WHITE][0] - backwards_counts[BLACK][0]) * params.backwards_mg[0];
    eval_mg += (backwards_counts[WHITE][1] - backwards_counts[BLACK][1]) * params.backwards_mg[1];
    eval_eg += (backwards_counts[WHITE][0] - backwards_counts[BLACK][0]) * params.backwards_eg[0];
    eval_eg += (backwards_counts[WHITE][1] - backwards_counts[BLACK][1]) * params.backwards_eg[1];

    eval_mg += (semi_backwards_counts[WHITE][0] - semi_backwards_counts[BLACK][0]) * params.semi_backwards_mg[0];
    eval_mg += (semi_backwards_counts[WHITE][1] - semi_backwards_counts[BLACK][1]) * params.semi_backwards_mg[1];
    eval_eg += (semi_backwards_counts[WHITE][0] - semi_backwards_counts[BLACK][0]) * params.semi_backwards_eg[0];
    eval_eg += (semi_backwards_counts[WHITE][1] - semi_backwards_counts[BLACK][1]) * params.semi_backwards_eg[1];

    eval_mg += (paired_counts[WHITE][0] - paired_counts[BLACK][0]) * params.paired_mg[0];
    eval_mg += (paired_counts[WHITE][1] - paired_counts[BLACK][1]) * params.paired_mg[1];
    eval_eg += (paired_counts[WHITE][0] - paired_counts[BLACK][0]) * params.paired_eg[0];
    eval_eg += (paired_counts[WHITE][1] - paired_counts[BLACK][1]) * params.paired_eg[1];

    eval_mg += (detached_counts[WHITE][0] - detached_counts[BLACK][0]) * params.detached_mg[0];
    eval_mg += (detached_counts[WHITE][1] - detached_counts[BLACK][1]) * params.detached_mg[1];
    eval_eg += (detached_counts[WHITE][0] - detached_counts[BLACK][0]) * params.detached_eg[0];
    eval_eg += (detached_counts[WHITE][1] - detached_counts[BLACK][1]) * params.detached_eg[1];

    eval_mg += (doubled_counts[WHITE][0] - doubled_counts[BLACK][0]) * params.doubled_mg[0];
    eval_mg += (doubled_counts[WHITE][1] - doubled_counts[BLACK][1]) * params.doubled_mg[1];
    eval_eg += (doubled_counts[WHITE][0] - doubled_counts[BLACK][0]) * params.doubled_eg[0];
    eval_eg += (doubled_counts[WHITE][1] - doubled_counts[BLACK][1]) * params.doubled_eg[1];

    U64 bb;

    // King PST
    int king_loc[2] = {bit_scan(w_king), bit_scan(b_king)};
    eval_mg += params.pst[WHITE][KING][king_loc[WHITE]][MG];
    eval_eg += params.pst[WHITE][KING][king_loc[WHITE]][EG];
    eval_mg -= params.pst[BLACK][KING][king_loc[BLACK]][MG];
    eval_eg -= params.pst[BLACK][KING][king_loc[BLACK]][EG];

    // Pawn PST
    bb = w_pawns;
    while (bb) {
        uint8_t sq = pop_bit(bb);
        eval_mg += params.pst[WHITE][PAWN][sq][MG];
        eval_eg += params.pst[WHITE][PAWN][sq][EG];

        eval_eg += distance(king_loc[WHITE], sq) * params.king_tropism_eg[0];
        eval_eg += distance(king_loc[BLACK], sq) * params.king_tropism_eg[1];
    }

    bb = b_pawns;
    while (bb) {
        uint8_t sq = pop_bit(bb);
        eval_mg -= params.pst[BLACK][PAWN][sq][MG];
        eval_eg -= params.pst[BLACK][PAWN][sq][EG];

        eval_eg -= distance(king_loc[BLACK], sq) * params.king_tropism_eg[0];
        eval_eg -= distance(king_loc[WHITE], sq) * params.king_tropism_eg[1];
    }

    // Chain
    bb = chain[WHITE];
    while (bb) {
        uint8_t sq = pop_bit(bb);
        uint8_t rank = rel_rank(WHITE, rank_index(sq));
        eval_mg += params.chain_mg[rank - 2];
        eval_eg += params.chain_eg[rank - 2];
    }

    bb = chain[BLACK];
    while (bb) {
        uint8_t sq = pop_bit(bb);
        uint8_t rank = rel_rank(BLACK, rank_index(sq));
        eval_mg -= params.chain_mg[rank - 2];
        eval_eg -= params.chain_eg[rank - 2];
    }

    // Passed
    bb = passed[WHITE];
    while (bb) {
        uint8_t sq = pop_bit(bb);
        uint8_t rank = rel_rank(WHITE, rank_index(sq));
        eval_mg += params.passed_mg[rank - 1];
        eval_eg += params.passed_eg[rank - 1];

        eval_eg += distance(king_loc[WHITE], sq) * params.passer_tropism_eg[0];
        eval_eg += distance(king_loc[BLACK], sq) * params.passer_tropism_eg[1];
    }

    bb = passed[BLACK];
    while (bb) {
        uint8_t sq = pop_bit(bb);
        uint8_t rank = rel_rank(BLACK, rank_index(sq));
        eval_mg -= params.passed_mg[rank - 1];
        eval_eg -= params.passed_eg[rank - 1];

        eval_eg -= distance(king_loc[BLACK], sq) * params.passer_tropism_eg[0];
        eval_eg -= distance(king_loc[WHITE], sq) * params.passer_tropism_eg[1];
    }

    // Candidates
    bb = candidate[WHITE];
    while (bb) {
        uint8_t sq = pop_bit(bb);
        uint8_t rank = rel_rank(WHITE, rank_index(sq));
        eval_mg += params.candidate_mg[rank - 1];
        eval_eg += params.candidate_eg[rank - 1];
    }

    bb = candidate[BLACK];
    while (bb) {
        uint8_t sq = pop_bit(bb);
        uint8_t rank = rel_rank(BLACK, rank_index(sq));
        eval_mg -= params.candidate_mg[rank - 1];
        eval_eg -= params.candidate_eg[rank - 1];
    }
}

void pawns::structure_t::eval_dynamic(const processed_params_t &params, const board_t &board, int &mg, int &eg) const {
    // These should only be done setwise - branches should be avoided whenever possible for the sake of performance
    U64 w_pawns = board.bb_pieces[WHITE][PAWN];
    U64 b_pawns = board.bb_pieces[BLACK][PAWN];

    U64 open_files = pawns::open_files(w_pawns, b_pawns);
    U64 half_open_files[2] = {
            pawns::half_or_open_files(w_pawns) ^ open_files,
            pawns::half_or_open_files(b_pawns) ^ open_files
    };

    // Blocked pawns
    U64 stop_squares[2] = {pawns::stop_squares<WHITE>(w_pawns), pawns::stop_squares<BLACK>(b_pawns)};
    int blocked_count[2][2] = {
            {pop_count(stop_squares[WHITE] & board.bb_side[WHITE]), pop_count(stop_squares[WHITE] & board.bb_side[BLACK])},
            {pop_count(stop_squares[BLACK] & board.bb_side[BLACK]), pop_count(stop_squares[BLACK] & board.bb_side[WHITE])}
    };

    // Rook on open files
    int open_file_count[2] = {
            pop_count(open_files & board.bb_pieces[WHITE][ROOK]),
            pop_count(open_files & board.bb_pieces[BLACK][ROOK])
    };
    int own_half_open_file_count[2] = {
            pop_count(half_open_files[WHITE] & board.bb_pieces[WHITE][ROOK]),
            pop_count(half_open_files[BLACK] & board.bb_pieces[BLACK][ROOK])
    };
    int other_half_open_file_count[2] = {
            pop_count(half_open_files[BLACK] & board.bb_pieces[WHITE][ROOK]),
            pop_count(half_open_files[WHITE] & board.bb_pieces[BLACK][ROOK])
    };

    // Outposts
    U64 attacks_bb[2] = {attacks<WHITE>(w_pawns), attacks<BLACK>(b_pawns)};
    U64 outpost_squares[2] = {outpost[WHITE] & attacks_bb[WHITE] & ~attacks_bb[BLACK],
                              outpost[BLACK] & attacks_bb[BLACK] & ~attacks_bb[WHITE]};
    U64 holes_bb[2] = {holes<WHITE>(w_pawns), holes<BLACK>(b_pawns)};

    int outpost_count[2][2] = {
            {
                    pop_count(outpost_squares[WHITE] & board.bb_pieces[WHITE][KNIGHT]),
                    pop_count(outpost_squares[WHITE] & board.bb_pieces[WHITE][BISHOP])
            },
            {
                    pop_count(outpost_squares[BLACK] & board.bb_pieces[BLACK][KNIGHT]),
                    pop_count(outpost_squares[BLACK] & board.bb_pieces[BLACK][BISHOP])
            },
    };
    int outpost_hole_count[2][2] = {
            {
                    pop_count(outpost_squares[WHITE] & holes_bb[BLACK] & board.bb_pieces[WHITE][KNIGHT]),
                    pop_count(outpost_squares[WHITE] & holes_bb[BLACK] & board.bb_pieces[WHITE][BISHOP])
            },
            {
                    pop_count(outpost_squares[BLACK] & holes_bb[WHITE] & board.bb_pieces[BLACK][KNIGHT]),
                    pop_count(outpost_squares[BLACK] & holes_bb[WHITE] & board.bb_pieces[BLACK][BISHOP])
            },
    };
    int outpost_half_count[2][2] = {
            {
                    pop_count(outpost_squares[WHITE] & half_open_files[WHITE] & board.bb_pieces[WHITE][KNIGHT]),
                    pop_count(outpost_squares[WHITE] & half_open_files[WHITE] & board.bb_pieces[WHITE][BISHOP])
            },
            {
                    pop_count(outpost_squares[BLACK] & half_open_files[BLACK] & board.bb_pieces[BLACK][KNIGHT]),
                    pop_count(outpost_squares[BLACK] & half_open_files[BLACK] & board.bb_pieces[BLACK][BISHOP])
            },
    };

    // Summing up
    mg += (blocked_count[WHITE][0] - blocked_count[BLACK][0]) * params.blocked_mg[0];
    mg += (blocked_count[WHITE][1] - blocked_count[BLACK][1]) * params.blocked_mg[1];
    eg += (blocked_count[WHITE][0] - blocked_count[BLACK][0]) * params.blocked_eg[0];
    eg += (blocked_count[WHITE][1] - blocked_count[BLACK][1]) * params.blocked_eg[1];

    mg += (open_file_count[WHITE] - open_file_count[BLACK]) * params.pos_r_open_file_mg;
    mg += (own_half_open_file_count[WHITE] - own_half_open_file_count[BLACK]) * params.pos_r_own_half_open_file_mg;
    mg += (other_half_open_file_count[WHITE] - other_half_open_file_count[BLACK]) *
          params.pos_r_other_half_open_file_mg;
    eg += (open_file_count[WHITE] - open_file_count[BLACK]) * params.pos_r_open_file_eg;
    eg += (own_half_open_file_count[WHITE] - own_half_open_file_count[BLACK]) * params.pos_r_own_half_open_file_eg;
    eg += (other_half_open_file_count[WHITE] - other_half_open_file_count[BLACK]) *
          params.pos_r_other_half_open_file_eg;

    mg += (outpost_count[WHITE][0] - outpost_count[BLACK][0]) * params.outpost_mg[0];
    mg += (outpost_count[WHITE][1] - outpost_count[BLACK][1]) * params.outpost_mg[1];
    eg += (outpost_count[WHITE][0] - outpost_count[BLACK][0]) * params.outpost_eg[0];
    eg += (outpost_count[WHITE][1] - outpost_count[BLACK][1]) * params.outpost_eg[1];
    mg += (outpost_hole_count[WHITE][0] - outpost_hole_count[BLACK][0]) * params.outpost_hole_mg[0];
    mg += (outpost_hole_count[WHITE][1] - outpost_hole_count[BLACK][1]) * params.outpost_hole_mg[1];
    eg += (outpost_hole_count[WHITE][0] - outpost_hole_count[BLACK][0]) * params.outpost_hole_eg[0];
    eg += (outpost_hole_count[WHITE][1] - outpost_hole_count[BLACK][1]) * params.outpost_hole_eg[1];
    mg += (outpost_half_count[WHITE][0] - outpost_half_count[BLACK][0]) * params.outpost_half_mg[0];
    mg += (outpost_half_count[WHITE][1] - outpost_half_count[BLACK][1]) * params.outpost_half_mg[1];
    eg += (outpost_half_count[WHITE][0] - outpost_half_count[BLACK][0]) * params.outpost_half_eg[0];
    eg += (outpost_half_count[WHITE][1] - outpost_half_count[BLACK][1]) * params.outpost_half_eg[1];
}