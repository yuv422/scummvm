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

#include "scooby/scooby.h"
#include "engines/advancedDetector.h"
#include "common/savefile.h"
#include "common/system.h"
#include "common/translation.h"
#include "backends/keymapper/action.h"
#include "backends/keymapper/keymapper.h"
#include "backends/keymapper/standard-actions.h"
#include "base/plugins.h"
#include "graphics/thumbnail.h"

static const PlainGameDescriptor scoobyGames[] = {
		{ "scooby", "Scooby-Doo Mystery" },
		{ 0, 0 }
};

namespace Scooby {

static const ScoobyGameDescription gameDescriptions[] = {
	{
			{
					"scooby",
					0,
					AD_ENTRY1s("scooby.md", "40a323e76dfa5892c0cd534f5bc3da6f", 2097152),
					Common::EN_USA,
					Common::kPlatformPSX,
					ADGF_DROPPLATFORM,
					GUIO0()
			},
			kGameIdScooby
	},

	{ AD_TABLE_END_MARKER, 0 }
};

} // End of namespace Scooby

static const char * const directoryGlobs[] = {
	"resource",
	0
};

class ScoobyMetaEngine : public AdvancedMetaEngine {
public:
	ScoobyMetaEngine() : AdvancedMetaEngine(Scooby::gameDescriptions, sizeof(Scooby::ScoobyGameDescription), scoobyGames) {
		_maxScanDepth = 2;
		_directoryGlobs = directoryGlobs;
	}

	const char *getEngineId() const override {
		return "scooby";
	}

	virtual const char *getName() const override {
		return "Scooby-Doo Mystery";
	}

	virtual const char *getOriginalCopyright() const override {
		return "(C) 1996 The Illusions Gaming Company";
	}

	virtual bool hasFeature(MetaEngineFeature f) const override;
	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override;
	virtual int getMaximumSaveSlot() const override;
	virtual SaveStateList listSaves(const char *target) const override;
	SaveStateDescriptor querySaveMetaInfos(const char *target, int slot) const override;
	virtual void removeSaveState(const char *target, int slot) const override;
	Common::KeymapArray initKeymaps(const char *target) const override;
};

bool ScoobyMetaEngine::hasFeature(MetaEngineFeature f) const {
	return
			(f == kSupportsListSaves) ||
			(f == kSupportsDeleteSave) ||
			(f == kSupportsLoadingDuringStartup) ||
			(f == kSavesSupportMetaInfo) ||
			(f == kSavesSupportThumbnail) ||
			(f == kSavesSupportCreationDate);
}

void ScoobyMetaEngine::removeSaveState(const char *target, int slot) const {
	Common::String fileName = Common::String::format("%s.%03d", target, slot);
	g_system->getSavefileManager()->removeSavefile(fileName);
}

int ScoobyMetaEngine::getMaximumSaveSlot() const {
	return 999;
}

SaveStateList ScoobyMetaEngine::listSaves(const char *target) const {
	Common::SaveFileManager *saveFileMan = g_system->getSavefileManager();
	Scooby::SaveHeader header;
	Common::String pattern = target;
	pattern += ".???";
	Common::StringArray filenames;
	filenames = saveFileMan->listSavefiles(pattern.c_str());
	SaveStateList saveList;
	for (Common::StringArray::const_iterator file = filenames.begin(); file != filenames.end(); ++file) {
		// Obtain the last 3 digits of the filename, since they correspond to the save slot
		int slotNum = atoi(file->c_str() + file->size() - 3);
		if (slotNum >= 0 && slotNum <= 999) {
			Common::InSaveFile *in = saveFileMan->openForLoading(file->c_str());
			if (in) {
				if (Scooby::ScoobyEngine::readSaveHeader(in, header) == Scooby::kRSHENoError) {
					saveList.push_back(SaveStateDescriptor(slotNum, header.description));
				}
				delete in;
			}
		}
	}
	Common::sort(saveList.begin(), saveList.end(), SaveStateDescriptorSlotComparator());
	return saveList;
}

SaveStateDescriptor ScoobyMetaEngine::querySaveMetaInfos(const char *target, int slot) const {
	Common::String filename = Scooby::ScoobyEngine::getSavegameFilename(target, slot);
	Common::InSaveFile *in = g_system->getSavefileManager()->openForLoading(filename.c_str());
	if (in) {
		Scooby::SaveHeader header;
		Scooby::kReadSaveHeaderError error;
		error = Scooby::ScoobyEngine::readSaveHeader(in, header, false);
		delete in;
		if (error == Scooby::kRSHENoError) {
			SaveStateDescriptor desc(slot, header.description);
			// Slot 0 is used for the "Continue" save
			desc.setDeletableFlag(slot != 0);
			desc.setWriteProtectedFlag(slot == 0);
			desc.setThumbnail(header.thumbnail);
			desc.setSaveDate(header.saveDate & 0xFFFF, (header.saveDate >> 16) & 0xFF, (header.saveDate >> 24) & 0xFF);
			desc.setSaveTime((header.saveTime >> 16) & 0xFF, (header.saveTime >> 8) & 0xFF);
			desc.setPlayTime(header.playTime * 1000);
			return desc;
		}
	}
	return SaveStateDescriptor();
}

bool ScoobyMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	const Scooby::ScoobyGameDescription *gd = (const Scooby::ScoobyGameDescription *)desc;
	if (gd) {
		switch (gd->gameId) {
		case Scooby::kGameIdScooby:
			*engine = new Scooby::ScoobyEngine(syst, desc);
			break;
		default:
			error("Unknown game id");
			break;
		}
	}
	return desc != 0;
}

Common::KeymapArray ScoobyMetaEngine::initKeymaps(const char *target) const {
	using namespace Common;

	Keymap *engineKeyMap = new Keymap(Keymap::kKeymapTypeGame, "scooby", "Scooby-Doo Mystery");

	Action *act;
/*
	act = new Action("LCLK", _("Action"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionSelect);
	act->addDefaultInputMapping("MOUSE_LEFT");
	act->addDefaultInputMapping("JOY_A");
	engineKeyMap->addAction(act);

	act = new Action("CHANGECOMMAND", _("Change Command"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionChangeCommand);
	act->addDefaultInputMapping("MOUSE_RIGHT");
	act->addDefaultInputMapping("JOY_B");
	engineKeyMap->addAction(act);

	act = new Action("INVENTORY", _("Inventory"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionInventory);
	act->addDefaultInputMapping("i");
	engineKeyMap->addAction(act);

	act = new Action("ENTER", _("Enter"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionEnter);
	act->addDefaultInputMapping("RETURN");
	act->addDefaultInputMapping("KP_ENTER");
	engineKeyMap->addAction(act);

	act = new Action(kStandardActionMoveUp, _("Up"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionUp);
	act->addDefaultInputMapping("UP");
	act->addDefaultInputMapping("JOY_UP");
	engineKeyMap->addAction(act);

	act = new Action(kStandardActionMoveDown, _("Down"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionDown);
	act->addDefaultInputMapping("DOWN");
	act->addDefaultInputMapping("JOY_DOWN");
	engineKeyMap->addAction(act);

	act = new Action(kStandardActionMoveLeft, _("Left"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionLeft);
	act->addDefaultInputMapping("LEFT");
	act->addDefaultInputMapping("JOY_LEFT");
	engineKeyMap->addAction(act);

	act = new Action(kStandardActionMoveRight, _("Right"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionRight);
	act->addDefaultInputMapping("RIGHT");
	act->addDefaultInputMapping("JOY_RIGHT");
	engineKeyMap->addAction(act);

	act = new Action("SQUARE", _("Square"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionSquare);
	act->addDefaultInputMapping("a");
	act->addDefaultInputMapping("JOY_X");
	engineKeyMap->addAction(act);

	act = new Action("TRIANGLE", _("Triangle"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionTriangle);
	act->addDefaultInputMapping("w");
	act->addDefaultInputMapping("JOY_Y");
	engineKeyMap->addAction(act);

	act = new Action("CIRCLE", _("Circle"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionCircle);
	act->addDefaultInputMapping("d");
	act->addDefaultInputMapping("JOY_B");
	engineKeyMap->addAction(act);

	act = new Action("CROSS", _("Cross"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionCross);
	act->addDefaultInputMapping("s");
	act->addDefaultInputMapping("JOY_A");
	engineKeyMap->addAction(act);

	act = new Action("L1", _("Left Shoulder"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionL1);
	act->addDefaultInputMapping("o");
	act->addDefaultInputMapping("JOY_LEFT_SHOULDER");
	engineKeyMap->addAction(act);

	act = new Action("R1", _("Right Shoulder"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionR1);
	act->addDefaultInputMapping("p");
	act->addDefaultInputMapping("JOY_RIGHT_SHOULDER");
	engineKeyMap->addAction(act);

	act = new Action("DEBUGGFX", _("Debug Graphics"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionDebugGfx);
	act->addDefaultInputMapping("TAB");
	engineKeyMap->addAction(act);

	act = new Action("QUIT", _("Quit Game"));
	act->setCustomEngineActionEvent(Scooby::kScoobyActionQuit);
	act->addDefaultInputMapping("C+q");
	engineKeyMap->addAction(act);
*/
	return Keymap::arrayOf(engineKeyMap);
}

#if PLUGIN_ENABLED_DYNAMIC(SCOOBY)
	REGISTER_PLUGIN_DYNAMIC(SCOOBY, PLUGIN_TYPE_ENGINE, ScoobyMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(SCOOBY, PLUGIN_TYPE_ENGINE, ScoobyMetaEngine);
#endif
