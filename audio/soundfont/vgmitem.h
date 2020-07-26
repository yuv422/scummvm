/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#ifndef AUDIO_SOUNDFONT_VGMITEM_H
#define AUDIO_SOUNDFONT_VGMITEM_H

#include "common/scummsys.h"
#include "common/str.h"
#include "common/array.h"
#include "rawfile.h"
#include "synthfile.h"

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
    VGMItem(VGMFile *thevgmfile, uint32 theOffset, uint32 theLength = 0,
            const Common::String theName = "", uint8 color = 0);
    virtual ~VGMItem(void);

   public:
    RawFile *GetRawFile();

   protected:
    // TODO make inline
    uint32 GetBytes(uint32 nIndex, uint32 nCount, void *pBuffer);
    uint8 GetByte(uint32 offset);
    uint16 GetShort(uint32 offset);

   public:
    uint8 color;
    VGMFile *vgmfile;
    Common::String name;
    uint32 dwOffset;  // offset in the pDoc data buffer
    uint32 unLength;  // num of bytes the event engulfs
};

class VGMContainerItem : public VGMItem {
   public:
    VGMContainerItem();
    VGMContainerItem(VGMFile *thevgmfile, uint32 theOffset, uint32 theLength = 0,
                     const Common::String theName = "", uint8 color = CLR_HEADER);
    virtual ~VGMContainerItem(void);

    VGMHeader *AddHeader(uint32 offset, uint32 length, const Common::String &name = "Header");

    void AddSimpleItem(uint32 offset, uint32 length, const Common::String &theName);

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
	VGMFile(const Common::String &format, RawFile *theRawFile, uint32 offset, uint32 length = 0,
			Common::String theName = "VGM File");
	virtual ~VGMFile();

	bool LoadVGMFile();
	virtual bool Load() = 0;

	RawFile *GetRawFile();

	size_t size() const { return unLength; }
	Common::String name() const { return m_name; }

	uint32 GetBytes(uint32 nIndex, uint32 nCount, void *pBuffer);

	inline uint8 GetByte(uint32 offset) const { return rawfile->GetByte(offset); }
	inline uint16 GetShort(uint32 offset) const { return rawfile->GetShort(offset); }
	inline uint32 GetWord(uint32 offset) const { return rawfile->GetWord(offset); }
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
	uint32 id;
	Common::String m_name;
};

// *********
// VGMHeader
// *********

class VGMHeader : public VGMContainerItem {
public:
	VGMHeader(VGMItem *parItem, uint32 offset = 0, uint32 length = 0,
			  const Common::String &name = "Header");
	virtual ~VGMHeader();
};

class VGMInstr;
class VGMRgnItem;
class VGMSampColl;

// ******
// VGMRgn
// ******

class VGMRgn : public VGMContainerItem {
public:
	VGMRgn(VGMInstr *instr, uint32 offset, uint32 length = 0, Common::String name = "Region");
	~VGMRgn();

	virtual bool LoadRgn() { return true; }

	void AddGeneralItem(uint32 offset, uint32 length, const Common::String &name);
	void SetFineTune(int16_t relativePitchCents) { fineTune = relativePitchCents; }
	void SetPan(uint8 pan);
	void AddPan(uint8 pan, uint32 offset, uint32 length = 1);
	void AddVolume(double volume, uint32 offset, uint32 length = 1);
	void AddUnityKey(int8_t unityKey, uint32 offset, uint32 length = 1);
	void AddKeyLow(uint8 keyLow, uint32 offset, uint32 length = 1);
	void AddKeyHigh(uint8 keyHigh, uint32 offset, uint32 length = 1);
	void AddSampNum(int sampNum, uint32 offset, uint32 length = 1);

	VGMInstr *parInstr;
	uint8 keyLow;
	uint8 keyHigh;
	uint8 velLow;
	uint8 velHigh;

	int8_t unityKey;
	short fineTune;

	Loop loop;

	int sampNum;
	uint32 sampOffset; /* Offset wrt whatever collection of samples we have */
	VGMSampColl *sampCollPtr;

	double volume;        /* Percentage of full volume */
	double pan;           /* Left 0 <- 0.5 Center -> 1 Right */
	double attack_time;   /* In seconds */
	double decay_time;    /* In seconds */
	double release_time;  /* In seconds */
	double sustain_level; /* Percentage */
	double sustain_time;  /* In seconds (no positive rate!) */

	uint16 attack_transform;
	uint16 release_transform;

	Common::Array<VGMRgnItem *> items;
};

// **********
// VGMRgnItem
// **********

class VGMRgnItem : public VGMItem {
public:
	enum RgnItemType {
		RIT_GENERIC,
		RIT_UNKNOWN,
		RIT_UNITYKEY,
		RIT_FINETUNE,
		RIT_KEYLOW,
		RIT_KEYHIGH,
		RIT_VELLOW,
		RIT_VELHIGH,
		RIT_PAN,
		RIT_VOL,
		RIT_SAMPNUM
	};

	VGMRgnItem(VGMRgn *rgn, RgnItemType theType, uint32 offset, uint32 length,
			   const Common::String &name);

public:
	RgnItemType type;
};

#endif // AUDIO_SOUNDFONT_VGMITEM_H
