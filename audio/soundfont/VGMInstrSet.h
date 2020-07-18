/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#pragma once
#include "common/scummsys.h"
#include "common/str.h"
#include "common/array.h"
#include "VGMFile.h"
//#include "DLSFile.h"
#include "SF2File.h"

class VGMSampColl;
class VGMInstr;
class VGMRgn;
class VGMSamp;
class VGMRgnItem;
// class VGMArt;

// ***********
// VGMInstrSet
// ***********

class VGMInstrSet : public VGMFile {
   public:

    VGMInstrSet(const Common::String &format, RawFile *file, uint32_t offset, uint32_t length = 0,
                Common::String name = "VGMInstrSet", VGMSampColl *theSampColl = NULL);
    virtual ~VGMInstrSet(void);

    virtual bool Load();
    virtual bool GetHeaderInfo();
    virtual bool GetInstrPointers();
    virtual bool LoadInstrs();

    VGMInstr *AddInstr(uint32_t offset, uint32_t length, unsigned long bank, unsigned long instrNum,
                       const Common::String &instrName = "");

//TODO do we need this?    virtual FileType GetFileType() { return FILETYPE_INSTRSET; }

    bool OnSaveAsDLS(void);
    bool OnSaveAsSF2(void);
    virtual bool SaveAsDLS(const Common::String &filepath);
    virtual bool SaveAsSF2(const Common::String &filepath);

   public:
    Common::Array<VGMInstr *> aInstrs;
    VGMSampColl *sampColl;
};

// ********
// VGMInstr
// ********

class VGMInstr : public VGMContainerItem {
   public:
    VGMInstr(VGMInstrSet *parInstrSet, uint32_t offset, uint32_t length, uint32_t bank,
             uint32_t instrNum, const Common::String &name = "Instrument");
    virtual ~VGMInstr(void);

//TODO    virtual Icon GetIcon() { return ICON_INSTR; };

    inline void SetBank(uint32_t bankNum);
    inline void SetInstrNum(uint32_t theInstrNum);

    VGMRgn *AddRgn(VGMRgn *rgn);
    VGMRgn *AddRgn(uint32_t offset, uint32_t length, int sampNum, uint8_t keyLow = 0,
                   uint8_t keyHigh = 0x7F, uint8_t velLow = 0, uint8_t velHigh = 0x7F);

    virtual bool LoadInstr();

   public:
    uint32_t bank;
    uint32_t instrNum;

    VGMInstrSet *parInstrSet;
    Common::Array<VGMRgn *> aRgns;
};
