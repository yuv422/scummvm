/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common.h"
#include "VGMSampColl.h"
#include "VGMSamp.h"

using namespace std;

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
