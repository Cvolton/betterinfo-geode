#include "BaseJsonManager.h"
#include "../utils.hpp"

#include <fstream>

bool BaseJsonManager::init(const char* filename){
    m_filename = filename;
    log::info("Loading {}", filename);
    auto loadResult = load();
    if(!loadResult) {
        log::warn("Unable to load {}", filename);
    }

    return true;
}

Result<> BaseJsonManager::load() {
    //copied straight from https://github.com/geode-sdk/geode/blob/9cd0327766386a6c2b347d483bb6556cae6f7f48/loader/src/loader/ModImpl.cpp#L172
    auto savedPath = Mod::get()->getSaveDir() / m_filename;
    if (ghc::filesystem::exists(savedPath)) {
        GEODE_UNWRAP_INTO(auto data, utils::file::readString(savedPath));

        try {
            this->m_json = json::parse(data);
        } catch (std::exception& err) {
            return Err(std::string("Unable to parse cache: ") + err.what());
        }
        if (!m_json.is_object()) {
            log::warn("{} was somehow not an object, forcing it to one", m_filename);
            m_json = json::Object();
        }
    }

    validateLoadedData();

    return Ok();
}

Result<> BaseJsonManager::save() {
    std::string savedStr = m_json.dump();

    auto res2 = utils::file::writeString(Mod::get()->getSaveDir() / m_filename, savedStr);
    if (!res2) {
        log::error("Unable to save values: {}", res2.unwrapErr());
    }

    log::info("Saved {}", m_filename);
    return Ok();
}

void BaseJsonManager::doSave() {
    auto loadResult = save();
    if(!loadResult) {
        log::warn("Unable to load {}", m_filename);
    }
}

void BaseJsonManager::validateLoadedData() {
    // to be overriden by derived class
}

void BaseJsonManager::validateIsObject(const char* key) {
    if(!m_json[key].is_object()) m_json[key] = json::Object();
}

BaseJsonManager::BaseJsonManager(){}

bool BaseJsonManager::objectExists(const char* dict, const std::string& key) {
    return m_json[dict].as_object().find(key) != m_json[dict].as_object().end();
}