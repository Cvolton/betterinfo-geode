#pragma once

class UploadDateDelegate {
public:
	virtual void onUploadDateLoaded(int levelID, const std::string& date) = 0;
};