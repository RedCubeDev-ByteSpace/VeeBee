//
// Created by ds on 1/29/26.
//
#include "binder.h"

#include <stdlib.h>

binder_t *BINDER_Init() {
    binder_t *newBinder = malloc(sizeof(binder_t));

    // initialize the program unit
    newBinder->programUnit = BD_PROGRAM_UNIT_Init();

    return newBinder;
}
