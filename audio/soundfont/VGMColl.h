/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#pragma once

#include "common.h"
#include "common/str.h"
#include "common/array.h"
#include "VGMItem.h"
//#include "Menu.h"

class VGMSeq;
class VGMInstrSet;
class VGMSampColl;
class VGMSamp;
class SF2File;
class SynthFile;

class VGMColl : public VGMItem {
   public:
    explicit VGMColl(Common::String name = "Unnamed Collection");
    virtual ~VGMColl() = default;

    void RemoveFileAssocs();
    [[nodiscard]] const Common::String &GetName() const;
    void SetName(const Common::String *newName);
    VGMSeq *GetSeq();
    void UseSeq(VGMSeq *theSeq);
    void AddInstrSet(VGMInstrSet *theInstrSet);
    void AddSampColl(VGMSampColl *theSampColl);
    void AddMiscFile(VGMFile *theMiscFile);
    bool Load();
    virtual bool LoadMain() { return true; }
//    virtual bool CreateDLSFile(DLSFile &dls);
    virtual SF2File *CreateSF2File();
    virtual bool PreDLSMainCreation() { return true; }
    virtual SynthFile *CreateSynthFile();
//    virtual bool MainDLSCreation(DLSFile &dls);
    virtual bool PostDLSMainCreation() { return true; }

    // This feels stupid, but the current callbacks system
    // is not exactly flexible.
    inline void SetDefaultSavePath(Common::String savepath) { dirpath = std::move(savepath); }

    VGMSeq *seq;
    Common::Array<VGMInstrSet *> instrsets;
    Common::Array<VGMSampColl *> sampcolls;
    Common::Array<VGMFile *> miscfiles;

   protected:
    void UnpackSampColl(SynthFile &synthfile, VGMSampColl *sampColl,
                        Common::Array<VGMSamp *> &finalSamps);

   protected:
    Common::String name;
    Common::String dirpath = "";
};
