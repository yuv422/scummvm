/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#pragma once

#include <climits>
#include <cassert>
#include <common/stream.h>
//#include <iterator>
#include "common/str.h"
//#include "mio.hpp"
//
//#include "util/common.h"
//#include "components/VGMTag.h"

class VGMFile;
class VGMItem;
class BytePattern;

class RawFile {
   public:
    virtual ~RawFile() = default;

    Common::String name() { return ""; };
    Common::String path() { return ""; };
    virtual size_t size() const = 0;
    Common::String extension() { return ""; };

    virtual Common::String GetParRawFileFullPath() const { return {}; }

    bool IsValidOffset(uint32_t ofs) { return ofs < size(); }

//    bool useLoaders() const noexcept { return m_flags & UseLoaders; }
//    void setUseLoaders(bool enable) noexcept {
//        if (enable) {
//            m_flags |= UseLoaders;
//        } else {
//            m_flags &= ~UseLoaders;
//        }
//    }
//    bool useScanners() const noexcept { return m_flags & UseScanners; }
//    void setUseScanners(bool enable) noexcept {
//        if (enable) {
//            m_flags |= UseScanners;
//        } else {
//            m_flags &= ~UseScanners;
//        }
//    }

    const char *begin() const noexcept { return data(); }
    const char *end()  { return data() + size(); }
//    std::reverse_iterator<const char *> rbegin()  {
//        return std::reverse_iterator<const char *>(end());
//    }
//    std::reverse_iterator<const char *> rend() const noexcept {
//        return std::reverse_iterator<const char *>(begin());
//    }
    virtual const char *data() const = 0;

    virtual const char &operator[](const size_t i) const = 0;
    virtual uint8_t GetByte(size_t offset) const = 0;
    virtual uint16_t GetShort(size_t offset) const = 0;
    virtual uint32_t GetWord(size_t offset) const = 0;
    virtual uint16_t GetShortBE(size_t offset) const = 0;
    virtual uint32_t GetWordBE(size_t offset) const = 0;

    uint32_t GetBytes(size_t offset, uint32_t nCount, void *pBuffer) const;
//    bool MatchBytes(const uint8_t *pattern, size_t offset, size_t nCount) const;
//    bool MatchBytePattern(const BytePattern &pattern, size_t offset) const;
//    bool SearchBytePattern(const BytePattern &pattern, uint32_t &nMatchOffset,
//                           uint32_t nSearchOffset = 0, uint32_t nSearchSize = static_cast<uint32_t>(-1)) const;

//    const std::vector<std::shared_ptr<VGMFile>> &containedVGMFiles() const noexcept {
//        return m_vgmfiles;
//    }
//    void AddContainedVGMFile(std::shared_ptr<VGMFile>);
//    void RemoveContainedVGMFile(VGMFile *);
//
//    VGMItem *GetItemFromOffset(long offset);
//    VGMFile *GetVGMFileFromOffset(long offset);

//    VGMTag tag;

   private:
//    std::vector<std::shared_ptr<VGMFile>> m_vgmfiles;
    enum ProcessFlags { UseLoaders = 1, UseScanners = 2 };
    int m_flags = UseLoaders | UseScanners;
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
