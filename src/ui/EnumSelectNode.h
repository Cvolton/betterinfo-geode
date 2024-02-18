#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

template <class Key, class DisplayType = int>
class EnumSelectNode : public CCMenu {
    std::map<Key, std::string> m_enumMap;
    std::map<Key, std::string>::iterator m_current;
    std::function<void(Key)> m_callback;
    CCMenuItemSpriteExtra* m_arrowLeft = nullptr;
    CCLabelBMFont* m_label = nullptr;
    CCMenuItemSpriteExtra* m_arrowRight = nullptr;
protected:
    bool init(std::initializer_list<std::pair<Key, std::string>> args, std::function<void(Key)> callback) {
        if(!CCMenu::init()) return false;

        this->setContentSize({256, 30});
        this->ignoreAnchorPointForPosition(false);

        auto arrowLeftSprite = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
        arrowLeftSprite->setScale(1.2f);

        m_arrowLeft = CCMenuItemSpriteExtra::create(
            arrowLeftSprite,
            this,
            menu_selector(EnumSelectNode<Key>::onPrev)
        );
        m_arrowLeft->setID("arrow-left-button"_spr);
        this->addChild(m_arrowLeft);

        m_label = CCLabelBMFont::create("Invalid Value", "bigFont.fnt");
        m_label->limitLabelWidth(200, 0.8f, 0);
        m_label->setID("mode-label"_spr);
        this->addChild(m_label);

        auto arrowRightSprite = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        arrowRightSprite->setScale(1.2f);

        m_arrowRight = CCMenuItemSpriteExtra::create(
            arrowRightSprite,
            this,
            menu_selector(EnumSelectNode<Key>::onNext)
        );
        m_arrowRight->setID("arrow-right-button"_spr);
        this->addChild(m_arrowRight);

        for(const auto& [key, value] : args) {
            m_enumMap[key] = value;
        }

        m_callback = callback;
        m_current = m_enumMap.begin();
        refreshLabel();
        positionItems();

        return true;
    }

    void positionItems() {
        m_arrowLeft->setPosition({8, getContentSize().height / 2});
        m_label->setPosition({getContentSize().width / 2, getContentSize().height / 2});
        m_arrowRight->setPosition({getContentSize().width - 8, getContentSize().height / 2});
    }

    void refreshLabel() {
        if(m_current == m_enumMap.end()) return;
        if(m_enumMap.empty()) return;

        m_label->setString(fmt::format("{}: {}", static_cast<DisplayType>(m_current->first), m_current->second).c_str());
        m_label->limitLabelWidth(200, 0.8f, 0);
    }
public:
    static EnumSelectNode* create(std::initializer_list<std::pair<Key, std::string>> values, std::function<void(Key)> callback) {
        auto ret = new EnumSelectNode();
        if(ret && ret->init(values, callback)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void onPrev(cocos2d::CCObject* sender) {
        if(m_current == m_enumMap.begin()) {
            m_current = m_enumMap.end();
        }
        m_current--;
        refreshLabel();
        m_callback(getCurrentValue());
    }

    void onNext(cocos2d::CCObject* sender) {
        m_current++;
        if(m_current == m_enumMap.end()) {
            m_current = m_enumMap.begin();
        }
        refreshLabel();
        m_callback(getCurrentValue());
    }

    Key getCurrentValue() const {
        return m_current->first;
    }

    void setCurrentValue(Key value) {
        m_current = m_enumMap.find(value);
        refreshLabel();
        m_callback(value);
    }
};