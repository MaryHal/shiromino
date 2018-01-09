#include "replay.h"

#include "core.h" // TODO: Extract keyflags from core.h

#include <string.h>

struct replay *compare_replays(struct replay *r1, struct replay *r2)
{
    int m1 = r1->mode;
    int m2 = r2->mode;
    int el1 = r1->ending_level;
    int sl1 = r1->starting_level;
    int el2 = r2->ending_level;
    int sl2 = r2->starting_level;
    int t1 = r1->time;
    int t2 = r2->time;

    if(m1 < m2) {
        return r1;
    } else if(m1 > m2) {
        return r2;
    } else if(m1 == m2) {
        if(sl1 < sl2)
            return r1;
        else if(sl1 > sl2)
            return r2;
        else {
            if(el1 - sl1 < el2 - sl2) {
                return r2;
            } else if(el1 - sl1 > el2 - sl2) {
                return r1;
            } else if(el1 - sl1 == el2 - sl2) {
                if(t1 < t2) {
                    return r1;
                } else if(t1 > t2) {
                    return r2;
                } else if(t1 == t2) {
                    return r1;
                }
            }
        }
    }

    return r1;
}

void read_replay_from_memory(struct replay *out_replay, uint8_t *buffer, size_t bufferLength)
{
    uint8_t *scanner = buffer;

    out_replay->mode = ((int*)scanner)[0];
    scanner += sizeof(int);
    
    out_replay->mode_flags = ((int*)scanner)[0];
    scanner += sizeof(int);

    out_replay->seed = ((long*)scanner)[0];
    scanner += sizeof(long);

    out_replay->grade = ((int*)scanner)[0];
    scanner += sizeof(int);

    out_replay->time = ((long*)scanner)[0];
    scanner += sizeof(long);

    out_replay->starting_level = ((int*)scanner)[0];
    scanner += sizeof(int);

    out_replay->ending_level = ((int*)scanner)[0];
    scanner += sizeof(int);

    out_replay->date = ((long*)scanner)[0];
    scanner += sizeof(long);

    out_replay->len = ((int*)scanner)[0];
    scanner += sizeof(int);

    out_replay->inputs = malloc(out_replay->len * sizeof(struct keyflags));
    
    memcpy(out_replay->inputs, scanner, out_replay->len * sizeof(struct keyflags));
}

uint8_t* generate_raw_replay(struct replay *r, size_t *out_replayLength)
{
    // TODO: Endianness?
    uint8_t *buffer = malloc(sizeof(struct keyflags) * r->len + sizeof(struct replay));
    size_t bufferOffset = 0;
    
    memcpy(buffer + bufferOffset, &r->mode, sizeof(int));
    bufferOffset += sizeof(int);

    memcpy(buffer + bufferOffset, &r->mode_flags, sizeof(int));
    bufferOffset += sizeof(int);

    memcpy(buffer + bufferOffset, &r->seed, sizeof(long));
    bufferOffset += sizeof(long);

    memcpy(buffer + bufferOffset, &r->grade, sizeof(int));
    bufferOffset += sizeof(int);

    memcpy(buffer + bufferOffset, &r->time, sizeof(long));
    bufferOffset += sizeof(long);

    memcpy(buffer + bufferOffset, &r->starting_level, sizeof(int));
    bufferOffset += sizeof(int);

    memcpy(buffer + bufferOffset, &r->ending_level, sizeof(int));
    bufferOffset += sizeof(int);

    memcpy(buffer + bufferOffset, &r->date, sizeof(long));
    bufferOffset += sizeof(long);

    memcpy(buffer + bufferOffset, &r->len, sizeof(int));
    bufferOffset += sizeof(int);
    
    /* memcpy(buffer + bufferOffset, r->inputs, sizeof(struct keyflags) * r->len); */
    /* bufferOffset += sizeof(struct keyflags) * r->len; */

    for (size_t i = 0; i < r->len; i++)
    {
        memcpy(buffer + bufferOffset, &r->inputs[i], sizeof(struct keyflags));
        bufferOffset += sizeof(struct keyflags);
    }

    *out_replayLength = bufferOffset;
    
    return buffer;
}

void dispose_raw_replay(void* data)
{
    free(data);
}
