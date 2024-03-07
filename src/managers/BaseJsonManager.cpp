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
    //copied straight from https://github.com/geode-sdk/geode/blob/9cd0327766386a6c2b347d483bb6556cae6f7f48/loader/src/loader/ModImpl.cpp#L172
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
    std::thread([this] {
        Sleep(15000);
        auto loadResult = save();
        if(!loadResult) {
            log::warn("Unable to save {}", m_filename);
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