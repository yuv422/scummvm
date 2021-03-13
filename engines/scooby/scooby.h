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
#ifndef SCOOBY_SCOOBY_H
#define SCOOBY_SCOOBY_H

#include "gui/EventRecorder.h"
#include "engines/engine.h"
#include "scooby/file.h"
#include "scooby/gfx.h"
#include "scooby/md/vdp.h"

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
private:
	File *_file;
	Gfx *_gfx;
	VDP *_vdp;

	uint32 _nextUpdatetime;

	uint16 _DAT_00ff07f8;
	int16 _SHORT_00ff07fa;
	uint8 _DAT_00ff09eb_flags;
	uint8 DAT_00ff09ed_flags;

public:
	ScoobyEngine(OSystem *syst, const ADGameDescription *desc);
	~ScoobyEngine() override;

	void updateEvents();
	Common::Error run() override;

	Common::String getSavegameFilename(int num);
	static Common::String getSavegameFilename(const Common::String &target, int num);
	static kReadSaveHeaderError readSaveHeader(Common::SeekableReadStream *in, SaveHeader &header, bool skipThumbnail = true);

	void waitForFrames(uint16 numFrames);

	void fadeFromBlack(const uint16 *palette);
	void fadeToBlack();
private:
	void gameLoop();
	uint32 calulateTimeLeft();
	void wait();

	void setupInitialVdpRegisters();
	void introSequence();
	void mainMenu();

	void loadPalette(uint32 offset, uint16 *palette);

	void FUN_00009c64(uint8 *src, uint8 *dest);
};

}
#endif // SCOOBY_SCOOBY_H
