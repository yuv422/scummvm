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


Scooby::ScoobyEngine::ScoobyEngine(OSystem *syst, const ADGameDescription *desc): Engine(syst) {
	_file = nullptr;
}

ScoobyEngine::~ScoobyEngine() {

}

Common::Error ScoobyEngine::run() {
	_file = new File(this);
	_gfx = new Gfx();

	if (ConfMan.hasKey("save_slot")) {
		loadGameState(ConfMan.getInt("save_slot"));
	} else {
		// Intro
		// Main menu
	}

	if (!shouldQuit()) {
//		_scene->draw();
//		_screen->updateScreen();

		gameLoop();
	}

	debug("Ok");

	delete _gfx;
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
		updateEvents();
	}
}

} // End of namespace Scooby
