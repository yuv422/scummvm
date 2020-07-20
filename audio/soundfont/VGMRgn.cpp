/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common.h"
#include "VGMRgn.h"
#include "VGMInstrSet.h"

// ******
// VGMRgn
// ******

VGMRgn::VGMRgn(VGMInstr *instr, uint32_t offset, uint32_t length, Common::String name)
    : VGMContainerItem(instr->parInstrSet, offset, length, name), parInstr(instr) {
    AddContainer<VGMRgnItem>(items);
}

VGMRgn::VGMRgn(VGMInstr *instr, uint32_t offset, uint32_t length, uint8_t theKeyLow,
               uint8_t theKeyHigh, uint8_t theVelLow, uint8_t theVelHigh, int theSampNum,
               Common::String name)
    : VGMContainerItem(instr->parInstrSet, offset, length, name),
      parInstr(instr),
      keyLow(theKeyLow),
      keyHigh(theKeyHigh),
      velLow(theVelLow),
      velHigh(theVelHigh),
      sampNum(theSampNum) {
    AddContainer<VGMRgnItem>(items);
}

VGMRgn::~VGMRgn() {
    DeleteVect<VGMRgnItem>(items);
}

void VGMRgn::SetPan(uint8_t p) {
    if (p == 127) {
        pan = 1.0;
    } else if (p == 0) {
        pan = 0;
    } else if (p == 64) {
        pan = 0.5;
    } else {
        pan = pan / 127.0;
    }
}

void VGMRgn::AddGeneralItem(uint32_t offset, uint32_t length, const Common::String &name) {
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_GENERIC, offset, length, name));
}

// assumes pan is given as 0-127 value, converts it to our double -1.0 to 1.0 format
void VGMRgn::AddPan(uint8_t p, uint32_t offset, uint32_t length) {
    SetPan(p);
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_PAN, offset, length, "Pan"));
}

void VGMRgn::AddVolume(double vol, uint32_t offset, uint32_t length) {
    volume = vol;
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_VOL, offset, length, "Volume"));
}

void VGMRgn::AddUnityKey(int8_t uk, uint32_t offset, uint32_t length) {
    this->unityKey = uk;
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_UNITYKEY, offset, length, "Unity Key"));
}

void VGMRgn::AddKeyLow(uint8_t kl, uint32_t offset, uint32_t length) {
    keyLow = kl;
    items.push_back(
        new VGMRgnItem(this, VGMRgnItem::RIT_KEYLOW, offset, length, "Note Range: Low Key"));
}

void VGMRgn::AddKeyHigh(uint8_t kh, uint32_t offset, uint32_t length) {
    keyHigh = kh;
    items.push_back(
        new VGMRgnItem(this, VGMRgnItem::RIT_KEYHIGH, offset, length, "Note Range: High Key"));
}

void VGMRgn::AddSampNum(int sn, uint32_t offset, uint32_t length) {
    sampNum = sn;
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_SAMPNUM, offset, length, "Sample Number"));
}

// **********
// VGMRgnItem
// **********

VGMRgnItem::VGMRgnItem(VGMRgn *rgn, RgnItemType theType, uint32_t offset, uint32_t length,
                       const Common::String &name)
    : VGMItem(rgn->vgmfile, offset, length, name), type(theType) {}
