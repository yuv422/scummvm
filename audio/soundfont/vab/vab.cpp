/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common/debug.h"
#include "common/scummsys.h"
#include "vab.h"
#include "psxspu.h"

using namespace std;

Vab::Vab(RawFile *file, uint32 offset) : VGMInstrSet("PS1", file, offset) {}

Vab::~Vab() {}

bool Vab::GetHeaderInfo() {
    uint32 nEndOffset = GetEndOffset();
    uint32 nMaxLength = nEndOffset - dwOffset;

    if (nMaxLength < 0x20) {
        return false;
    }

    m_name = "VAB";

    VGMHeader *vabHdr = AddHeader(dwOffset, 0x20, "VAB Header");
    vabHdr->AddSimpleItem(dwOffset + 0x00, 4, "ID");
    vabHdr->AddSimpleItem(dwOffset + 0x04, 4, "Version");
    vabHdr->AddSimpleItem(dwOffset + 0x08, 4, "VAB ID");
    vabHdr->AddSimpleItem(dwOffset + 0x0c, 4, "Total Size");
    vabHdr->AddSimpleItem(dwOffset + 0x10, 2, "Reserved");
    vabHdr->AddSimpleItem(dwOffset + 0x12, 2, "Number of Programs");
    vabHdr->AddSimpleItem(dwOffset + 0x14, 2, "Number of Tones");
    vabHdr->AddSimpleItem(dwOffset + 0x16, 2, "Number of VAGs");
    vabHdr->AddSimpleItem(dwOffset + 0x18, 1, "Master Volume");
    vabHdr->AddSimpleItem(dwOffset + 0x19, 1, "Master Pan");
    vabHdr->AddSimpleItem(dwOffset + 0x1a, 1, "Bank Attributes 1");
    vabHdr->AddSimpleItem(dwOffset + 0x1b, 1, "Bank Attributes 2");
    vabHdr->AddSimpleItem(dwOffset + 0x1c, 4, "Reserved");

    GetBytes(dwOffset, 0x20, &hdr);

    return true;
}

bool Vab::GetInstrPointers() {
    uint32 nEndOffset = GetEndOffset();

    uint32 offProgs = dwOffset + 0x20;
    uint32 offToneAttrs = offProgs + (16 * 128);

    uint16 numPrograms = GetShort(dwOffset + 0x12);
    uint16 numVAGs = GetShort(dwOffset + 0x16);

    uint32 offVAGOffsets = offToneAttrs + (32 * 16 * numPrograms);

    VGMHeader *progsHdr = AddHeader(offProgs, 16 * 128, "Program Table");
    VGMHeader *toneAttrsHdr = AddHeader(offToneAttrs, 32 * 16, "Tone Attributes Table");

    if (numPrograms > 128) {
		debug("Too many programs %x, offset %x", numPrograms, dwOffset);
        return false;
    }
    if (numVAGs > 255) {
		debug("Too many VAGs %x, offset %x", numVAGs, dwOffset);
        return false;
    }

    // Load each instruments.
    //
    // Rule 1. Valid instrument pointers are not always sequentially located from 0 to (numProgs -
    // 1). Number of tones can be 0. That's an empty instrument. We need to ignore it. See Clock
    // Tower PSF for example.
    //
    // Rule 2. Do not load programs more than number of programs. Even if a program table value is
    // provided. Otherwise an out-of-order access can be caused in Tone Attributes Table. See the
    // swimming event BGM of Aitakute... ~your smiles in my heart~ for example. (github issue #115)
    uint32 numProgramsLoaded = 0;
    for (uint32 progIndex = 0; progIndex < 128 && numProgramsLoaded < numPrograms; progIndex++) {
        uint32 offCurrProg = offProgs + (progIndex * 16);
        uint32 offCurrToneAttrs = offToneAttrs + (uint32)(aInstrs.size() * 32 * 16);

        if (offCurrToneAttrs + (32 * 16) > nEndOffset) {
            break;
        }

        uint8 numTonesPerInstr = GetByte(offCurrProg);
        if (numTonesPerInstr > 32) {
           debug("Program %x contains too many tones (%d)", progIndex, numTonesPerInstr);
        } else if (numTonesPerInstr != 0) {
            VabInstr *newInstr = new VabInstr(this, offCurrToneAttrs, 0x20 * 16, 0, progIndex);
            aInstrs.push_back(newInstr);
            GetBytes(offCurrProg, 0x10, &newInstr->attr);

            VGMHeader *progHdr = progsHdr->AddHeader(offCurrProg, 0x10, "Program");
            progHdr->AddSimpleItem(offCurrProg + 0x00, 1, "Number of Tones");
            progHdr->AddSimpleItem(offCurrProg + 0x01, 1, "Volume");
            progHdr->AddSimpleItem(offCurrProg + 0x02, 1, "Priority");
            progHdr->AddSimpleItem(offCurrProg + 0x03, 1, "Mode");
            progHdr->AddSimpleItem(offCurrProg + 0x04, 1, "Pan");
            progHdr->AddSimpleItem(offCurrProg + 0x05, 1, "Reserved");
            progHdr->AddSimpleItem(offCurrProg + 0x06, 2, "Attribute");
            progHdr->AddSimpleItem(offCurrProg + 0x08, 4, "Reserved");
            progHdr->AddSimpleItem(offCurrProg + 0x0c, 4, "Reserved");

            newInstr->masterVol = GetByte(offCurrProg + 0x01);

            toneAttrsHdr->unLength = offCurrToneAttrs + (32 * 16) - offToneAttrs;

            numProgramsLoaded++;
        }
    }

    if ((offVAGOffsets + 2 * 256) <= nEndOffset) {
        char name[256];
        Common::Array<SizeOffsetPair> vagLocations;
        uint32 totalVAGSize = 0;
        VGMHeader *vagOffsetHdr = AddHeader(offVAGOffsets, 2 * 256, "VAG Pointer Table");

        uint32 vagStartOffset = GetShort(offVAGOffsets) * 8;
        vagOffsetHdr->AddSimpleItem(offVAGOffsets, 2, "VAG Size /8 #0");
        totalVAGSize = vagStartOffset;

        for (uint32 i = 0; i < numVAGs; i++) {
            uint32 vagOffset;
            uint32 vagSize;

            if (i == 0) {
                vagOffset = vagStartOffset;
                vagSize = GetShort(offVAGOffsets + (i + 1) * 2) * 8;
            } else {
                vagOffset = vagStartOffset + vagLocations[i - 1].offset + vagLocations[i - 1].size;
                vagSize = GetShort(offVAGOffsets + (i + 1) * 2) * 8;
            }

            sprintf(name, "VAG Size /8 #%u", i + 1);
            vagOffsetHdr->AddSimpleItem(offVAGOffsets + (i + 1) * 2, 2, name);

            if (vagOffset + vagSize <= nEndOffset) {
                vagLocations.push_back(SizeOffsetPair(vagOffset, vagSize));
                totalVAGSize += vagSize;
            } else {
              //TODO  L_WARN("VAG #{} at {:#x} with size {:#x}) is invalid", i + 1, vagOffset, vagSize);
            }
        }
        unLength = (offVAGOffsets + 2 * 256) - dwOffset;

        // single VAB file?
        uint32 offVAGs = offVAGOffsets + 2 * 256;
        if (dwOffset == 0 && vagLocations.size() != 0) {
            // load samples as well
            PSXSampColl *newSampColl =
                new PSXSampColl(format, this, offVAGs, totalVAGSize, vagLocations);
            if (newSampColl->LoadVGMFile()) {
                //TODO pRoot->AddVGMFile(newSampColl);

                this->sampColl = newSampColl;
            } else {
                delete newSampColl;
            }
        }
    }

    return true;
}

// ********
// VabInstr
// ********

VabInstr::VabInstr(VGMInstrSet *instrSet, uint32 offset, uint32 length, uint32 theBank,
                   uint32 theInstrNum, const Common::String &name)
    : VGMInstr(instrSet, offset, length, theBank, theInstrNum, name), masterVol(127) {}

VabInstr::~VabInstr(void) {}

bool VabInstr::LoadInstr() {
    int8_t numRgns = attr.tones;
    for (int i = 0; i < numRgns; i++) {
        VabRgn *rgn = new VabRgn(this, dwOffset + i * 0x20);
        if (!rgn->LoadRgn()) {
            delete rgn;
            return false;
        }
        aRgns.push_back(rgn);
    }
    return true;
}

// ******
// VabRgn
// ******

VabRgn::VabRgn(VabInstr *instr, uint32 offset) : VGMRgn(instr, offset) {}

bool VabRgn::LoadRgn() {
    VabInstr *instr = (VabInstr *)parInstr;
    unLength = 0x20;
    GetBytes(dwOffset, 0x20, &attr);

    AddGeneralItem(dwOffset, 1, "Priority");
    AddGeneralItem(dwOffset + 1, 1, "Mode (use reverb?)");
    AddVolume((GetByte(dwOffset + 2) * instr->masterVol) / (127.0 * 127.0), dwOffset + 2, 1);
    AddPan(GetByte(dwOffset + 3), dwOffset + 3);
    AddUnityKey(GetByte(dwOffset + 4), dwOffset + 4);
    AddGeneralItem(dwOffset + 5, 1, "Pitch Tune");
    AddKeyLow(GetByte(dwOffset + 6), dwOffset + 6);
    AddKeyHigh(GetByte(dwOffset + 7), dwOffset + 7);
    AddGeneralItem(dwOffset + 8, 1, "Vibrato Width");
    AddGeneralItem(dwOffset + 9, 1, "Vibrato Time");
    AddGeneralItem(dwOffset + 10, 1, "Portamento Width");
    AddGeneralItem(dwOffset + 11, 1, "Portamento Holding Time");
    AddGeneralItem(dwOffset + 12, 1, "Pitch Bend Min");
    AddGeneralItem(dwOffset + 13, 1, "Pitch Bend Max");
    AddGeneralItem(dwOffset + 14, 1, "Reserved");
    AddGeneralItem(dwOffset + 15, 1, "Reserved");
    AddGeneralItem(dwOffset + 16, 2, "ADSR1");
    AddGeneralItem(dwOffset + 18, 2, "ADSR2");
    AddGeneralItem(dwOffset + 20, 2, "Parent Program");
    AddSampNum(GetShort(dwOffset + 22) - 1, dwOffset + 22, 2);
    AddGeneralItem(dwOffset + 24, 2, "Reserved");
    AddGeneralItem(dwOffset + 26, 2, "Reserved");
    AddGeneralItem(dwOffset + 28, 2, "Reserved");
    AddGeneralItem(dwOffset + 30, 2, "Reserved");
    ADSR1 = attr.adsr1;
    ADSR2 = attr.adsr2;
    if ((int)sampNum < 0)
        sampNum = 0;

    if (keyLow > keyHigh) {
       //TODO L_ERROR("Low key higher than high key {} > {} (at {:#x})", keyLow, keyHigh, dwOffset);
        return false;
    }

    // gocha: AFAIK, the valid range of pitch is 0-127. It must not be negative.
    // If it exceeds 127, driver clips the value and it will become 127. (In Hokuto no Ken, at
    // least) I am not sure if the interpretation of this value depends on a driver or VAB version.
    // The following code takes the byte as signed, since it could be a typical extended
    // implementation.
    int8_t ft = (int8_t)GetByte(dwOffset + 5);
    double cents = ft * 100.0 / 128.0;
    SetFineTune((int16_t)cents);

    PSXConvADSR<VabRgn>(this, ADSR1, ADSR2, false);
    return true;
}
