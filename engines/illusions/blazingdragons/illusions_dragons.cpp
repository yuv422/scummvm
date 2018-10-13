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

#include "illusions/bigfilearchive.h"
#include "illusions/resourcesystem.h"

#include "illusions/input.h"
#include "illusions_dragons.h"

namespace Illusions {

IllusionsEngine_Dragons::IllusionsEngine_Dragons(OSystem *syst, const IllusionsGameDescription *gd)
		: IllusionsEngine(syst, gd) {

}

Common::Error IllusionsEngine_Dragons::run() {

	_resReader = new BigfileArchive("bigfile.dat", this->getLanguage());

	_resSys = new ResourceSystem(this);

//	_resSys->addResourceLoader(ILLUSIONS_DRAGONS_ACT, new ActorResourceLoader);
	_input = new Input();

	while (!shouldQuit()) {
		_system->updateScreen();
		updateEvents();
	}

	debug("Ok");

	delete _input;
	return Common::kNoError;
}

bool IllusionsEngine_Dragons::hasFeature(Engine::EngineFeature f) const {
	return
			(f == kSupportsRTL) ||
			(f == kSupportsLoadingDuringRuntime) ||
			(f == kSupportsSavingDuringRuntime);
}

void IllusionsEngine_Dragons::setDefaultTextCoords() {

}

void IllusionsEngine_Dragons::loadSpecialCode(uint32 resId) {

}

void IllusionsEngine_Dragons::unloadSpecialCode(uint32 resId) {

}

void IllusionsEngine_Dragons::notifyThreadId(uint32 &threadId) {

}

bool IllusionsEngine_Dragons::testMainActorFastWalk(Control *control) {
	return false;
}

bool IllusionsEngine_Dragons::testMainActorCollision(Control *control) {
	return false;
}

Control *IllusionsEngine_Dragons::getObjectControl(uint32 objectId) {
	return nullptr;
}

Common::Point IllusionsEngine_Dragons::getNamedPointPosition(uint32 namedPointId) {
	return Common::Point();
}

uint32 IllusionsEngine_Dragons::getPriorityFromBase(int16 priority) {
	return 0;
}

uint32 IllusionsEngine_Dragons::getPrevScene() {
	return 0;
}

uint32 IllusionsEngine_Dragons::getCurrentScene() {
	return 0;
}

bool IllusionsEngine_Dragons::isCursorObject(uint32 actorTypeId, uint32 objectId) {
	return false;
}

void IllusionsEngine_Dragons::setCursorControlRoutine(Control *control) {

}

void IllusionsEngine_Dragons::placeCursorControl(Control *control, uint32 sequenceId) {

}

void IllusionsEngine_Dragons::setCursorControl(Control *control) {

}

void IllusionsEngine_Dragons::showCursor() {

}

void IllusionsEngine_Dragons::hideCursor() {

}

void IllusionsEngine_Dragons::startScriptThreadSimple(uint32 threadId, uint32 callingThreadId) {

}

uint32 IllusionsEngine_Dragons::startTempScriptThread(byte *scriptCodeIp, uint32 callingThreadId, uint32 value8,
													  uint32 valueC, uint32 value10) {
	return 0;
}

}  // End of namespace Illusions
