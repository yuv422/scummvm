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

    virtual bool LoadInstr();

   public:
    uint32_t bank;
    uint32_t instrNum;

    VGMInstrSet *parInstrSet;
    Common::Array<VGMRgn *> aRgns;
};
