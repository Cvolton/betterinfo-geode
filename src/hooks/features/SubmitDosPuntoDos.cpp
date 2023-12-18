#include <Geode/Bindings.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/utils/web.hpp>
#include "../../objects/FLAlertResultHandler.h"

#include <thread>

using namespace geode::prelude;

class $modify(MenuLayer) {
    bool init() {
        if(!MenuLayer::init()) return false;

        std::thread([this]{
            std::this_thread::sleep_for(std::chrono::seconds(1));

            Loader::get()->queueInMainThread([this] {
                if(!Mod::get()->setSavedValue("submit-alert-shown", true)) FLAlertLayer::create(FLAlertResultHandler::create("submit-22-levels"), Mod::get()->getName().c_str(), "Allow BetterInfo to <cg>automatically</c> submit <cy>found</c> <cj>2.2 levels</c> to its own servers?", "No", "Yes", 300.f)->show();
            });
        }).detach();

        return true;
    }
};

class $modify(LevelCell) {
    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();

        if(m_level->m_gameVersion > 21 && Mod::get()->getSettingValue<bool>("submit-22-levels")) {
            //send a web request from geode to https://history.geometrydash.eu/api/v1/level/{m_level->m_levelID}/save

            web::AsyncWebRequest().userAgent(fmt::format("BetterInfo {}", Mod::get()->getVersion().toString())).fetch(fmt::format("https://history.geometrydash.eu/api/v1/level/{}/save", m_level->m_levelID.value())).text().then([this](const std::string& response){
                log::info("Save Level Now accepted level {} with response {}", m_level->m_levelID.value(), response);
            }).expect([](const std::string& error){
                log::warn("Saving level now failed: {}", error);
            });

            web::AsyncWebRequest()
                .userAgent("")
                .postRequest()
                .postFields(fmt::format("levelID={}&secret=Wmfd2893gb7", m_level->m_levelID.value()))
                .fetch("http://www.boomlings.com/database/downloadGJLevel22.php")
                .text()
                .then([this](const std::string& response) {
                    web::AsyncWebRequest()
                        .userAgent(fmt::format("BetterInfo {}", Mod::get()->getVersion().toString()))
                        .postRequest()
                        .postFields(fmt::format("download={}&levelID={}", response, m_level->m_levelID.value()))
                        .fetch("https://cvolton.eu/_experiments/download/")
                        .text()
                        .then([this](const std::string& response) {
                            log::info("Level data result: {}", response);
                        }).expect([](const std::string& error) {
                            log::warn("Sending level data failed: {}", error);
                        });
                }).expect([](const std::string& error) {
                    log::warn("Downloading level data failed: {}", error);
                });

        }
    }
};