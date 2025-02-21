#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
using namespace geode::prelude;

class $modify(LBLayer, LevelBrowserLayer) {
	bool init(GJSearchObject * searchObj) {
		auto arr = LocalLevelManager::sharedState()->m_localLevels;
		if (searchObj->m_searchType == SearchType::MyLevels && arr -> count() > 0 && (Mod::get()->getSavedValue<bool>("pending") || Mod::get()->getSavedValue<bool>(std::to_string(EditorIDs::getID(typeinfo_cast<GJGameLevel*>(arr->objectAtIndex(0))))) == false)) {
			for (int i = 0; i < arr->count(); i++) {
				auto level = typeinfo_cast<GJGameLevel*>(arr->objectAtIndex(i));
				if (level == nullptr) continue;

				if (Mod::get()->getSavedValue<bool>(std::to_string(EditorIDs::getID(level)))) {
					if (i == 0) continue;

					auto prevLevel = typeinfo_cast<GJGameLevel*>(arr->objectAtIndex(i - 1));
					if (Mod::get()->getSavedValue<bool>(std::to_string(EditorIDs::getID(prevLevel)))) continue;

					for (int j = 0; j < arr->count(); j++) {
						auto level2 = typeinfo_cast<GJGameLevel*>(arr->objectAtIndex(j));
						if (level2 == nullptr) continue;

						if (Mod::get()->getSavedValue<bool>(std::to_string(EditorIDs::getID(level2))) == false) {
							arr->removeObject(level);
							arr->insertObject(level, arr->indexOfObject(level2));
							break;
						}
					}
				}
			}
		}
		
		Mod::get()->setSavedValue<bool>("pending", false);
		LevelBrowserLayer::init(searchObj);

		return true;
	}
};

class $modify(PLEditLevelLayer, EditLevelLayer) {

	bool init(GJGameLevel * level) {
		if (!EditLevelLayer::init(level)) return false;

		/*CREATE BUTTON*/

		if (auto menu = this->getChildByID("folder-menu")) {
			auto sprOn = CCSprite::create("pinOn.png"_spr);
			auto sprOff = CCSprite::create("pinOff.png"_spr);
			auto pinToggle = CCMenuItemToggler::create(sprOn, sprOff, this, menu_selector(PLEditLevelLayer::onPinToggle));
			pinToggle->setID("pin_toggle"_spr);
			pinToggle->setScale(1.25f);
			pinToggle->setRotation(45.f);
			pinToggle->toggle((Mod::get()->getSavedValue<bool>(std::to_string(EditorIDs::getID(m_level)))) ? false : true);
			menu->addChild(pinToggle);
			menu->updateLayout();
		}
		return true;
	}

	void onPinToggle(CCObject * sender) {
		if (auto button = typeinfo_cast<CCMenuItemToggler*>(sender)) {
			if (button->isToggled() == true) {
				Mod::get()->setSavedValue<bool>(std::to_string(EditorIDs::getID(m_level)), true);
			}
			else {
				Mod::get()->setSavedValue<bool>(std::to_string(EditorIDs::getID(m_level)), false);
			}

			Mod::get()->setSavedValue<bool>("pending", true);
		}

	}

};

class $modify(LevelCell) {
	void loadLocalLevelCell() {
		LevelCell::loadLocalLevelCell();

		if (Mod::get()->getSavedValue<bool>(std::to_string(EditorIDs::getID(m_level)))) {
			if (auto layer = typeinfo_cast<CCLayer*>(getChildByID("main-layer"))) {
				auto pinIcon = CCSprite::create("pinOn.png"_spr);
				pinIcon->setPosition({ 12, 55 });
				pinIcon->setScale(0.5f);
				pinIcon->setRotation(45.f);
				pinIcon->setID("pin_icon"_spr);
				layer->addChild(pinIcon);
				if (auto name = typeinfo_cast<CCLabelBMFont*>(layer->getChildByID("level-name"))) name->setColor({ 255, 200, 202 });
			}
		}
	}
};