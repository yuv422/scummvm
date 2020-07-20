/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#pragma once

#include "common/scummsys.h"
#include "common/str.h"
#include "common/array.h"
#include "RawFile.h"

enum EventColors {
    CLR_HEADER
};

class RawFile;

//template <class T>
class VGMFile;
class VGMItem;
class VGMHeader;

class VGMItem {
   public:
    VGMItem();
    VGMItem(VGMFile *thevgmfile, uint32_t theOffset, uint32_t theLength = 0,
            const Common::String theName = "", uint8_t color = 0);
    virtual ~VGMItem(void);

    friend bool operator>(VGMItem &item1, VGMItem &item2);
    friend bool operator<=(VGMItem &item1, VGMItem &item2);
    friend bool operator<(VGMItem &item1, VGMItem &item2);
    friend bool operator>=(VGMItem &item1, VGMItem &item2);

   public:
    RawFile *GetRawFile();

   protected:
    // TODO make inline
    uint32_t GetBytes(uint32_t nIndex, uint32_t nCount, void *pBuffer);
    uint8_t GetByte(uint32_t offset);
    uint16_t GetShort(uint32_t offset);
    uint32_t GetWord(uint32_t offset);
    uint16_t GetShortBE(uint32_t offset);
    uint32_t GetWordBE(uint32_t offset);

   public:
    uint8_t color;
    VGMFile *vgmfile;
    Common::String name;
    uint32_t dwOffset;  // offset in the pDoc data buffer
    uint32_t unLength;  // num of bytes the event engulfs
};

class VGMContainerItem : public VGMItem {
   public:
    VGMContainerItem();
    VGMContainerItem(VGMFile *thevgmfile, uint32_t theOffset, uint32_t theLength = 0,
                     const Common::String theName = "", uint8_t color = CLR_HEADER);
    virtual ~VGMContainerItem(void);

    VGMHeader *AddHeader(uint32_t offset, uint32_t length, const Common::String &name = "Header");

    void AddSimpleItem(uint32_t offset, uint32_t length, const Common::String &theName);

    template <class T>
    void AddContainer(Common::Array<T *> &container) {
        containers.push_back(reinterpret_cast<Common::Array<VGMItem *> *>(&container));
    }

   public:
    Common::Array<VGMHeader *> headers;
    Common::Array<Common::Array<VGMItem *> *> containers;
    Common::Array<VGMItem *> localitems;
};

class VGMColl;

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
