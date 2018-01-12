#ifndef __SCOREDB_H_
#define __SCOREDB_H_

struct sqlite3;
typedef struct sqlite3 sqlite3;

struct replay;

struct scoredb {
    sqlite3 *db;
};

struct scoredb *createOrLoadScoreDb(const char* filename);
void scoredb_destroy(struct scoredb *s);

void scoredb_add(struct scoredb *s, struct replay *r);

int scoredb_get_replay_count(struct scoredb *s);

// Get list of replay descriptors (no replay data)
struct replay *scoredb_get_replay_list(struct scoredb *s, int page, int *out_replayCount);

void scoredb_get_full_replay(struct scoredb *s, struct replay *out_replay, int replay_id);

void scoredb_get_full_replay_by_condition(struct scoredb *s, struct replay *out_replay, int mode);

#endif // __SCOREDB_H_
