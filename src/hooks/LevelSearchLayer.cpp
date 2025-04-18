#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

#include "../utils.hpp"

using namespace geode::prelude;

class BI_DLL $modify(BILevelSearchLayer, LevelSearchLayer) {

    GJSearchObject* getSearchObject(SearchType type, gd::string gdstr) {
        std::string str = gdstr;

        std::string::const_iterator it = str.begin();
        while (it != str.end() && std::isdigit(*it)) ++it;
        bool isID = !str.empty() && it == str.end();

        if(Mod::get()->getSavedValue<bool>("search_trim")) {
            for(char c : {'\n', '\t', '\r', ' '}) {
                str.erase(0, str.find_first_not_of(c));
                str.erase(str.find_last_not_of(c) + 1);
            }
        }

        if(Mod::get()->getSavedValue<bool>("search_surround_percent") && (!isID || Mod::get()->getSavedValue<bool>("search_no_id")) && type == SearchType::Search) str = str + "%25%25";
        gdstr = str;

        return LevelSearchLayer::getSearchObject(type, gdstr);
    }

};