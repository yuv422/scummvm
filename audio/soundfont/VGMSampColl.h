/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#pragma once
#include "VGMFile.h"

class VGMInstrSet;
class VGMSamp;

class VGMSampColl : public VGMFile {
   public:
    VGMSampColl(const Common::String &format, RawFile *rawfile, uint32_t offset, uint32_t length = 0,
                Common::String theName = "VGMSampColl");
    VGMSampColl(const Common::String &format, RawFile *rawfile, VGMInstrSet *instrset, uint32_t offset,
                uint32_t length = 0, Common::String theName = "VGMSampColl");
    virtual ~VGMSampColl(void);

    virtual bool Load();
    virtual bool GetHeaderInfo() { return true; }  // retrieve any header data
    virtual bool GetSampleInfo() { return true; }  // retrieve sample info, including pointers to data, # channels, rate, etc.

   protected:

   public:
    bool bLoaded;

    uint32_t sampDataOffset;  // offset of the beginning of the sample data.  Used for
                              // rgn->sampOffset matching
    VGMInstrSet *parInstrSet;
    Common::Array<VGMSamp *> samples;
};
