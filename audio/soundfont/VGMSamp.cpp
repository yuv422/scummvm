/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "VGMSamp.h"

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

// ***********
// VGMSampColl
// ***********

VGMSampColl::VGMSampColl(const Common::String &format, RawFile *rawfile, uint32_t offset, uint32_t length,
						 Common::String theName)
		: VGMFile(format, rawfile, offset, length, theName),
		  parInstrSet(NULL),
		  bLoaded(false),
		  sampDataOffset(0) {
	AddContainer<VGMSamp>(samples);
}

VGMSampColl::VGMSampColl(const Common::String &format, RawFile *rawfile, VGMInstrSet *instrset,
						 uint32_t offset, uint32_t length, Common::String theName)
		: VGMFile(format, rawfile, offset, length, theName),
		  parInstrSet(instrset),
		  bLoaded(false),
		  sampDataOffset(0) {
	AddContainer<VGMSamp>(samples);
}

VGMSampColl::~VGMSampColl(void) {
	DeleteVect<VGMSamp>(samples);
}

bool VGMSampColl::Load() {
	if (bLoaded)
		return true;
	if (!GetHeaderInfo())
		return false;
	if (!GetSampleInfo())
		return false;

	if (samples.size() == 0)
		return false;

	if (unLength == 0) {
		for (Common::Array<VGMSamp *>::iterator itr = samples.begin(); itr != samples.end(); ++itr) {
			VGMSamp *samp = (*itr);

			// Some formats can have negative sample offset
			// For example, Konami's SNES format and Hudson's SNES format
			// TODO: Fix negative sample offset without breaking instrument
			// assert(dwOffset <= samp->dwOffset);

			// if (dwOffset > samp->dwOffset)
			//{
			//	unLength += samp->dwOffset - dwOffset;
			//	dwOffset = samp->dwOffset;
			//}

			if (dwOffset + unLength < samp->dwOffset + samp->unLength) {
				unLength = (samp->dwOffset + samp->unLength) - dwOffset;
			}
		}
	}

	bLoaded = true;
	return true;
}
