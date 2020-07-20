/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common.h"
#include "VGMInstrSet.h"
#include "VGMSamp.h"
#include "VGMRgn.h"

using namespace std;

// ***********
// VGMInstrSet
// ***********

VGMInstrSet::VGMInstrSet(const Common::String &format, /*FmtID fmtID,*/
                         RawFile *file, uint32_t offset, uint32_t length, Common::String name,
                         VGMSampColl *theSampColl)
    : VGMFile(format, file, offset, length, name),
      sampColl(theSampColl) {
    AddContainer<VGMInstr>(aInstrs);
}

VGMInstrSet::~VGMInstrSet() {
    DeleteVect<VGMInstr>(aInstrs);
    delete sampColl;
}

bool VGMInstrSet::Load() {
    if (!GetHeaderInfo())
        return false;
    if (!GetInstrPointers())
        return false;
    if (!LoadInstrs())
        return false;

    if (aInstrs.size() == 0)
        return false;

    if (sampColl != NULL) {
        if (!sampColl->Load()) {
          error("Failed to load VGMSampColl");
        }
    }

    return true;
}

bool VGMInstrSet::GetHeaderInfo() {
    return true;
}

bool VGMInstrSet::GetInstrPointers() {
    return true;
}

bool VGMInstrSet::LoadInstrs() {
    size_t nInstrs = aInstrs.size();
    for (size_t i = 0; i < nInstrs; i++) {
        if (!aInstrs[i]->LoadInstr())
            return false;
    }
    return true;
}

// ********
// VGMInstr
// ********

VGMInstr::VGMInstr(VGMInstrSet *instrSet, uint32_t offset, uint32_t length, uint32_t theBank,
                   uint32_t theInstrNum, const Common::String &name)
    : VGMContainerItem(instrSet, offset, length, name),
      parInstrSet(instrSet),
      bank(theBank),
      instrNum(theInstrNum) {
    AddContainer<VGMRgn>(aRgns);
}

VGMInstr::~VGMInstr() {
    DeleteVect<VGMRgn>(aRgns);
}

bool VGMInstr::LoadInstr() {
    return true;
}
