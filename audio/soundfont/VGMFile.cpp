/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common.h"
#include "VGMFile.h"

using namespace std;

VGMFile::VGMFile(FileType fileType, const Common::String &fmt, RawFile *theRawFile, uint32_t offset,
                 uint32_t length, Common::String theName)
    : VGMContainerItem(this, offset, length),
      rawfile(theRawFile),
      format(fmt),
      file_type(fileType),
      m_name(theName),
      id(-1) {}

VGMFile::~VGMFile(void) {}

// Only difference between this AddToUI and VGMItemContainer's version is that we do not add
// this as an item because we do not want the VGMFile to be itself an item in the Item View
void VGMFile::AddToUI(VGMItem *parent, void *UI_specific) {
    for (uint32_t i = 0; i < containers.size(); i++) {
        for (uint32_t j = 0; j < containers[i]->size(); j++)
            (*containers[i])[j]->AddToUI(nullptr, UI_specific);
    }
}

bool VGMFile::LoadVGMFile() {
    bool val = Load();
    if (!val)
        return false;
//    Format *fmt = GetFormat();
//    if (fmt)
//        fmt->OnNewFile(this);

    return val;
}

const Common::String *VGMFile::GetName(void) const {
    return &m_name;
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

void VGMHeader::AddPointer(uint32_t offset, uint32_t length, uint32_t destAddress, bool notNull,
                           const Common::String &name) {
    localitems.push_back(new VGMHeaderItem(this, VGMHeaderItem::HIT_POINTER, offset, length, name));
}

void VGMHeader::AddTempo(uint32_t offset, uint32_t length, const Common::String &name) {
    localitems.push_back(new VGMHeaderItem(this, VGMHeaderItem::HIT_TEMPO, offset, length, name));
}

void VGMHeader::AddSig(uint32_t offset, uint32_t length, const Common::String &name) {
    localitems.push_back(new VGMHeaderItem(this, VGMHeaderItem::HIT_SIG, offset, length, name));
}

// *************
// VGMHeaderItem
// *************

VGMHeaderItem::VGMHeaderItem(VGMHeader *hdr, HdrItemType theType, uint32_t offset, uint32_t length,
                             const Common::String &name)
    : VGMItem(hdr->vgmfile, offset, length, name, CLR_HEADER), type(theType) {}
