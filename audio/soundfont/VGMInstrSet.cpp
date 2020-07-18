/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common.h"
#include "VGMInstrSet.h"
#include "VGMSampColl.h"
#include "VGMRgn.h"
//#include "VGMColl.h"
//#include "Root.h"

//#include <fmt/format.h>


using namespace std;

// ***********
// VGMInstrSet
// ***********

VGMInstrSet::VGMInstrSet(const Common::String &format, /*FmtID fmtID,*/
                         RawFile *file, uint32_t offset, uint32_t length, Common::String name,
                         VGMSampColl *theSampColl)
    : VGMFile(FILETYPE_INSTRSET, /*fmtID,*/ format, file, offset, length, name),
      sampColl(theSampColl) {
    AddContainer<VGMInstr>(aInstrs);
}

VGMInstrSet::~VGMInstrSet() {
    DeleteVect<VGMInstr>(aInstrs);
    delete sampColl;
}

VGMInstr *VGMInstrSet::AddInstr(uint32_t offset, uint32_t length, unsigned long bank,
                                unsigned long instrNum, const Common::String &instrName) {
    VGMInstr *instr =
        new VGMInstr(this, offset, length, bank, instrNum,
                     instrName.empty() ? Common::String::format("Instrument {}", aInstrs.size()) : instrName);
    aInstrs.push_back(instr);
    return instr;
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

    if (unLength == 0) {
        SetGuessedLength();
    }

    if (sampColl != NULL) {
        if (!sampColl->Load()) {
          //TODO  L_WARN("Failed to load VGMSampColl");
        }
    }

//    pRoot->AddVGMFile(this);
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

bool VGMInstrSet::OnSaveAsDLS(void) {
//    Common::String filepath = pRoot->UI_GetSaveFilePath(ConvertToSafeFileName(m_name), "dls");
//    if (filepath.length() != 0) {
//        return SaveAsDLS(filepath.c_str());
//    }
    return false;
}

bool VGMInstrSet::OnSaveAsSF2(void) {
//    Common::String filepath = pRoot->UI_GetSaveFilePath(ConvertToSafeFileName(m_name), "sf2");
//    if (filepath.length() != 0) {
//        return SaveAsSF2(filepath);
//    }
    return false;
}

bool VGMInstrSet::SaveAsDLS(const Common::String &filepath) {
//    DLSFile dlsfile;
//    bool dlsCreationSucceeded = false;
//
//    if (assocColls.size())
//        dlsCreationSucceeded = assocColls.front()->CreateDLSFile(dlsfile);
//    else
//        return false;
//
//    if (dlsCreationSucceeded) {
//        return dlsfile.SaveDLSFile(filepath);
//    }
    return false;
}

bool VGMInstrSet::SaveAsSF2(const Common::String &filepath) {
//    SF2File *sf2file = NULL;
//
//    if (assocColls.size())
//        sf2file = assocColls.front()->CreateSF2File();
//    else
//        return false;
//    if (sf2file != NULL) {
//        bool bResult = sf2file->SaveSF2File(filepath);
//        delete sf2file;
//        return bResult;
//    }
    return false;
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

void VGMInstr::SetBank(uint32_t bankNum) {
    bank = bankNum;
}

void VGMInstr::SetInstrNum(uint32_t theInstrNum) {
    instrNum = theInstrNum;
}

VGMRgn *VGMInstr::AddRgn(VGMRgn *rgn) {
    aRgns.push_back(rgn);
    return rgn;
}

VGMRgn *VGMInstr::AddRgn(uint32_t offset, uint32_t length, int sampNum, uint8_t keyLow,
                         uint8_t keyHigh, uint8_t velLow, uint8_t velHigh) {
    VGMRgn *newRgn = new VGMRgn(this, offset, length, keyLow, keyHigh, velLow, velHigh, sampNum);
    aRgns.push_back(newRgn);
    return newRgn;
}

bool VGMInstr::LoadInstr() {
    return true;
}
