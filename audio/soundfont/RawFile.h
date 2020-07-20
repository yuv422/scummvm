/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#pragma once

#include <climits>
#include <cassert>
#include "common/stream.h"
#include "common/str.h"

class VGMFile;
class VGMItem;

class RawFile {
   public:
    virtual ~RawFile() = default;

    virtual size_t size() const = 0;

    bool IsValidOffset(uint32_t ofs) { return ofs < size(); }

    const char *begin() const noexcept { return data(); }
    const char *end()  { return data() + size(); }
    virtual const char *data() const = 0;

    virtual const char &operator[](const size_t i) const = 0;
    virtual uint8_t GetByte(size_t offset) const = 0;
    virtual uint16_t GetShort(size_t offset) const = 0;
    virtual uint32_t GetWord(size_t offset) const = 0;
    virtual uint16_t GetShortBE(size_t offset) const = 0;
    virtual uint32_t GetWordBE(size_t offset) const = 0;

    uint32_t GetBytes(size_t offset, uint32_t nCount, void *pBuffer) const;

   private:
    enum ProcessFlags { UseLoaders = 1, UseScanners = 2 };
};

class MemFile : public RawFile {
private:
	Common::SeekableReadStream *_seekableReadStream;
	const byte *_data;

public:
	MemFile(const byte *data, uint32 size);
	~MemFile();

	const char &operator[](size_t offset) const override { return _data[offset]; } //TODO look at this warning
	const char *data() const override;

	uint8_t GetByte(size_t offset) const override;

	uint16_t GetShort(size_t offset) const override;

	uint32_t GetWord(size_t offset) const override;

	uint16_t GetShortBE(size_t offset) const override;

	uint32_t GetWordBE(size_t offset) const override;

	size_t size() const override;

};
