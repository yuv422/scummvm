/*
 * VGMTrans (c) 2002-2019
 * Licensed under the zlib license,
 * refer to the included LICENSE.txt file
 */

#include "common/memstream.h"
#include "RawFile.h"

uint32_t RawFile::GetBytes(size_t offset, uint32_t nCount, void *pBuffer) const {
    memcpy(pBuffer, data() + offset, nCount);
    return nCount;
}

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
