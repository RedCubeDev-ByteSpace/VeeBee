//
// Created by ds on 12/21/25.
//
#include "error.h"

#include <stdio.h>

void error(err_subsystem_t subsystem, error_type_t type, const char *msg) {
    printf("[%s](%s): %s", SUBSYSTEM_NAMES[subsystem], ERROR_TYPE_NAMES[type], msg);
}
