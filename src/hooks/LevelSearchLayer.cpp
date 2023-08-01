#include <Geode/Bindings.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(BILevelSearchLayer, LevelSearchLayer) {

    GJSearchObject* getSearchObject(SearchType type, gd::string gdstr) {
        std::string str = gdstr;

        std::string::const_iterator it = str.begin();
        while (it != str.end() && std::isdigit(*it)) ++it;
        bool isID = !str.empty() && it == str.end();

        if(Mod::get()->getSavedValue<bool>("search_trim")) {
            str.erase(0, str.find_first_not_of(' '));
            str.erase(str.find_last_not_of(' ') + 1);
        }

        //if(Mod::get()->getSavedValue<bool>("search_no_id") && isID && type == SearchType::Search) str = str + "%25";
        //if(Mod::get()->getSavedValue<bool>("search_no_id") && isID && type == SearchType::Users) str = str + "%20";

        if(Mod::get()->getSavedValue<bool>("search_surround_percent") && (!isID || Mod::get()->getSavedValue<bool>("search_no_id")) && type == SearchType::Search) str = str + "%25%25";
        
        //if(Mod::get()->getSavedValue<bool>("search_contains") && type == SearchType::Search && (!isID || Mod::get()->getSavedValue<bool>("search_no_id"))) str = "%25" + str;

        gdstr = str;

        return LevelSearchLayer::getSearchObject(type, gdstr);
    }

};