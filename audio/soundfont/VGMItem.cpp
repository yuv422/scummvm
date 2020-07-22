/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common.h"
#include "VGMItem.h"

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
