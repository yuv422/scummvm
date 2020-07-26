/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common.h"
#include "vgmitem.h"
#include "vgminstrset.h"

using namespace std;

VGMItem::VGMItem() {}

VGMItem::VGMItem(VGMFile *thevgmfile, uint32 theOffset, uint32 theLength, const Common::String theName)
    : vgmfile(thevgmfile),
      name(theName),
      dwOffset(theOffset),
      unLength(theLength) {}

VGMItem::~VGMItem() {}

RawFile *VGMItem::GetRawFile() {
    return vgmfile->rawfile;
}

uint32 VGMItem::GetBytes(uint32 nIndex, uint32 nCount, void *pBuffer) {
    return vgmfile->GetBytes(nIndex, nCount, pBuffer);
}

uint8 VGMItem::GetByte(uint32 offset) {
    return vgmfile->GetByte(offset);
}

uint16 VGMItem::GetShort(uint32 offset) {
    return vgmfile->GetShort(offset);
}

//  ****************
//  VGMContainerItem
//  ****************

VGMContainerItem::VGMContainerItem() : VGMItem() {
    AddContainer(headers);
    AddContainer(localitems);
}

VGMContainerItem::VGMContainerItem(VGMFile *thevgmfile, uint32 theOffset, uint32 theLength,
                                   const Common::String theName)
    : VGMItem(thevgmfile, theOffset, theLength, theName) {
    AddContainer(headers);
    AddContainer(localitems);
}

VGMContainerItem::~VGMContainerItem() {
    DeleteVect(headers);
    DeleteVect(localitems);
}

VGMHeader *VGMContainerItem::AddHeader(uint32 offset, uint32 length, const Common::String &name) {
    VGMHeader *header = new VGMHeader(this, offset, length, name);
    headers.push_back(header);
    return header;
}

void VGMContainerItem::AddSimpleItem(uint32 offset, uint32 length, const Common::String &name) {
    localitems.push_back(new VGMItem(this->vgmfile, offset, length, name));
}

// *********
// VGMFile
// *********

VGMFile::VGMFile(const Common::String &fmt, RawFile *theRawFile, uint32 offset,
				 uint32 length, Common::String theName)
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

uint32 VGMFile::GetBytes(uint32 nIndex, uint32 nCount, void *pBuffer) {
	// if unLength != 0, verify that we're within the bounds of the file, and truncate num read
	// bytes to end of file
	if (unLength != 0) {
		uint32 endOff = dwOffset + unLength;
		assert(nIndex >= dwOffset && nIndex < endOff);
		if (nIndex + nCount > endOff)
			nCount = endOff - nIndex;
	}

	return rawfile->GetBytes(nIndex, nCount, pBuffer);
}

// *********
// VGMHeader
// *********

VGMHeader::VGMHeader(VGMItem *parItem, uint32 offset, uint32 length, const Common::String &name)
		: VGMContainerItem(parItem->vgmfile, offset, length, name) {}

VGMHeader::~VGMHeader() {}

// ******
// VGMRgn
// ******

VGMRgn::VGMRgn(VGMInstr *instr, uint32 offset, uint32 length, Common::String name)
		: VGMContainerItem(instr->parInstrSet, offset, length, name),
		keyLow(0),
		keyHigh(127),
		velLow (0),
		velHigh (127),
		unityKey(-1),
		fineTune(0),
		sampNum(0),
		sampCollPtr(nullptr),
		volume(-1),
		pan(0.5),
		attack_time(0),
		decay_time(0),
		release_time(0),
		sustain_level(-1),
		sustain_time(0),
		attack_transform(no_transform),
		release_transform(no_transform),
		parInstr(instr) {
	AddContainer<VGMRgnItem>(items);
}

VGMRgn::~VGMRgn() {
	DeleteVect<VGMRgnItem>(items);
}

void VGMRgn::SetPan(uint8 p) {
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

void VGMRgn::AddGeneralItem(uint32 offset, uint32 length, const Common::String &name) {
	items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_GENERIC, offset, length, name));
}

// assumes pan is given as 0-127 value, converts it to our double -1.0 to 1.0 format
void VGMRgn::AddPan(uint8 p, uint32 offset, uint32 length) {
	SetPan(p);
	items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_PAN, offset, length, "Pan"));
}

void VGMRgn::AddVolume(double vol, uint32 offset, uint32 length) {
	volume = vol;
	items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_VOL, offset, length, "Volume"));
}

void VGMRgn::AddUnityKey(int8_t uk, uint32 offset, uint32 length) {
	this->unityKey = uk;
	items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_UNITYKEY, offset, length, "Unity Key"));
}

void VGMRgn::AddKeyLow(uint8 kl, uint32 offset, uint32 length) {
	keyLow = kl;
	items.push_back(
			new VGMRgnItem(this, VGMRgnItem::RIT_KEYLOW, offset, length, "Note Range: Low Key"));
}

void VGMRgn::AddKeyHigh(uint8 kh, uint32 offset, uint32 length) {
	keyHigh = kh;
	items.push_back(
			new VGMRgnItem(this, VGMRgnItem::RIT_KEYHIGH, offset, length, "Note Range: High Key"));
}

void VGMRgn::AddSampNum(int sn, uint32 offset, uint32 length) {
	sampNum = sn;
	items.push_back(new VGMRgnItem(this, VGMRgnItem::RIT_SAMPNUM, offset, length, "Sample Number"));
}

// **********
// VGMRgnItem
// **********

VGMRgnItem::VGMRgnItem(VGMRgn *rgn, RgnItemType theType, uint32 offset, uint32 length,
					   const Common::String &name)
		: VGMItem(rgn->vgmfile, offset, length, name), type(theType) {}
