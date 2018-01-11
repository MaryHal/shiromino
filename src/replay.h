#ifndef __REPLAY_H_
#define __REPLAY_H_

#define NO_REPLAY -1

#include <time.h>
#include <stdint.h>

struct keyflags;

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
    
    int index;
};

void read_replay_from_memory(struct replay *out_replay, const uint8_t *buffer, size_t bufferLength);

uint8_t* generate_raw_replay(struct replay *r, size_t *out_replayLength);
void dispose_raw_replay(void* data);

#endif // __REPLAY_H_
