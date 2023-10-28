#ifndef CACHELINE_H
#define CACHELINE_H


#include <cstddef>

class CacheSet;
class CacheLine {
    size_t state; // The states currently will corresponds to 0,1,2,3 
    // M -- 0, E -- 1, S -- 2, I -- 3 or M -- 0 E -- 1 Sc -- 2 Sm -- 3
    size_t tag;
    friend class CacheSet;

    CacheLine(size_t tag, size_t state);
};

#endif // CACHELINE_H