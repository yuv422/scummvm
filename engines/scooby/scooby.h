/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#ifndef SCUMMVM_SCOOBY_H
#define SCUMMVM_SCOOBY_H

#include "gui/EventRecorder.h"
#include "engines/engine.h"

namespace Scooby {

enum {
	kGameIdScooby = 1
};

struct ScoobyGameDescription {
	ADGameDescription desc;
	int gameId;
};

struct SaveHeader {
	Common::String description;
	uint32 version;
	uint32 flags;
	uint32 saveDate;
	uint32 saveTime;
	uint32 playTime;
	Graphics::Surface *thumbnail;
};

enum kReadSaveHeaderError {
	kRSHENoError = 0,
	kRSHEInvalidType = 1,
	kRSHEInvalidVersion = 2,
	kRSHEIoError = 3
};

class ScoobyEngine : public Engine {
public:
	ScoobyEngine(OSystem *syst, const ADGameDescription *desc);
	~ScoobyEngine();

//	void updateEvents();
	Common::Error run() override;

	Common::String getSavegameFilename(int num);
	static Common::String getSavegameFilename(const Common::String &target, int num);
	static kReadSaveHeaderError readSaveHeader(Common::SeekableReadStream *in, SaveHeader &header, bool skipThumbnail = true);
};

}
#endif // SCUMMVM_SCOOBY_H
