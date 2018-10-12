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

#ifndef SCUMMVM_ILLUSIONS_DRAGONS_H
#define SCUMMVM_ILLUSIONS_DRAGONS_H

#include "illusions/illusions.h"


namespace Illusions {

class IllusionsEngine_Dragons : public IllusionsEngine {
public:
	IllusionsEngine_Dragons(OSystem *syst, const IllusionsGameDescription *gd);

protected:
	virtual Common::Error run();

	virtual bool hasFeature(EngineFeature f) const;

public:

	void setDefaultTextCoords();

	void loadSpecialCode(uint32 resId);

	void unloadSpecialCode(uint32 resId);

	void notifyThreadId(uint32 &threadId);

	bool testMainActorFastWalk(Control *control);

	bool testMainActorCollision(Control *control);

	Control *getObjectControl(uint32 objectId);

	Common::Point getNamedPointPosition(uint32 namedPointId);

	uint32 getPriorityFromBase(int16 priority);

	uint32 getPrevScene();

	uint32 getCurrentScene();

	bool isCursorObject(uint32 actorTypeId, uint32 objectId);

	void setCursorControlRoutine(Control *control);

	void placeCursorControl(Control *control, uint32 sequenceId);

	void setCursorControl(Control *control);

	void showCursor();

	void hideCursor();

	void startScriptThreadSimple(uint32 threadId, uint32 callingThreadId);

	uint32 startTempScriptThread(byte *scriptCodeIp, uint32 callingThreadId,
								 uint32 value8, uint32 valueC, uint32 value10);

};

} // End of namespace Illusions

#endif //SCUMMVM_ILLUSIONS_DRAGONS_H
