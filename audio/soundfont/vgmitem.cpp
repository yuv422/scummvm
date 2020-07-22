/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common.h"
#include "vgmitem.h"
#include "vgminstrset.h"

using namespace std;

VGMItem::VGMItem() : color(0) {}

VGMItem::VGMItem(VGMFile *thevgmfile, uint32_t theOffset, uint32_t theLength, const Common::String theName,
                 uint8_t theColor)
    : vgmfile(thevgmfile),
      name(theName),
      dwOffset(theOffset),
      unLength(theLength),
      color(theColor) {}

VGMItem::~VGMItem(void) {}

RawFile *VGMItem::GetRawFile() {
    return vgmfile->rawfile;
}

uint32_t VGMItem::GetBytes(uint32_t nIndex, uint32_t nCount, void *pBuffer) {
    return vgmfile->GetBytes(nIndex, nCount, pBuffer);
}

uint8_t VGMItem::GetByte(uint32_t offset) {
    return vgmfile->GetByte(offset);
}

uint16_t VGMItem::GetShort(uint32_t offset) {
    return vgmfile->GetShort(offset);
}

//  ****************
//  VGMContainerItem
//  ****************

VGMContainerItem::VGMContainerItem() : VGMItem() {
    AddContainer(headers);
    AddContainer(localitems);
}

VGMContainerItem::VGMContainerItem(VGMFile *thevgmfile, uint32_t theOffset, uint32_t theLength,
                                   const Common::String theName, uint8_t color)
    : VGMItem(thevgmfile, theOffset, theLength, theName, color) {
    AddContainer(headers);
    AddContainer(localitems);
}

VGMContainerItem::~VGMContainerItem() {
    DeleteVect(headers);
    DeleteVect(localitems);
}

VGMHeader *VGMContainerItem::AddHeader(uint32_t offset, uint32_t length, const Common::String &name) {
    VGMHeader *header = new VGMHeader(this, offset, length, name);
    headers.push_back(header);
    return header;
}

void VGMContainerItem::AddSimpleItem(uint32_t offset, uint32_t length, const Common::String &name) {
    localitems.push_back(new VGMItem(this->vgmfile, offset, length, name, CLR_HEADER));
}

// *********
// VGMFile
// *********

VGMFile::VGMFile(const Common::String &fmt, RawFile *theRawFile, uint32_t offset,
				 uint32_t length, Common::String theName)
		: VGMContainerItem(this, offset, length),
		  rawfile(theRawFile),
		  format(fmt),
		  m_name(theName),
		  id(-1) {}

VGMFile::~VGMFile(void) {}

bool VGMFile::LoadVGMFile() {
	bool val = Load();
	if (!val)
		return false;

	return val;
}

// These functions are common to all VGMItems, but no reason to refer to vgmfile
// or call GetRawFile() if the item itself is a VGMFile
RawFile *VGMFile::GetRawFile() {
	return rawfile;
}

uint32_t VGMFile::GetBytes(uint32_t nIndex, uint32_t nCount, void *pBuffer) {
	// if unLength != 0, verify that we're within the bounds of the file, and truncate num read
	// bytes to end of file
	if (unLength != 0) {
		uint32_t endOff = dwOffset + unLength;
		assert(nIndex >= dwOffset && nIndex < endOff);
		if (nIndex + nCount > endOff)
			nCount = endOff - nIndex;
	}

	return rawfile->GetBytes(nIndex, nCount, pBuffer);
}

// *********
// VGMHeader
// *********

VGMHeader::VGMHeader(VGMItem *parItem, uint32_t offset, uint32_t length, const Common::String &name)
		: VGMContainerItem(parItem->vgmfile, offset, length, name) {}

VGMHeader::~VGMHeader() {}

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
