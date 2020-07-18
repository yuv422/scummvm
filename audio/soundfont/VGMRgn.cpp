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

void VGMRgn::SetRanges(uint8_t theKeyLow, uint8_t theKeyHigh, uint8_t theVelLow,
                       uint8_t theVelHigh) {
    keyLow = theKeyLow;
    keyHigh = theKeyHigh;
    velLow = theVelLow;
    velHigh = theVelHigh;
}

void VGMRgn::SetUnityKey(int8_t theUnityKey) {
    unityKey = theUnityKey;
}

void VGMRgn::SetSampNum(size_t sampNumber) {
    sampNum = sampNumber;
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

void VGMRgn::SetLoopInfo(int theLoopStatus, uint32_t theLoopStart, uint32_t theLoopLength) {
    loop.loopStatus = theLoopStatus;
    loop.loopStart = theLoopStart;
    loop.loopLength = theLoopLength;
}

void VGMRgn::SetADSR(long attackTime, uint16_t atkTransform, long decayTime, long sustainLev,
                     uint16_t rlsTransform, long releaseTime) {
    attack_time = attackTime;
    attack_transform = atkTransform;
    decay_time = decayTime;
    sustain_level = sustainLev;
    release_transform = rlsTransform;
    release_time = releaseTime;
}

void VGMRgn::AddGeneralItem(uint32_t offset, uint32_t length, const Common::String &name) {
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_GENERIC, offset, length, name));
}

void VGMRgn::AddUnknown(uint32_t offset, uint32_t length) {
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_UNKNOWN, offset, length, "Unknown"));
}

// assumes pan is given as 0-127 value, converts it to our double -1.0 to 1.0 format
void VGMRgn::AddPan(uint8_t p, uint32_t offset, uint32_t length) {
    SetPan(p);
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_PAN, offset, length, "Pan"));
}

void VGMRgn::SetVolume(double vol) {
    volume = vol;
}

void VGMRgn::AddVolume(double vol, uint32_t offset, uint32_t length) {
    volume = vol;
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_VOL, offset, length, "Volume"));
}

void VGMRgn::AddUnityKey(int8_t uk, uint32_t offset, uint32_t length) {
    this->unityKey = uk;
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_UNITYKEY, offset, length, "Unity Key"));
}

void VGMRgn::AddFineTune(int16_t relativePitchCents, uint32_t offset, uint32_t length) {
    this->fineTune = relativePitchCents;
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_FINETUNE, offset, length, "Fine Tune"));
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

void VGMRgn::AddVelLow(uint8_t vl, uint32_t offset, uint32_t length) {
    velLow = vl;
    items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_VELLOW, offset, length, "Vel Range: Low"));
}

void VGMRgn::AddVelHigh(uint8_t vh, uint32_t offset, uint32_t length) {
    velHigh = vh;
    items.push_back(
        new VGMRgnItem(this, VGMRgnItem::RIT_VELHIGH, offset, length, "Vel Range: High"));
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

VGMItem::Icon VGMRgnItem::GetIcon() {
    return ICON_BINARY;
}
