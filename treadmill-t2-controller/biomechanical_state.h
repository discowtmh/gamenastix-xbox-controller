// This file is a part of Treadmill project.
// Copyright 2018 Disco WTMH S.A.

#ifndef TREADMILL_T2_CONTROLLER_BIOMECHANICAL_STATE_H
#define TREADMILL_T2_CONTROLLER_BIOMECHANICAL_STATE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct BiomechanicalState {
    int leftFootX;
    int leftFootY;
    int leftFootZ;
    int rightFootX;
    int rightFootY;
    int rightFootZ;
    int backpackX;
    int backpackY;
    int backpackZ;
};

extern struct BiomechanicalState biomechanicalState;

#ifdef __cplusplus
}
#endif

#endif
