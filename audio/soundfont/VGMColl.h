/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#ifndef AUDIO_SOUNDFONT_VGMCOLL_H
#define AUDIO_SOUNDFONT_VGMCOLL_H

#include "common.h"
#include "common/str.h"
#include "common/array.h"
#include "VGMItem.h"

class VGMInstrSet;
class VGMSampColl;
class VGMSamp;
class SF2File;
class SynthFile;

class VGMColl : public VGMItem {
   public:
    explicit VGMColl(Common::String name = "Unnamed Collection");
    virtual ~VGMColl() = default;

    void AddInstrSet(VGMInstrSet *theInstrSet);
    virtual SF2File *CreateSF2File();
    virtual SynthFile *CreateSynthFile();

    Common::Array<VGMInstrSet *> instrsets;
    Common::Array<VGMSampColl *> sampcolls;

   protected:
    void UnpackSampColl(SynthFile &synthfile, VGMSampColl *sampColl,
                        Common::Array<VGMSamp *> &finalSamps);

   protected:
    Common::String name;
};
#endif // AUDIO_SOUNDFONT_VGMCOLL_H
