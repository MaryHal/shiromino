#include "replay.h"

#include "game_qs.h"

#include <string.h>

void get_replay_descriptor(struct replay *r, char *buffer, size_t bufferLength)
{
    const uint8_t BUF_SIZE = 32;
    char modeStringBuffer[BUF_SIZE];
    
    switch(r->mode) {
        case MODE_PENTOMINO:
            strncpy(modeStringBuffer, "PENTOMINO", BUF_SIZE);
            break;
        case MODE_G2_DEATH:
            strncpy(modeStringBuffer, "G2 DEATH", BUF_SIZE);
            break;
        case MODE_G3_TERROR:
            strncpy(modeStringBuffer, "G3 TERROR", BUF_SIZE);
            break;
        case MODE_G1_20G:
            strncpy(modeStringBuffer, "G1 20G", BUF_SIZE);
            break;
        case MODE_G1_MASTER:
            strncpy(modeStringBuffer, "G1 MASTER", BUF_SIZE);
            break;
        case MODE_G2_MASTER:
            strncpy(modeStringBuffer, "G2 MASTER", BUF_SIZE);
            break;
        default:
            break;
    }

    nz_timer *t = nz_timer_create(60);
    t->time = r->time;

    char dateBuffer[BUF_SIZE];
    struct tm *ts = localtime(&r->date);
    strftime(dateBuffer, BUF_SIZE, "%Y.%m.%d", ts);

    snprintf(buffer, bufferLength, "%s  %-10s %4d-%-4d  %02d:%02d:%02d   %s",
             get_grade_name(r->grade),
             modeStringBuffer,
             r->starting_level,
             r->ending_level,
             timegetmin(t),
             timegetsec(t) % 60,
             timegetmsec(t) / 10,
             dateBuffer);
}

void read_replay_from_memory(struct replay *out_replay, const uint8_t *buffer, size_t bufferLength)
{
    // Keep the same existing format
    const uint8_t *scanner = buffer;

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

    memcpy(&out_replay->inputs[0], scanner, out_replay->len * sizeof(struct keyflags));
}

uint8_t* generate_raw_replay(struct replay *r, size_t *out_replayLength)
{
    // TODO: Endianness?
    uint8_t *buffer = malloc(sizeof(struct replay));
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
    
    memcpy(buffer + bufferOffset, &r->inputs, sizeof(struct keyflags) * r->len);
    bufferOffset += sizeof(struct keyflags) * r->len;

    *out_replayLength = bufferOffset;
    
    return buffer;
}

void dispose_raw_replay(void* data)
{
    free(data);
}
