//
// Created by ds on 12/23/25.
//
#include <string.h>

#include "test.h"
#include "../backend/Lexer/source.h"

// ---------------------------------------------------------------------------------------------------------------------

DEFINE_TEST(TEST_LX_source_text_real)
DEFINE_TEST(TEST_LX_source_text_empty)
DEFINE_TEST(TEST_LX_source_text_null)
DEFINE_TEST(TEST_LX_source_file_real)
DEFINE_TEST(TEST_LX_source_file_empty)
DEFINE_TEST(TEST_LX_source_file_null)
DEFINE_TEST(TEST_LX_source_file_toolong)

// ---------------------------------------------------------------------------------------------------------------------
BEGIN_TEST_SET(LEXER_SOURCE)
  RUN_TEST(TEST_LX_source_text_real)
  RUN_TEST(TEST_LX_source_text_empty)
  RUN_TEST(TEST_LX_source_text_null)
  RUN_TEST(TEST_LX_source_file_real)
  RUN_TEST(TEST_LX_source_file_empty)
  RUN_TEST(TEST_LX_source_file_null)
  RUN_TEST(TEST_LX_source_file_toolong)
END_TEST_SET
// ---------------------------------------------------------------------------------------------------------------------

BEGIN_TEST(TEST_LX_source_text_real)
  const char *testText = "real text";
  source_t src = SOURCE_Init_FromText(testText);

  TEST_ASSERT(src.type == SC_TEXT)
  TEST_ASSERT(strlen(src.filename) == 0)
  TEST_ASSERT(src.length == strlen(testText))
  TEST_ASSERT(strcmp(testText, src.buffer) == 0)

  SOURCE_Unload(&src);
END_TEST

BEGIN_TEST(TEST_LX_source_text_empty)
  source_t src = SOURCE_Init_FromText("");

  TEST_ASSERT(src.type == SC_TEXT)
  TEST_ASSERT(strlen(src.filename) == 0)
  TEST_ASSERT(src.length == 0)
  TEST_ASSERT(strlen(src.buffer) == 0)

  SOURCE_Unload(&src);
END_TEST

BEGIN_TEST(TEST_LX_source_text_null)
  source_t src = SOURCE_Init_FromText(NULL);

  TEST_ASSERT(src.type == SC_EMPTY)
  TEST_ASSERT(strlen(src.filename) == 0)
  TEST_ASSERT(src.length == 0)
  TEST_ASSERT(strlen(src.buffer) == 0)

  SOURCE_Unload(&src);
END_TEST

BEGIN_TEST(TEST_LX_source_file_real)

  const char *testFile = "./test.bee";
  const char *testText = "real text";

  // write a temp file first
  FILE *file = fopen(testFile, "w");
  fputs(testText, file);
  fclose(file);

  // try to read it into a source
  source_t src = SOURCE_Init_FromFile(testFile);

  TEST_ASSERT(src.type == SC_FILE)
  TEST_ASSERT(strcmp(testFile, src.filename) == 0)
  TEST_ASSERT(src.length == strlen(testText))
  TEST_ASSERT(strcmp(testText, src.buffer) == 0)

  SOURCE_Unload(&src);
END_TEST

BEGIN_TEST(TEST_LX_source_file_empty)
  // try to read a nonexistant file
  source_t src = SOURCE_Init_FromFile("");

  TEST_ASSERT(src.type == SC_EMPTY)
  TEST_ASSERT(strlen(src.filename) == 0)
  TEST_ASSERT(src.length == 0)
  TEST_ASSERT(strlen(src.buffer) == 0)

  SOURCE_Unload(&src);
END_TEST

BEGIN_TEST(TEST_LX_source_file_null)
  // try to read a null filename
  source_t src = SOURCE_Init_FromFile(NULL);

  TEST_ASSERT(src.type == SC_EMPTY)
  TEST_ASSERT(strlen(src.filename) == 0)
  TEST_ASSERT(src.length == 0)
  TEST_ASSERT(strlen(src.buffer) == 0)

  SOURCE_Unload(&src);
END_TEST

BEGIN_TEST(TEST_LX_source_file_toolong)
  // try to read a null filename
  source_t src = SOURCE_Init_FromFile("this/is/far/longer/than/the/two/hundred/and/fifty/five/allowed/characters/in/a/file/path/i/honestly/cant/think/of/anything/more/to/write/like/honestly/this/is/challenging/but/im/not/there/yet/but/were/getting/close/just/a/few/more/chars/aaaand/this/should/do/it");

  TEST_ASSERT(src.type == SC_EMPTY)
  TEST_ASSERT(strlen(src.filename) == 0)
  TEST_ASSERT(src.length == 0)
  TEST_ASSERT(strlen(src.buffer) == 0)

  SOURCE_Unload(&src);
END_TEST