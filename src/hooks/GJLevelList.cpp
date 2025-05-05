#include <Geode/Geode.hpp>
#include <Geode/modify/GJLevelList.hpp>

#include "../utils.hpp"

#include <cvolton.level-id-api/include/EditorIDs.hpp>

using namespace geode::prelude;

class BI_DLL $modify(BIGJLevelList, GJLevelList) {
    void showListInfo() {
        static_assert(&GJLevelList::showListInfo, "Hook not implemented");

        std::string info = getUnpackedDescription();
        if(info.empty()) info = "(No description provided)";
        info += "\n\n";

        if(m_listID > 0) info += fmt::format("List ID: <cy>{}</c>\n", m_listID);
        if(m_listType != GJLevelType::Editor) {
            if(m_uploadDate > 0) info += fmt::format("Uploaded: <cy>{} ago</c> <cl>({})</c>\n", TimeUtils::timestampToHumanReadable(m_uploadDate), TimeUtils::timeToString(m_uploadDate));
            if(m_updateDate > 0) info += fmt::format("Updated: <cy>{} ago</c> <cl>({})</c>", TimeUtils::timestampToHumanReadable(m_updateDate), TimeUtils::timeToString(m_updateDate));
        }

        for(char c : {'\n', '\t', '\r', ' '}) {
            info.erase(0, info.find_first_not_of(c));
            info.erase(info.find_last_not_of(c) + 1);
        }

        FLAlertLayer::create(nullptr, "List Info", info.c_str(), "OK", nullptr, 380.f)->show();
    }
};