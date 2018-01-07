#include <stdlib.h>

#include "zed_dbg.h"
#include "scores.h"
#include "replay.h"

#include "sqlite3.h"

struct scoredb *createOrLoadScoreDb(const char* filename)
{   
    struct scoredb *s = malloc(sizeof(struct scoredb));

    int ret = sqlite3_open_v2(filename, &s->db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    check(ret == SQLITE_OK, "Could not open/create sqlite database: %s", sqlite3_errmsg(s->db));
    
    const char *createTableSql = "CREATE TABLE IF NOT EXISTS scores (scoreId PRIMARY KEY, mode INTEGER, grade INTEGER, level INTEGER, time INTEGER, date TEXT);";
    check(sqlite3_exec(s->db, createTableSql, NULL, NULL, NULL) == 0, "Could not create scores table");
    
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
    
    const char* insertSql = "INSERT INTO scores (mode, grade, level, time, date)  VALUES (:mode, :grade, :level, :time, DATETIME('now'));";
    sqlite3_prepare_v2(s->db, insertSql, -1, &sql, NULL);
    
    sqlite3_bind_int(sql, sqlite3_bind_parameter_index(sql, ":mode"),  r->mode);
    sqlite3_bind_int(sql, sqlite3_bind_parameter_index(sql, ":grade"), r->grade);
    sqlite3_bind_int(sql, sqlite3_bind_parameter_index(sql, ":level"), r->ending_level);
    sqlite3_bind_int(sql, sqlite3_bind_parameter_index(sql, ":time"),  r->time);
    
    check(sqlite3_step(sql) == SQLITE_DONE, "Could not insert value into scores table: %s", sqlite3_errmsg(s->db));
        
 error:
    sqlite3_finalize(sql);
}
