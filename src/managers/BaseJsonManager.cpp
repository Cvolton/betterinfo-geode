#include "BaseJsonManager.h"
#include "../utils.hpp"

#include <fstream>
#include <thread>

bool BaseJsonManager::init(const char* filename){
    m_filename = filename;
    auto loadResult = load();
    if(!loadResult) {
        log::error("Unable to load {} : {}", filename, loadResult.unwrapErr());
    }

    return true;
}

Result<> BaseJsonManager::load() {
    auto savedPath = Mod::get()->getSaveDir() / m_filename;
    if (ghc::filesystem::exists(savedPath)) {
        auto result = utils::file::readString(savedPath);
        if (!result) {
            return Err(result.unwrapErr());
        }

        std::string error;
        auto parsed = matjson::parse(result.unwrap(), error);
        if (!parsed.has_value()) {
            return Err("Unable to parse: {}", error);
        }
        m_json = parsed.value();

        if (!m_json.is_object()) {
            m_json = matjson::Object();
            return Err("Not an object");
        }
    }

    validateLoadedData();
    m_loaded = true;

    return Ok();
}

Result<> BaseJsonManager::save() {
    std::lock_guard<std::mutex> guard(m_jsonMutex);
    std::string savedStr = m_json.dump(matjson::NO_INDENTATION);

    auto res2 = utils::file::writeString(Mod::get()->getSaveDir() / m_filename, savedStr);
    if (!res2) {
        log::error("Unable to save values: {}", res2.unwrapErr());
    }
    return Ok();
}

void BaseJsonManager::doSave() {
    using namespace std::chrono_literals;

    static bool isSaving = false;
    static bool requestedAnotherSave = false;
    if(isSaving) {
        requestedAnotherSave = true;
        return;
    }

    isSaving = true;
    std::thread([this] {
        thread::setName("BI JSON Manager Saver");

        auto loadResult = save();
        if(!loadResult) {
            log::warn("Unable to save {}", m_filename);
        }
        std::this_thread::sleep_for(15000ms);
        isSaving = false;
        if(requestedAnotherSave) {
            requestedAnotherSave = false;
            doSave();
        }
    }).detach();
}

void BaseJsonManager::waitForLoad() {
    while(!m_loaded) {}
}

void BaseJsonManager::validateLoadedData() {
    // to be overriden by derived class
}

void BaseJsonManager::validateIsObject(const char* key) {
    if(!m_json[key].is_object()) m_json[key] = matjson::Object();
}

BaseJsonManager::BaseJsonManager(){}

bool BaseJsonManager::objectExists(const char* dict, const std::string& key) {
    return m_json[dict].as_object().find(key) != m_json[dict].as_object().end();
}