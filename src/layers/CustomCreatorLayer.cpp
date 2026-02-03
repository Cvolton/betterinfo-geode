#include "CustomCreatorLayer.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "LevelIDLayer.h"
#include "LevelFiltering/LevelSearchViewLayer.h"
#include "LevelFiltering/LevelCategorySearchAlert.h"
#include "../utils.hpp"
#include <Geode/ui/GeodeUI.hpp>

CustomCreatorLayer* CustomCreatorLayer::create() {
    auto ret = new CustomCreatorLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool CustomCreatorLayer::init() {
    if (!BIBaseLayer::init()) return false;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto menu = CCMenu::create();
    menu->setID("main-menu"_spr);
    menu->setZOrder(10);
    addChild(menu);

    auto buttonsMenu = CCMenu::create();
    buttonsMenu->setID("creator-buttons-menu"_spr);
    buttonsMenu->setContentSize({ 300.f, 240.f });
    buttonsMenu->setPosition({winSize.width / 2, winSize.height / 2 + 4.5f});
    buttonsMenu->setLayout(
        RowLayout::create()
            ->setGap(6.f)
            ->setGrowCrossAxis(true)
            ->setCrossAxisOverflow(false)
    );
    addChild(buttonsMenu);

    auto bottomMenu = CCMenu::create();
    bottomMenu->setID("bottom-menu"_spr);
    bottomMenu->setContentSize({winSize.width - 140.f, 65.f});
    bottomMenu->setPosition({winSize.width / 2, 25});
    bottomMenu->setLayout(
        RowLayout::create()
    );
    addChild(bottomMenu);

    auto label = CCLabelBMFont::create(Mod::get()->getName().c_str(), "goldFont.fnt");
    label->setPosition({winSize.width / 2, winSize.height - 25});
    label->setID("title-label"_spr);

    addChild(label);

    const std::vector<std::tuple<const char*, SearchType, const char*>> browserButtons = {
        { "BI_featuredBtn_001.png", SearchType::FeaturedGDW, "featured-button"_spr },
        { "BI_featuredLiteBtn_001.png", SearchType::FeaturedLite, "featured-lite-button"_spr },
        { "GJ_fameBtn_001.png", SearchType::HallOfFame, "hall-of-fame-button"_spr },
        { "GJ_bonusBtn_001.png", SearchType::Bonus, "bonus-button"_spr }
    };

    for(auto button : browserButtons) {
        auto sprite = BetterInfo::createWithBISpriteFrameName(std::get<0>(button));
        auto menuItem = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(CustomCreatorLayer::onBrowserButton)
        );
        buttonsMenu->addChild(menuItem);
        menuItem->setSizeMult(1.2f);
        menuItem->setTag((int) std::get<1>(button));
        menuItem->setID(std::get<2>(button));
    }

    auto searchIDBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createBISprite("BI_searchID_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onSearchID)
    );
    buttonsMenu->addChild(searchIDBtn);
    searchIDBtn->setSizeMult(1.2f);
    searchIDBtn->setID("search-id-button"_spr);

    auto searchBtn = CCMenuItemSpriteExtra::create(
        BetterInfo::createBISprite("BI_searchLevel_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onSearch)
    );
    buttonsMenu->addChild(searchBtn);
    searchBtn->setSizeMult(1.2f);
    searchBtn->setID("search-button"_spr);

    auto settingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSprite->setScale(0.775f);
    auto settingsBtn = CCMenuItemSpriteExtra::create(
        settingsSprite,
        this,
        menu_selector(CustomCreatorLayer::onSettings)
    );
    bottomMenu->addChild(settingsBtn);
    settingsBtn->setSizeMult(1.2f);
    settingsBtn->setID("settings-button"_spr);

    auto infoBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onInfo)
    );
    menu->addChild(infoBtn);
    infoBtn->setPosition({ (winSize.width / 2) - 25 , (winSize.height / 2) - 25 });
    infoBtn->setSizeMult(1.2f);
    infoBtn->setID("info-button"_spr);

    buttonsMenu->updateLayout();
    bottomMenu->updateLayout();

    return true;
}

void CustomCreatorLayer::onInfo(CCObject* object) {
    FLAlertLayer::create(
        nullptr, 
        Mod::get()->getName().c_str(), 
            "This is the main menu for all features related to the mod.\n"
            "\n"
            "<co>Featured (GDW):</c> Levels formerly in Geometry Dash World\n"
            "<cy>Featured (Lite):</c> Levels featured in Geometry Dash Lite\n"
            "<cr>Hall of Fame:</c> Epic levels (as seen in GD 2.1)\n"
            "<cl>Bonus:</c> Platformer levels featured in Geometry Dash Lite.\n"
            "<cj>Search (ID):</c> View comments of any level ID.\n"
            "<cg>Level Search:</c> Show a list of levels based on your progress\n"
            "\n"
            "<cp>Settings:</c> Opens the mod settings",
        "OK", 
        nullptr,
        450
    )->show();
}

void CustomCreatorLayer::onSearch(CCObject* object) {
    LevelCategorySearchAlert::create()->show();
}

void CustomCreatorLayer::onSearchID(CCObject* object) {
    LevelIDLayer::create()->show();
}

void CustomCreatorLayer::onBrowserButton(CCObject* object) {

    // need to load bonusLevelsLabel_001.png because robtop forgot to add it to gmd steam
    auto* frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    auto* textureCache = CCTextureCache::sharedTextureCache();

    const char* targetName = "bonusLevelsLabel_001.png";       // The name you want it to have
    const char* filePath = "bonusLevelsLabel_001.png"_spr;  // The actual file on disk

    // 2. Only run if the target name isn't already in the cache
    if (!frameCache->spriteFrameByName(targetName)) {

        // 3. Load the image file into a Texture
        CCTexture2D* texture = textureCache->addImage(filePath, false);
        
        if (texture) {
            // 4. Create a rectangle spanning the full size of the texture
            CCSize size = texture->getContentSize();
            CCRect rect = CCRectMake(0, 0, size.width, size.height);

            // 5. Create a new Sprite Frame using that texture
            CCSpriteFrame* newFrame = CCSpriteFrame::createWithTexture(texture, rect);

            // 6. Inject it into the Sprite Frame Cache
            frameCache->addSpriteFrame(newFrame, targetName);
        }
    }

    // continue normal operation
    auto searchObject = GJSearchObject::create((SearchType) object->getTag());
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onSettings(CCObject* object) {
    //open settings menu instead of the alert :)
    openSettingsPopup(Mod::get());
}

CCScene* CustomCreatorLayer::scene() {
    auto layer = CustomCreatorLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}
