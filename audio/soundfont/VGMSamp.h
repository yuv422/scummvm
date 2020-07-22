/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#pragma once
#include "common.h"
#include "common/scummsys.h"
#include "common/str.h"
#include "VGMItem.h"

class VGMSampColl;
class VGMInstrSet;

enum WAVE_TYPE { WT_UNDEFINED, WT_PCM8, WT_PCM16 };

class VGMSamp : public VGMItem {
   public:
	
    VGMSamp(VGMSampColl *sampColl, uint32_t offset = 0, uint32_t length = 0,
            uint32_t dataOffset = 0, uint32_t dataLength = 0, uint8_t channels = 1,
            uint16_t bps = 16, uint32_t rate = 0, Common::String name = "Sample");
    virtual ~VGMSamp();

    virtual double GetCompressionRatio();  // ratio of space conserved.  should generally be > 1
    // used to calculate both uncompressed sample size and loopOff after conversion
	virtual void ConvertToStdWave(uint8_t *buf) {};

    inline void SetLoopStatus(int loopStat) { loop.loopStatus = loopStat; }
    inline void SetLoopOffset(uint32_t loopStart) { loop.loopStart = loopStart; }
    inline void SetLoopLength(uint32_t theLoopLength) { loop.loopLength = theLoopLength; }

   public:
    WAVE_TYPE waveType;
    uint32_t dataOff;  // offset of original sample data
    uint32_t dataLength;
    uint16_t bps;      // bits per sample
    uint32_t rate;     // sample rate in herz (samples per second)
    uint8_t channels;  // mono or stereo?
    uint32_t ulUncompressedSize;

    bool bPSXLoopInfoPrioritizing;
    Loop loop;

    int8_t unityKey;
    short fineTune;
    double
        volume;  // as percent of full volume.  This will be converted to attenuation for SynthFile

    long pan;

    VGMSampColl *parSampColl;
    Common::String sampName;
};

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
