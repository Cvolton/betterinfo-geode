#pragma once

#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class CvoltonAlertLayerStub : public FLAlertLayer {
public:
	CCPoint m_alertSize;
	CCMenuItemSpriteExtra* m_closeBtn;

	bool init(CCPoint contentSize, float closeBtnScale = 1, const ccColor4B& color = {0x00, 0x00, 0x00, 0x4B}, int squareNumber = 1);
	void createTitle(std::string text, float separatorScale = .75f, float usernameScale = 1);
	CCMenuItemSpriteExtra* createButton(const char* texture, CCPoint position, SEL_MenuHandler callback, float textureScale = 1.0f, float sizeMult = 1.2f);
	void onCloseThunk(CCObject* sender);
	virtual void onClose(CCObject* sender);
	void keyBackClicked();
};