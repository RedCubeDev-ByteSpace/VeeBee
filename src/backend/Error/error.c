//
// Created by ds on 12/21/25.
//
#include <stdio.h>
#include "error.h"

void error(err_subsystem_t subsystem, error_type_t type, const char *msg) {
    printf("\n[%s](%s): %s\n", SUBSYSTEM_NAMES[subsystem], ERROR_TYPE_NAMES[type], msg);
}