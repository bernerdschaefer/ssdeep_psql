#include "postgres.h"
#include "fmgr.h"
#include <fuzzy.h>

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_fuzzy_hash);
Datum pg_fuzzy_hash(PG_FUNCTION_ARGS);

Datum pg_fuzzy_hash(PG_FUNCTION_ARGS) {
  text *pg_story = PG_GETARG_TEXT_P(0);
  int pg_story_size = VARSIZE(pg_story);

  char *hash = (char *) palloc(FUZZY_MAX_RESULT);
  text *pg_hash;
  int hash_length;

  fuzzy_hash_buf((unsigned char *) VARDATA(pg_story), pg_story_size, hash);

  hash_length = strlen(hash);
  pg_hash = (text *) palloc(hash_length);
  SET_VARSIZE(pg_hash, hash_length);
  memcpy(VARDATA(pg_hash), hash, hash_length);

  pfree(hash);

  PG_RETURN_TEXT_P(pg_hash);
}
// 
// CREATE OR REPLACE FUNCTION fuzzy_hash(TEXT) RETURNS TEXT AS 'ssdeep_psql.so', 'pg_fuzzy_hash' LANGUAGE 'C';
//

PG_FUNCTION_INFO_V1(pg_fuzzy_compare);
Datum pg_fuzzy_compare(PG_FUNCTION_ARGS);

Datum pg_fuzzy_compare(PG_FUNCTION_ARGS) {
  char *hash1, *hash2;
  text *arg1 = PG_GETARG_TEXT_P(0);
  text *arg2 = PG_GETARG_TEXT_P(1);
  int32 score;

  hash1 = (char *) palloc(FUZZY_MAX_RESULT);
  hash2 = (char *) palloc(FUZZY_MAX_RESULT);

  fuzzy_hash_buf((unsigned char *) VARDATA(arg1), VARSIZE(arg1), hash1);
  fuzzy_hash_buf((unsigned char *) VARDATA(arg2), VARSIZE(arg2), hash2);

  score = (int32) fuzzy_compare(hash1, hash2);

  pfree(hash1);
  pfree(hash2);

  PG_RETURN_INT32(score);
}
// 
// CREATE OR REPLACE FUNCTION fuzzy_compare(TEXT, TEXT) RETURNS INTEGER AS 'ssdeep_psql.so', 'pg_fuzzy_compare' LANGUAGE 'C';
//
