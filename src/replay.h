#ifndef __REPLAY_H_
#define __REPLAY_H_

#include <time.h>

struct replay {
    struct keyflags *inputs;
    unsigned int len;
    unsigned int mlen;

    int mode;
    unsigned int mode_flags;
    long seed;
    int grade;
    long time;
    int starting_level;
    int ending_level;

    time_t date;
};

struct replay *compare_replays(struct replay *r1, struct replay *r2);

#endif // __REPLAY_H_
