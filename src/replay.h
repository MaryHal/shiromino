#ifndef __REPLAY_H_
#define __REPLAY_H_

#define NO_REPLAY -1

#define MAX_KEYFLAGS 73200 

#include <time.h>
#include <stdint.h>

#include "core.h" // TODO: Extract keyflags from core.h

struct replay {
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

    struct keyflags inputs[MAX_KEYFLAGS];
};

void get_replay_descriptor(struct replay *r, char *buffer, size_t bufferLength);

void read_replay_from_memory(struct replay *out_replay, const uint8_t *buffer, size_t bufferLength);

uint8_t* generate_raw_replay(struct replay *r, size_t *out_replayLength);
void dispose_raw_replay(void* data);

#endif // __REPLAY_H_
