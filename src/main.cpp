#include <Geode/Geode.hpp>

using namespace geode::prelude;

void setupPageLimitBypassWindows() {
    //This patches the maximum number for SetIDPopup to INT_MAX
    // The relevant function is SetIDPopup::create (Windows) or SetIDPopup::init (Android)
    auto res = Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x1431F6), {0x68, 0xFF, 0xFF, 0xFF, 0x7F});


    //This patches the amount of characters allowed in the text input in SetIDPopup to 6 characters
    // The relevant function is SetIDPopup::init
    res = Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x14371C), {0x06});
}

void setupPageLimitBypass() {
    /*
     * SetIDPopup is one of those hyperoptimized classes where the func signature
     * differs based on which platform you're looking for
     * - the max value is a member value, however it is hardcoded in
     * create() on Windows and init() on Android. As such it is easier to deal with
     * this by patching the needed variables directly.
     */

    log::info("Setting up Page Limit Bypass");
    #ifdef GEODE_IS_WINDOWS
        log::info("Performing Windows specific loading steps");
        setupPageLimitBypassWindows();
    #endif
}

$execute {
    setupPageLimitBypass();
}