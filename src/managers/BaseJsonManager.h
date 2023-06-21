#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BaseJsonManager : public CCObject {
protected:
	BaseJsonManager();

public:
	std::string m_filename;
	json::Value m_json = json::Object();

	bool init(const char* filename);
	Result<> load();
	Result<> save();
	void doSave();

	virtual void validateLoadedData();
	void validateIsObject(const char* key);

	bool objectExists(const char* dict, const std::string& key);
};