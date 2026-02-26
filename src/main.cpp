#include <Geode/Geode.hpp>

#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

#include <cvolton.level-id-api/include/EditorIDs.hpp>

using namespace geode::prelude;

class $modify(LevelBrowserLayer)
{
	$override bool init(GJSearchObject *object)
	{
		auto *levels = LocalLevelManager::sharedState()->m_localLevels;
		if (object->m_searchType == SearchType::MyLevels && levels->count() > 0)
		{
			auto levelsExt = CCArrayExt<GJGameLevel *>(levels);
			std::sort(levelsExt.begin(), levelsExt.end(), [](GJGameLevel *a, GJGameLevel *b)
					  { return Mod::get()->getSavedValue<bool>(std::to_string(EditorIDs::getID(a))) >
							   Mod::get()->getSavedValue<bool>(std::to_string(EditorIDs::getID(b))); });
		}

		LevelBrowserLayer::init(object);
		return true;
	}
};

class $modify(LevelPinsEditLevelLayer, EditLevelLayer)
{
	$override bool init(GJGameLevel *level)
	{
		if (!EditLevelLayer::init(level))
			return false;

		if (auto menu = this->getChildByID("folder-menu"))
		{
			auto pinToggle = CCMenuItemToggler::create(
				CCSprite::create("pinOff.png"_spr),
				CCSprite::create("pinOn.png"_spr),
				this,
				menu_selector(LevelPinsEditLevelLayer::onPinToggle));

			pinToggle->setID("pin-toggle"_spr);
			pinToggle->setScale(1.25f);
			pinToggle->setRotation(45.f);

			int levelID = EditorIDs::getID(level);
			pinToggle->toggle(Mod::get()->getSavedValue<bool>(std::to_string(levelID)));

			menu->addChild(pinToggle);
			menu->updateLayout();
		}

		return true;
	}

	void onPinToggle(CCObject *sender)
	{
		if (auto toggle = static_cast<CCMenuItemToggler *>(sender))
		{
			int levelID = EditorIDs::getID(this->m_level);
			bool toggled = !toggle->m_toggled;
			Mod::get()->setSavedValue<bool>(std::to_string(levelID), toggled);
		}
	}
};

class $modify(LevelCell)
{
	$override void loadLocalLevelCell()
	{
		LevelCell::loadLocalLevelCell();

		int levelID = EditorIDs::getID(this->m_level);

		if (Mod::get()->getSavedValue<bool>(std::to_string(levelID)))
		{
			if (auto layer = this->getChildByID("main-layer"))
			{
				auto pinSpr = CCSprite::create("pinOn.png"_spr);
				pinSpr->setPosition({12.0f, 55.0f});
				pinSpr->setScale(0.5f);
				pinSpr->setRotation(45.0f);

				pinSpr->setID("pin-icon"_spr);
				layer->addChild(pinSpr);

				if (auto name = static_cast<CCLabelBMFont *>(layer->getChildByID("level-name")))
					name->setColor(Mod::get()->getSettingValue<cocos2d::ccColor3B>("name-color"));
			}
		}
	}
};