#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BaseJsonManager {
	std::atomic_bool m_loaded = false;

protected:
	BaseJsonManager();

public:
	std::string m_filename;
	matjson::Value m_json = matjson::Object();
	std::mutex m_jsonMutex;

	bool init(const char* filename);
	Result<> load();
	Result<> save();
	void doSave();
	void waitForLoad();

	virtual void validateLoadedData();
	void validateIsObject(const char* key);

	bool objectExists(const char* dict, const std::string& key);
};