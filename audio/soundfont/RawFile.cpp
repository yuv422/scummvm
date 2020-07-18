/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common/memstream.h"
#include "RawFile.h"

//#include "LogManager.h"
//#include "components/VGMFile.h"
//#include "util/BytePattern.h"

/* RawFile */
//
///* Get the item at the specified offset */
//VGMItem *RawFile::GetItemFromOffset(long offset) {
//    for (auto file : m_vgmfiles) {
//        auto item = file->GetItemFromOffset(offset);
//        if (item) {
//            return item;
//        }
//    }
//
//    return nullptr;
//}
//
///* Get the VGMFile at the specified offset */
//VGMFile *RawFile::GetVGMFileFromOffset(long offset) {
//    for (auto file : m_vgmfiles) {
//        if (file->IsItemAtOffset(offset)) {
//            return file.get();
//        }
//    }
//
//    return nullptr;
//}
//
///* FIXME: we own the VGMFile, should use unique_ptr instead */
//void RawFile::AddContainedVGMFile(std::shared_ptr<VGMFile> vgmfile) {
//    m_vgmfiles.emplace_back(vgmfile);
//}
//
//void RawFile::RemoveContainedVGMFile(VGMFile *vgmfile) {
//    auto iter =
//        std::find_if(m_vgmfiles.begin(), m_vgmfiles.end(),
//                     [vgmfile](const std::shared_ptr<VGMFile> &p) { return p.get() == vgmfile; });
//    if (iter != m_vgmfiles.end())
//        m_vgmfiles.erase(iter);
//    else
//        L_WARN("Requested deletion for VGMFile '{}' but it was not found",
//               (*const_cast<std::string *>(vgmfile->GetName())));
//}
//
uint32_t RawFile::GetBytes(size_t offset, uint32_t nCount, void *pBuffer) const {
    memcpy(pBuffer, data() + offset, nCount);
    return nCount;
}
//
//bool RawFile::MatchBytes(const uint8_t *pattern, size_t offset, size_t nCount) const {
//    return memcmp(data() + offset, pattern, nCount) == 0;
//}
//
//bool RawFile::MatchBytePattern(const BytePattern &pattern, size_t offset) const {
//    return pattern.match(data() + offset, pattern.length());
//}
//
//bool RawFile::SearchBytePattern(const BytePattern &pattern, uint32_t &nMatchOffset,
//                                uint32_t nSearchOffset, uint32_t nSearchSize) const {
//    if (nSearchOffset >= size())
//        return false;
//
//    if ((nSearchOffset + nSearchSize) > size())
//        nSearchSize = size() - nSearchOffset;
//
//    if (nSearchSize < pattern.length())
//        return false;
//
//    for (size_t offset = nSearchOffset; offset < nSearchOffset + nSearchSize - pattern.length();
//         offset++) {
//        if (MatchBytePattern(pattern, offset)) {
//            nMatchOffset = offset;
//            return true;
//        }
//    }
//    return false;
//}

const char *MemFile::data() const {
	return (const char *)_data;
}

uint8_t MemFile::GetByte(size_t offset) const {
	_seekableReadStream->seek(offset);
	return _seekableReadStream->readByte();
}

uint16_t MemFile::GetShort(size_t offset) const {
	_seekableReadStream->seek(offset);
	return _seekableReadStream->readUint16LE();
}

uint32_t MemFile::GetWord(size_t offset) const {
	_seekableReadStream->seek(offset);
	return _seekableReadStream->readUint32LE();
}

uint16_t MemFile::GetShortBE(size_t offset) const {
	_seekableReadStream->seek(offset);
	return _seekableReadStream->readUint16BE();
}

uint32_t MemFile::GetWordBE(size_t offset) const {
	_seekableReadStream->seek(offset);
	return _seekableReadStream->readUint32BE();
}

size_t MemFile::size() const {
	return _seekableReadStream->size();
}

MemFile::~MemFile() {
	delete _seekableReadStream;
}

MemFile::MemFile(const byte *data, uint32 size) : _data(data) {
	_seekableReadStream = new Common::MemoryReadStream(data, size, DisposeAfterUse::YES);
}
