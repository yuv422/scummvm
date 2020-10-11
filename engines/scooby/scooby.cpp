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
#include "scooby/offsets.h"

namespace Scooby {

#define SCOOBY_TICK_INTERVAL 17 // ~60fps

Scooby::ScoobyEngine::ScoobyEngine(OSystem *syst, const ADGameDescription *desc): Engine(syst) {
	_file = nullptr;
	_nextUpdatetime = 0;

	_DAT_00ff07f8 = 0;
	_SHORT_00ff07fa = 0;
	_DAT_00ff09eb_flags = 0;
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
		mainMenu();
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
	byte buf[0x8000];
	uint16 palette[64];

	_file->offsetFromStart(data_SegaLogoTilemap);
	_file->read(buf, 48 * 2);

	_gfx->loadTilemap(VDP::PlaneA, buf, 10, 11, 0xc, 4);

	uint16 size = _file->decompressBytes(data_SegaLogoTiles_packed, buf, sizeof(buf));
	debug("Size: %d", size);
	_vdp->writeVRAM(0, buf, size);

	loadPalette(data_SegaLogoPalette, palette);

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

	/* VDP: Mode Register 4
   320 pixel (40 cell) wide mode.
   Progressive mode. */
	_vdp->control_port_w(0x8c81);

	// Scooby title
	_file->offsetFromStart(data_ScoobyLogoTilemap);
	_file->read(buf, 0x28 * 0x1c * 2);
	_gfx->loadTilemap(VDP::PlaneA, buf, 0, 0, 0x28, 0x1c);

	_file->offsetFromStart(data_ScoobyLogoTiles);
	_file->read(buf, 0x20e0 * 2);
	_vdp->writeVRAM(0, buf, 0x20e0 * 2);

	loadPalette(data_ScoobyLogoPalette, palette);

	fadeFromBlack(palette);

	waitForFrames(300);

	fadeToBlack();
	_vdp->zeroVRAM(0, 0x8000);

	/* VDP: Mode Register 4
   256 pixel (32 cell) wide mode.
   Progressive mode. */
	_vdp->control_port_w(0x8c00);

	_DAT_00ff09eb_flags |= 2u;
	_DAT_00ff07f8 = 0;

	// Acclaim logo
	_file->offsetFromStart(data_AcclaimLogoBgTilemap);
	_file->read(buf, 0x20 * 0x6 * 2);
	_gfx->loadTilemap(VDP::PlaneA, buf, 0, 9, 0x20, 0x6);

	_file->offsetFromStart(data_AcclaimLogoFgTilemap);
	_file->read(buf, 0x180 * 2);
	_vdp->writeVRAM(0xe480, buf, 0x180 * 2);

	size = _file->decompressBytes(data_AcclaimLogoTiles, buf, sizeof(buf));
	_vdp->writeVRAM(0, buf, size);

	loadPalette(data_AcclaimLogoPalette, palette);

	_SHORT_00ff07fa = -0x100;
	_vdp->writeVRAMWord(0xdc00, 0);
	_vdp->writeVRAMWord(0xdc02, _SHORT_00ff07fa);

	fadeFromBlack(palette);

	//TODO this was done in vblank handler in original game
	do {
		waitForFrames(1);
		_SHORT_00ff07fa += 4;
		_vdp->writeVRAMWord(0xdc00, 0);
		_vdp->writeVRAMWord(0xdc02, _SHORT_00ff07fa);
	} while (_SHORT_00ff07fa < 0x30);

	waitForFrames(200);

	fadeToBlack();
	_vdp->zeroVRAM(0, 0x8000);

	// Sunsoft logo
	_file->offsetFromStart(data_SunSoftLogoTilemap);
	_file->read(buf, 0x1a * 0x9 * 2);
	_gfx->loadTilemap(VDP::PlaneA, buf, 3, 8, 0x1a, 0x9);

	size = _file->decompressBytes(data_SunSoftLogoTiles, buf, sizeof(buf));
	_vdp->writeVRAM(0, buf, size);
	loadPalette(data_SunSoftLogoPalette, palette);

	fadeFromBlack(palette);
	waitForFrames(300);
	fadeToBlack();
	_vdp->zeroVRAM(0, 0x8000);

	//Illusions logo animation
	size = _file->decompressBytes(data_IllusionsLogoTiles, buf, sizeof(buf));
	_vdp->writeVRAM(0, buf, size);

	_file->decompressBytes(data_IllusionsLogoAnimTilemap, buf, sizeof(buf));
	_gfx->loadTilemap(VDP::PlaneA, buf, 8, 8, 0x14, 0x5);
	loadPalette(data_IllusionsLogoPalette, palette);

	fadeFromBlack(palette);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 15; j++) {
			waitForFrames(2);
			_gfx->loadTilemap(VDP::PlaneA, buf + j * 200, 8, 8, 0x14, 0x5);
		}
	}

	_file->decompressBytes(data_IllusionsLogoFullTilemap, buf, sizeof(buf));
	_gfx->loadTilemap(VDP::PlaneA, buf, 4, 5, 0x19, 0xf);

	waitForFrames(200);
	fadeToBlack();
	_vdp->zeroVRAM(0, 0x8000);
}

void ScoobyEngine::setupInitialVdpRegisters() {
	_file->offsetFromStart(data_initialVdpRegisters);

	for (uint16 i = 0; i <= 0x12; i++) {
		uint16 command = 0x8000u | (uint16)(i << 8u) | _file->readByte();
		debug("VDP init %d %04X", i, command);
		_vdp->control_port_w(command);
	}
}

void ScoobyEngine::fadeFromBlack(const uint16 *palette) {
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

void ScoobyEngine::loadPalette(uint32 offset, uint16 *palette) {
	_file->offsetFromStart(offset);
	for (int i = 0; i < 64; i++) {
		palette[i] = _file->readUint16();
	}
}

void ScoobyEngine::mainMenu() {
	byte buf[0x10000];
	uint16 palette[0x40];

	/* VDP: Window Plane Horizontal Position
	   Draw window from HP to left edge of screen.
	   Position: 0x0 (in units of 8 pixels). */
	_vdp->control_port_w(0x9100);
	/* VDP: Window Plane Vertical Position
	   Draw window from VP to top edge of screen.
	   Position: 0x0 (in units of 8 pixels). */
	_vdp->control_port_w(0x9200);

//TODO

	_vdp->control_port_w(0x857e);
//	write_volatile_4(0xc00000,0);
//	vBlankFunctionPtr = FUN_0000a38c_mainmenu_vblank;
//	hBlankFunctionPtr = noOp_hblank;
	/* VDP: Window Plane Horizontal Position
	   Draw window from HP to left edge of screen.
	   Position: 0x0 (in units of 8 pixels). */
	_vdp->control_port_w(0x9100);
	/* VDP: Window Plane Vertical Position
	   Draw window from VP to top edge of screen.
	   Position: 0x0 (in units of 8 pixels). */
	_vdp->control_port_w(0x9200);
	/* VDP: Plane Size
	   Width - 256 pixels (32 cells)
	   Height - 256 pixels (32 cells) */
	_vdp->control_port_w(0x9000);

	uint32 size = _file->decompressBytes(0x1155a, buf, sizeof(buf));
	_vdp->writeVRAM(0, buf, size);

	_file->offsetFromStart(data_MainMenuBackgroundTilemap);
	_file->read(buf, 0x380 * 2);
	_vdp->writeVRAM(0xe000, buf, 0x380 * 2);

	loadPalette(0x16cb8, palette);
	fadeFromBlack(palette);
}

} // End of namespace Scooby
