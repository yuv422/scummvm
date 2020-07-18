/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#pragma once

#include "common/scummsys.h"

enum LoopMeasure { LM_SAMPLES, LM_BYTES };

struct Loop {
    Loop()
        : loopStatus(-1),
          loopType(0),
          loopStartMeasure(LM_BYTES),
          loopLengthMeasure(LM_BYTES),
          loopStart(0),
          loopLength(0) {}

    int loopStatus;
    uint32 loopType;
    uint8 loopStartMeasure;
    uint8 loopLengthMeasure;
    uint32 loopStart;
    uint32 loopLength;
};
