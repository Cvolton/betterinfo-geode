#pragma once
#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class PaginatedFLAlert : public FLAlertLayer {
    std::string m_title;
    std::vector<std::string> m_content;
    size_t m_page = 0;
protected:
    virtual bool init(const std::string& title, const std::vector<std::string>& content, CCNode* parent = nullptr, size_t page = 0);
    virtual void keyBackClicked();
public:
    static PaginatedFLAlert* create(const std::string& title, const std::vector<std::string>& content, CCNode* parent = nullptr, size_t page = 0);
    void onNext(cocos2d::CCObject* sender);
    void onPrev(cocos2d::CCObject* sender);
    void loadPage(size_t page);

    void keyDown(cocos2d::enumKeyCodes key);
};