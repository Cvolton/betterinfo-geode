#include <Geode/Bindings.hpp>
#include <Geode/Modify/GameLevelManager.hpp>

using namespace geode::prelude;

class $modify(GameLevelManager) {
    gd::string userNameForUserID(int userID) {
        auto userName = GameLevelManager::userNameForUserID(userID);
        auto userNameStd = std::string(userName);

        if(Mod::get()->getSettingValue<bool>("fix-green-users") && (userNameStd == "" || userNameStd == "-")){
            //todo: implement gdhistory functionality

            if(userID == 32471) userName = "PixelCube"; //previous dataset had an error
        }

        return userName;
    }
};