#include "BaseJsonManager.h"
#include "../utils.hpp"

#include <fstream>
#include <thread>

bool BaseJsonManager::init(const char* filename){
    m_filename = filename;
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
            this->m_json = matjson::parse(data);
        } catch (std::exception& err) {
            return Err(std::string("Unable to parse cache: ") + err.what());
        }
        if (!m_json.is_object()) {
            log::warn("{} was somehow not an object, forcing it to one", m_filename);
            m_json = matjson::Object();
        }
    }

    validateLoadedData();
    m_loaded = true;

    return Ok();
}

Result<> BaseJsonManager::save() {
    std::string savedStr = m_json.dump(matjson::NO_INDENTATION);

    auto res2 = utils::file::writeString(Mod::get()->getSaveDir() / m_filename, savedStr);
    if (!res2) {
        log::error("Unable to save values: {}", res2.unwrapErr());
    }
    return Ok();
}

void BaseJsonManager::doSave() {
    std::thread([this] {
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