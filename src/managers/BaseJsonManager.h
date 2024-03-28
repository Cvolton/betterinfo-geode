#pragma once

#include <Geode/Geode.hpp>
#include <shared_mutex>

using namespace geode::prelude;

class BaseJsonManager {
	std::atomic_bool m_loaded = false;
	std::atomic_bool m_initDone = false;

protected:
	BaseJsonManager();

public:
	std::string m_filename;
	matjson::Value m_json = matjson::Object();
	std::shared_mutex m_jsonMutex;

	bool init(const char* filename);
	Result<> load();
	Result<> save();
	void doSave();

	void waitForLoad();
	void waitForInit();

	virtual void validateLoadedData();
	void validateIsObject(const char* key);

	bool objectExists(const char* dict, const std::string& key);
};