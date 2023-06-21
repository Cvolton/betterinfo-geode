#pragma once

#include "../objects/BISearchObject.h"

class BISearchObjectDelegate {
public:
	virtual void onSearchObjectFinished(const BISearchObject& searchObj) = 0;
};