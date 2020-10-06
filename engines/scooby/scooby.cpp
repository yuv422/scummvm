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
#include "gui/message.h"
#include "common/config-manager.h"
#include "common/keyboard.h"
#include "common/language.h"
#include "common/translation.h"
#include "engines/util.h"
#include "graphics/thumbnail.h"
#include "common/error.h"
#include "scooby/scooby.h"

namespace Scooby {

#define SCOOBY_TICK_INTERVAL 17 // ~60fps

Scooby::ScoobyEngine::ScoobyEngine(OSystem *syst, const ADGameDescription *desc): Engine(syst) {
	_file = nullptr;
	_nextUpdatetime = 0;
}

ScoobyEngine::~ScoobyEngine() {

}

Common::Error ScoobyEngine::run() {
	_file = new File(this);
	_vdp = new VDP();
	_gfx = new Gfx(_vdp);

	setupInitialVdpRegisters();

	if (ConfMan.hasKey("save_slot")) {
		loadGameState(ConfMan.getInt("save_slot"));
	} else {
		introSequence();
		// Main menu
	}

	if (!shouldQuit()) {
//		_scene->draw();
//		_screen->updateScreen();

		gameLoop();
	}

	debug("Ok");

	delete _gfx;
	delete _vdp;
	delete _file;

	return Common::kNoError;
}

Common::String ScoobyEngine::getSavegameFilename(int num) {
	return Common::String();
}

Common::String ScoobyEngine::getSavegameFilename(const Common::String &target, int num) {
	return Common::String();
}

kReadSaveHeaderError ScoobyEngine::readSaveHeader(Common::SeekableReadStream *in, SaveHeader &header, bool skipThumbnail) {
	return kRSHEIoError;
}

void ScoobyEngine::updateEvents() {
	Common::Event event;

	while (_eventMan->pollEvent(event)) {
		switch (event.type) {
		case Common::EVENT_QUIT :
			quitGame();
			break;

		default: break;
		}
	}
}

void ScoobyEngine::gameLoop() {
	while (!shouldQuit()) {
		_gfx->drawFrame();
		wait();
		updateEvents();
	}
}

uint32 ScoobyEngine::calulateTimeLeft() {
	uint32 now;

	now = _system->getMillis();

	if (_nextUpdatetime <= now) {
		_nextUpdatetime = now + SCOOBY_TICK_INTERVAL;
		return (0);
	}
	uint32 delay = _nextUpdatetime - now;
	_nextUpdatetime += SCOOBY_TICK_INTERVAL;
	return (delay);
}

void ScoobyEngine::wait() {
	_system->delayMillis(calulateTimeLeft());
}

void ScoobyEngine::waitForFrames(uint16 numFrames) {
	for (uint16 i = 0; i < numFrames && !Engine::shouldQuit(); i++) {
		wait();
		_gfx->drawFrame();
		updateEvents();
	}
}

void ScoobyEngine::introSequence() {
	//SEGA Logo
	byte buf[2000];
	uint16 palette[64];

	_file->offsetFromStart(0x23226);
	_file->read(buf, 48 * 2);

	byte *ptr = buf;
	uint16 destAddress = 0xc594;
	for (int i = 0; i < 4; i++) {
		_vdp->writeVRAM(destAddress, ptr, 0xc * 2);
		destAddress += 0x80;
		ptr += (0xc * 2);
	}

	uint16 size = _file->decompressBytes(0x23286, buf, sizeof(buf));
	debug("Size: %d", size);
	_vdp->writeVRAM(0, buf, size);

	_file->offsetFromStart(0x236a0);
	for (int i = 0; i < 64; i++) {
		palette[i] = _file->readUint16();
	}

	fadeFromBlack(palette);

	waitForFrames(0x32);

	/* shine anim on sega logo */
	for (int i = 0; i < 9; i++) {
		uint16 temp = palette[10];
		for (int j = 0; j < 8; j++) {
			palette[10 - j] = palette[10 - j - 1];
			_vdp->writePalette(10 - j, palette[10 - j]);
		}
		palette[10 - 8] = temp;
		_vdp->writePalette(10 - 8, temp);
		waitForFrames(3);
	}

	waitForFrames(100);

	fadeToBlack();
	_vdp->zeroVRAM(0, 0x8000);
}

void ScoobyEngine::setupInitialVdpRegisters() {
	_file->offsetFromStart(0x10123);

	for (uint16 i = 0; i <= 0x12; i++) {
		uint16 command = 0x8000u | (uint16)(i << 8u) | _file->readByte();
		debug("VDP init %d %04X", i, command);
		_vdp->control_port_w(command);
	}
}

void ScoobyEngine::fadeFromBlack(uint16 *palette) {
	uint16 tempPalette[64];
	memset(tempPalette, 0, sizeof(tempPalette));
	_vdp->writeCRAM(0, (byte *)tempPalette, 0x80);

	for (int fadeCounter = 0; fadeCounter < 8; fadeCounter++) {
		for (int i = 0; i < 64; i++) {
			if ((palette[i] & 0xe00u) != (tempPalette[i] & 0xe00u)) {
				tempPalette[i] += 0x200;
			}
			if ((palette[i] & 0xe0u) != (tempPalette[i] & 0xe0u)) {
				tempPalette[i] += 0x20;
			}
			if ((palette[i] & 0xeu) != (tempPalette[i] & 0xeu)) {
				tempPalette[i] += 0x2;
			}
			_vdp->writePalette(i, tempPalette[i]);
		}
		waitForFrames(1);
	}
}

void ScoobyEngine::fadeToBlack() {
	for (int fadeCounter = 0; fadeCounter < 8; fadeCounter++) {
		for (int i = 0; i < 64; i++) {
			uint16 colour = _vdp->readPaletteRecord(i);
			if ((colour & 0xe00u) != 0) {
				colour -= 0x200;
			}
			if ((colour & 0xe0u) != 0) {
				colour -= 0x20;
			}
			if ((colour & 0xeu) != 0) {
				colour -= 0x2;
			}
			_vdp->writePalette(i, colour);
		}
		waitForFrames(1);
	}
}

} // End of namespace Scooby
