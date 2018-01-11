#include "scores.h"
#include "replay.h"

#include "sqlite3.h"

#include "zed_dbg.h"

#include <stdlib.h>

struct scoredb *createOrLoadScoreDb(const char* filename)
{   
    struct scoredb *s = malloc(sizeof(struct scoredb));

    int ret = sqlite3_open_v2(filename, &s->db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    check(ret == SQLITE_OK, "Could not open/create sqlite database: %s", sqlite3_errmsg(s->db));
    
    // TODO: Actually design the database. Replay table? Player table + related columns?
    const char *createTableSql = "CREATE TABLE IF NOT EXISTS scores (scoreId INTEGER PRIMARY KEY, mode INTEGER, grade INTEGER, level INTEGER, time INTEGER, replay BLOB, date INTEGER);";
    ret = sqlite3_exec(s->db, createTableSql, NULL, NULL, NULL);
    check(ret == 0, "Could not create scores table");
    
    // TODO: Create a view with human-readable data
    
    return s;
    
 error:
    return NULL;
}

void scoredb_destroy(struct scoredb *s)
{
    /* check(sqlite3_close(s->db) != SQLITE_OK, "Could not close sqlite database: %s", sqlite3_errmsg(s->db));  */
    sqlite3_close(s->db);
    free(s);
}

void scoredb_add(struct scoredb *s, struct replay *r)
{
    sqlite3_stmt *sql;
    
    const char *insertSql = "INSERT INTO scores (mode, grade, level, time, replay, date) VALUES (:mode, :grade, :level, :time, :replay, strftime('%s', 'now'));";
    sqlite3_prepare_v2(s->db, insertSql, -1, &sql, NULL);
    
    size_t replayLen = 0;
    uint8_t *replayData = generate_raw_replay(r, &replayLen);
    
    sqlite3_bind_int(sql,  sqlite3_bind_parameter_index(sql, ":mode"),   r->mode);
    sqlite3_bind_int(sql,  sqlite3_bind_parameter_index(sql, ":grade"),  r->grade);
    sqlite3_bind_int(sql,  sqlite3_bind_parameter_index(sql, ":level"),  r->ending_level);
    sqlite3_bind_int(sql,  sqlite3_bind_parameter_index(sql, ":time"),   r->time);
    sqlite3_bind_blob(sql, sqlite3_bind_parameter_index(sql, ":replay"), replayData, replayLen, dispose_raw_replay);
    
    int ret = sqlite3_step(sql);
    check(ret == SQLITE_DONE, "Could not insert value into scores table: %s", sqlite3_errmsg(s->db));
        
 error:
    sqlite3_finalize(sql);
}

int scoredb_get_replay_count(struct scoredb *s)
{
    int replayCount = 0;
    const char *getReplayCountSql = "SELECT COUNT(*) FROM scores;";

    sqlite3_stmt *sql;
    sqlite3_prepare_v2(s->db, getReplayCountSql, -1, &sql, NULL);
    
    int ret = sqlite3_step(sql);
    check(ret == SQLITE_ROW, "Could not get replay count: %s", sqlite3_errmsg(s->db));
    
    replayCount = sqlite3_column_int(sql, 0);
    
 error:
    sqlite3_finalize(sql);
    
    return replayCount;
}

struct replay *scoredb_get_replay_list(struct scoredb *s, int page, int *out_replayCount)
{
    int replayCount = scoredb_get_replay_count(s);
    struct replay *replayList = malloc(sizeof(struct replay) * replayCount);
    
    // TODO: Only show current player's replays?
    // TODO: Pagination? Current interface expects a full list of replays
    static const int pageSize = 20;
    const char *getReplayListSql = "SELECT scoreId, mode, grade, level, time, date FROM scores ORDER BY mode, level DESC, time;";

    sqlite3_stmt *sql;
    sqlite3_prepare_v2(s->db, getReplayListSql, -1, &sql, NULL);
    
    /* sqlite3_bind_int(sql, sqlite3_bind_parameter_index(sql, ":pageSize"), pageSize); */
    /* sqlite3_bind_int(sql, sqlite3_bind_parameter_index(sql, ":offset"),  page * pageSize); */
    
    for (int i = 0; i < replayCount; i++)
    {
        int ret = sqlite3_step(sql);
        check(ret == SQLITE_ROW, "Could not get replay count: %s", sqlite3_errmsg(s->db));
        
        replayList[i].index        = sqlite3_column_int(sql, 0);
        replayList[i].mode         = sqlite3_column_int(sql, 1);
        replayList[i].grade        = sqlite3_column_int(sql, 2);
        replayList[i].ending_level = sqlite3_column_int(sql, 3);
        replayList[i].time         = sqlite3_column_int(sql, 4);
        replayList[i].date         = sqlite3_column_int(sql, 5);
    }
    
 error:
    sqlite3_finalize(sql);

    *out_replayCount = replayCount;
    return replayList;
}


void scoredb_get_full_replay(struct scoredb *s, struct replay* out_replay, int replay_id)
{
    const char *getReplaySql = "SELECT replay FROM scores WHERE scoreId = :scoreId;";
    
    sqlite3_stmt *sql;
    sqlite3_prepare_v2(s->db, getReplaySql, -1, &sql, NULL);
    
    sqlite3_bind_int(sql, sqlite3_bind_parameter_index(sql, ":scoreId"), replay_id);
    
    int ret = sqlite3_step(sql);
    check(ret == SQLITE_ROW, "Could not get replay count: %s", sqlite3_errmsg(s->db));
    
    int replayBufferLength = sqlite3_column_bytes(sql, 0);
    const uint8_t *replayBuffer = sqlite3_column_blob(sql, 0);
    
    read_replay_from_memory(out_replay, replayBuffer, replayBufferLength);
    
 error:
    sqlite3_finalize(sql);
}
