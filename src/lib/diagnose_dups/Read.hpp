#pragma once

#include "Tile.hpp"

#include <sam.h> //from htslib

#include <cassert>
#include <cmath>
#include <limits>
#include <stdint.h>
#include <string>

struct Read {
    int32_t insert_size;
    Tile tile;
    int x;
    int y;
    bool is_read1;
    mutable bool ignore; // XXX This is dumb. Should consider a better way to do this.
};

void parse_read(bam1_t const* record, Read& read);

inline
bool is_on_same_tile(Read const& x, Read const& y) {
    return same_tile(x.tile, y.tile);
}

inline
bool is_on_adjacent_tile(Read const& x, Read const& y) {
    return adjacent_tile(x.tile, y.tile);
}

inline
bool is_on_same_strand(Read const& lhs, Read const& rhs) {
    return lhs.is_read1 == rhs.is_read1;
}

inline
uint64_t euclidean_distance(Read const& a, Read const& b) {
    double rv = hypot(a.x - b.x, a.y - b.y) + 0.5;
    assert(!isnan(rv));
    return rv;
}
