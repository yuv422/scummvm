/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "VGMSamp.h"
#include "VGMSampColl.h"
//#include "Root.h"

// *******
// VGMSamp
// *******

VGMSamp::VGMSamp(VGMSampColl *sampColl, uint32_t offset, uint32_t length, uint32_t dataOffset,
                 uint32_t dataLen, uint8_t nChannels, uint16_t theBPS, uint32_t theRate,
                 Common::String theName)
    : parSampColl(sampColl),
      sampName(theName),
      VGMItem(sampColl->vgmfile, offset, length),
      dataOff(dataOffset),
      dataLength(dataLen),
      bps(theBPS),
      rate(theRate),
      ulUncompressedSize(0),
      channels(nChannels),
      pan(0),
      unityKey(-1),
      fineTune(0),
      volume(-1),
      waveType(WT_UNDEFINED),
      bPSXLoopInfoPrioritizing(false) {
    name = sampName;  // I would do this in the initialization list, but VGMItem()
                             // constructor is called before sampName is initialized,
    // so data() ends up returning a bad pointer
}

VGMSamp::~VGMSamp() {}

double VGMSamp::GetCompressionRatio() {
    return 1.0;
}
