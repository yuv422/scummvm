/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */
#pragma once

#include "common/scummsys.h"
#include "common/str.h"
#include "common/array.h"

enum EventColors {
    CLR_UNKNOWN,
    CLR_UNRECOGNIZED,
    CLR_HEADER,
    CLR_MISC,
    CLR_MARKER,
    CLR_TIMESIG,
    CLR_TEMPO,
    CLR_PROGCHANGE,
    CLR_TRANSPOSE,
    CLR_PRIORITY,
    CLR_VOLUME,
    CLR_EXPRESSION,
    CLR_PAN,
    CLR_NOTEON,
    CLR_NOTEOFF,
    CLR_DURNOTE,
    CLR_TIE,
    CLR_REST,
    CLR_PITCHBEND,
    CLR_PITCHBENDRANGE,
    CLR_MODULATION,
    CLR_PORTAMENTO,
    CLR_PORTAMENTOTIME,
    CLR_CHANGESTATE,
    CLR_ADSR,
    CLR_LFO,
    CLR_REVERB,
    CLR_SUSTAIN,
    CLR_LOOP,
    CLR_LOOPFOREVER,
    CLR_TRACKEND,
};

class RawFile;

//template <class T>
class VGMFile;
class VGMItem;
class VGMHeader;

struct ItemSet {
    VGMItem *item;
    VGMItem *parent;
    const char *itemName;
};

enum ItemType { ITEMTYPE_UNDEFINED, ITEMTYPE_VGMFILE, ITEMTYPE_SEQEVENT };

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
    virtual bool IsItemAtOffset(uint32_t offset, bool includeContainer = true,
                                bool matchStartOffset = false);
    virtual VGMItem *GetItemFromOffset(uint32_t offset, bool includeContainer = true,
                                       bool matchStartOffset = false);
    virtual uint32_t GuessLength(void);
    virtual void SetGuessedLength(void);

    RawFile *GetRawFile();

    virtual Common::Array<const char *> *GetMenuItemNames() { return nullptr; }
    virtual bool CallMenuItem(VGMItem *, int ) { return false; }
    virtual Common::String GetDescription() { return name; }
    virtual ItemType GetType() const { return ITEMTYPE_UNDEFINED; }
    virtual void AddToUI(VGMItem *parent, void *UI_specific);
    virtual bool IsContainerItem() { return false; }

   protected:
    // TODO make inline
    uint32_t GetBytes(uint32_t nIndex, uint32_t nCount, void *pBuffer);
    uint8_t GetByte(uint32_t offset);
    uint16_t GetShort(uint32_t offset);
    uint32_t GetWord(uint32_t offset);
    uint16_t GetShortBE(uint32_t offset);
    uint32_t GetWordBE(uint32_t offset);
    bool IsValidOffset(uint32_t offset);

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
    virtual VGMItem *GetItemFromOffset(uint32_t offset, bool includeContainer = true,
                                       bool matchStartOffset = false);
    virtual uint32_t GuessLength(void);
    virtual void SetGuessedLength(void);
    virtual void AddToUI(VGMItem *parent, void *UI_specific);
    virtual bool IsContainerItem() { return true; }

    VGMHeader *AddHeader(uint32_t offset, uint32_t length, const Common::String &name = "Header");

    void AddItem(VGMItem *item);
    void AddSimpleItem(uint32_t offset, uint32_t length, const Common::String &theName);
    void AddUnknownItem(uint32_t offset, uint32_t length);

    template <class T>
    void AddContainer(Common::Array<T *> &container) {
        containers.push_back(reinterpret_cast<Common::Array<VGMItem *> *>(&container));
    }
    template <class T>
    bool RemoveContainer(Common::Array<T *> &container) {
        auto iter = Common::find(containers.begin(), containers.end(),
                              reinterpret_cast<Common::Array<VGMItem *> *>(&container));
        if (iter != containers.end()) {
            containers.erase(iter);
            return true;
        } else
            return false;
    }

   public:
    Common::Array<VGMHeader *> headers;
    Common::Array<Common::Array<VGMItem *> *> containers;
    Common::Array<VGMItem *> localitems;
};

class ItemPtrOffsetCmp {
   public:
    bool operator()(const VGMItem *a, const VGMItem *b) const {
        return (a->dwOffset < b->dwOffset);
    }
};
