#include <Geode/Bindings.hpp>
#include <Geode/modify/GJGameLevel.hpp>

using namespace geode::prelude;

class $modify(GJGameLevel) {
    static gd::string lengthKeyToString(int key) {
        switch (key) {
            case 0: return "Tiny";
            case 1: return "Short";
            case 2: return "Medium";
            case 3: return "Long";
            case 4: return "XL";
            case 5: return "Plat.";
            default: return fmt::format("Unk. ({})", key);
        }
    }
};