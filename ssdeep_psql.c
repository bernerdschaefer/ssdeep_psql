#include "postgres.h"
#include "fmgr.h"
#include <fuzzy.h>

PG_FUNCTION_INFO_V1(ssdeep_hash_generate);

Datum ssdeep_hash_generate(PG_FUNCTION_ARGS) {
  text *pg_story = PG_GETARG_TEXT_P(0);
  size_t story_length = VARSIZE(pg_story) - VARHDRSZ;
  unsigned char *story = (unsigned char *) palloc(story_length + 1);
  char *hash = (char *) palloc(FUZZY_MAX_RESULT);

  memcpy(story, VARDATA(pg_story), story_length);
  story[story_length] = '\0';

  fuzzy_hash_buf(story, story_length, hash);

  pfree(story);

  PG_RETURN_CSTRING(story);
}