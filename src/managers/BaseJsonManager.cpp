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

    log::info("Loaded {}", filename);
    m_initDone = true;

    return true;
}

Result<> BaseJsonManager::load() {
    //std::cout <<  ("Locking unique_lock load") << std::endl;
    std::unique_lock guard(m_jsonMutex);

    auto savedPath = Mod::get()->getSaveDir() / m_filename;
    if (std::filesystem::exists(savedPath)) {
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

    //std::cout <<  ("Unlocked unique_lock load") << std::endl;

    return Ok();
}

Result<> BaseJsonManager::save() {
    //std::cout <<  ("Locking shared_lock save") << std::endl;
    std::shared_lock guard(m_jsonMutex);
    std::string savedStr = m_json.dump(matjson::NO_INDENTATION);

    auto res2 = utils::file::writeString(Mod::get()->getSaveDir() / m_filename, savedStr);
    if (!res2) {
        log::error("Unable to save values: {}", res2.unwrapErr());
    }
    //std::cout <<  ("Unlocked shared_lock save") << std::endl;
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
    using namespace std::chrono_literals;

    while(!m_loaded) {
        std::this_thread::sleep_for(100ms);
    }
}

void BaseJsonManager::waitForInit() {
    using namespace std::chrono_literals;

    while(!m_initDone) {
        std::this_thread::sleep_for(100ms);
    }
}

void BaseJsonManager::validateLoadedData() {
    // to be overriden by derived class
}

void BaseJsonManager::validateIsObject(const char* key) {
    // no mutex because this is only intended to be called from funcs that already lock it
    if(!m_json[key].is_object()) m_json[key] = matjson::Object();
}

BaseJsonManager::BaseJsonManager(){}

bool BaseJsonManager::objectExists(const char* dict, const std::string& key) {
    std::shared_lock guard(m_jsonMutex);
    
    return m_json[dict].as_object().find(key) != m_json[dict].as_object().end();
}