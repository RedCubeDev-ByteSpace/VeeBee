//
// Created by ds on 12/23/25.
//

#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#include <stdbool.h>

#define DEFINE_TEST(NAME) bool NAME();

#define BEGIN_TEST_SET(NAME)                               \
  int main() {                                             \
    bool testOk = true;                                    \
    bool thisTestOk;                                       \
                                                           \
    printf("\nTestset \"%s\"\n", #NAME);                   \

#define END_TEST_SET                                       \
    if (testOk) return 0;                                  \
    else return 1;                                         \
  }                                                        \

#define RUN_TEST(TEST)                                     \
                                                           \
  thisTestOk = (TEST());                                   \
                                                           \
  if (thisTestOk) {                                        \
    printf("  OK\n");                                      \
  } else {                                                 \
    printf("  FAILED\n");                                  \
  }                                                        \
                                                           \
  testOk &= thisTestOk;                                    \



#define BEGIN_TEST(TEST_NAME)                              \
  bool TEST_NAME() {                                       \
    printf(" -> %s: ", #TEST_NAME);                        \
    fflush(stdout);                                        \
    bool passed = true;                                    \

#define TEST_ASSERT(ASSERTION)                             \
  if (!(ASSERTION)) {                                      \
  printf("\n    [!] Failed assertion \"%s\"", #ASSERTION); \
  passed = false;                                          \
  }

#define TEST_NOT_IMPLEMENTED()              \
  passed = false;                           \
  printf("\n    [!] Test not implemented"); \


#define END_TEST                                           \
    if (!passed) printf("\n  ");                           \
    return passed;                                         \
  }                                                        \

#endif //TEST_H
