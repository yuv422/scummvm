/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#ifndef AUDIO_SOUNDFONT_VAB_H
#define AUDIO_SOUNDFONT_VAB_H
#include "audio/soundfont/common.h"
#include "common/str.h"
#include "audio/soundfont/vgminstrset.h"
#include "audio/soundfont/vgmsamp.h"

// VAB Header
struct VabHdr {
    int32_t form;
    /*always "VABp"*/
    int32_t ver;
    /*format version number*/
    int32_t id;
    /*bank ID*/
    uint32 fsize;
    /*file size*/
    uint16 reserved0;
    /*system reserved*/
    uint16 ps;
    /*total number of programs in this bank*/
    uint16 ts;
    /*total number of effective tones*/
    uint16 vs;
    /*number of waveforms (VAG)*/
    uint8 mvol;
    /*master volume*/
    uint8 pan;
    /*master pan*/
    uint8 attr1;
    /*bank attribute*/
    uint8 attr2;
    /*bank attribute*/
    uint32 reserved1; /*system reserved*/
};

// Program Attributes
struct ProgAtr {
    uint8 tones;
    /*number of effective tones which compose the program*/
    uint8 mvol;
    /*program volume*/
    uint8 prior;
    /*program priority*/
    uint8 mode;
    /*program mode*/
    uint8 mpan;
    /*program pan*/
    int8_t reserved0;
    /*system reserved*/
    int16_t attr;
    /*program attribute*/
    uint32 reserved1;
    /*system reserved*/
    uint32 reserved2; /*system reserved*/
};

// Tone Attributes
struct VagAtr {
    uint8 prior;
    /*tone priority (0 - 127); used for controlling allocation when more voices than can be keyed on
     * are requested*/
    uint8 mode;
    /*tone mode (0 = normal; 4 = reverb applied */
    uint8 vol;
    /*tone volume*/
    uint8 pan;
    /*tone pan*/
    uint8 center;
    /*center note (0~127)*/
    uint8 shift;
    /*pitch correction (0~127,cent units)*/
    uint8 min;
    /*minimum note limit (0~127)*/
    uint8 max;
    /*maximum note limit (0~127, provided min < max)*/
    uint8 vibW;
    /*vibrato width (1/128 rate,0~127)*/
    uint8 vibT;
    /*1 cycle time of vibrato (tick units)*/
    uint8 porW;
    /*portamento width (1/128 rate, 0~127)*/
    uint8 porT;
    /*portamento holding time (tick units)*/
    uint8 pbmin;
    /*pitch bend (-0~127, 127 = 1 octave)*/
    uint8 pbmax;
    /*pitch bend (+0~127, 127 = 1 octave)*/
    uint8 reserved1;
    /*system reserved*/
    uint8 reserved2;
    /*system reserved*/
    uint16 adsr1;
    /*ADSR1*/
    uint16 adsr2;
    /*ADSR2*/
    int16_t prog;
    /*parent program*/
    int16_t vag;
    /*waveform (VAG) used*/
    int16_t reserved[4]; /*system reserved*/
};

class Vab : public VGMInstrSet {
   public:
    Vab(RawFile *file, uint32 offset);
    virtual ~Vab(void);

    virtual bool GetHeaderInfo();
    virtual bool GetInstrPointers();

   public:
    VabHdr hdr;
};

// ********
// VabInstr
// ********

class VabInstr : public VGMInstr {
   public:
    VabInstr(VGMInstrSet *instrSet, uint32 offset, uint32 length, uint32 theBank,
             uint32 theInstrNum, const Common::String &name = "Instrument");
    virtual ~VabInstr(void);

    virtual bool LoadInstr();

   public:
    ProgAtr attr;
    uint8 masterVol;
};

// ******
// VabRgn
// ******

class VabRgn : public VGMRgn {
   public:
    VabRgn(VabInstr *instr, uint32 offset);

    virtual bool LoadRgn();

   public:
    uint16 ADSR1;  // raw ps2 ADSR1 value (articulation data)
    uint16 ADSR2;  // raw ps2 ADSR2 value (articulation data)

    VagAtr attr;
};
#endif // AUDIO_SOUNDFONT_VAB_H
