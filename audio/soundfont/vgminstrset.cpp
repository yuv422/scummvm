/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common.h"
#include "vgminstrset.h"
#include "vgmsamp.h"

using namespace std;

// ***********
// VGMInstrSet
// ***********

VGMInstrSet::VGMInstrSet(const Common::String &theFormat, /*FmtID fmtID,*/
                         RawFile *file, uint32 offset, uint32 length, Common::String theName,
                         VGMSampColl *theSampColl)
    : VGMFile(theFormat, file, offset, length, theName),
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

VGMInstr::VGMInstr(VGMInstrSet *instrSet, uint32 offset, uint32 length, uint32 theBank,
                   uint32 theInstrNum, const Common::String &name)
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
