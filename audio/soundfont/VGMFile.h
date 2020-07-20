/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#pragma once
#include "common/list.h"
#include "VGMItem.h"
#include "RawFile.h"

enum FmtID : unsigned int;
class VGMColl;
class Format;

class VGMFile : public VGMContainerItem {
   public:

   public:
    VGMFile(const Common::String &format, RawFile *theRawFile, uint32_t offset, uint32_t length = 0,
            Common::String theName = "VGM File");
    virtual ~VGMFile();

    const Common::String *GetName(void) const;

    bool LoadVGMFile();
    virtual bool Load() = 0;

    RawFile *GetRawFile();

    size_t size() const noexcept { return unLength; }
    Common::String name() const noexcept { return m_name; }

    uint32_t GetBytes(uint32_t nIndex, uint32_t nCount, void *pBuffer);

    inline uint8_t GetByte(uint32_t offset) const { return rawfile->GetByte(offset); }
    inline uint16_t GetShort(uint32_t offset) const { return rawfile->GetShort(offset); }
    inline uint32_t GetWord(uint32_t offset) const { return rawfile->GetWord(offset); }
    inline uint16_t GetShortBE(uint32_t offset) const { return rawfile->GetShortBE(offset); }
    inline uint32_t GetWordBE(uint32_t offset) const { return rawfile->GetWordBE(offset); }

    size_t GetStartOffset() { return dwOffset; }
    /*
     * For whatever reason, you can create null-length VGMItems.
     * The only safe way for now is to
     * assume maximum length
     */
    size_t GetEndOffset() { return rawfile->size(); }

    const char *data() const { return rawfile->data() + dwOffset; }

    RawFile *rawfile;

   protected:
    Common::String format;
    uint32_t id;
    Common::String m_name;
};

// *********
// VGMHeader
// *********

class VGMHeader : public VGMContainerItem {
   public:
    VGMHeader(VGMItem *parItem, uint32_t offset = 0, uint32_t length = 0,
              const Common::String &name = "Header");
    virtual ~VGMHeader();
};
