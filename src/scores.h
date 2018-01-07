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

#endif // __SCOREDB_H_
