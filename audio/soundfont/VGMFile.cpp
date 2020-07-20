/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common.h"
#include "VGMFile.h"

using namespace std;

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
