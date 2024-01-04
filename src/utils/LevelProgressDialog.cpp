#include "LevelProgressDialog.h"
#include "TimeUtils.h"

#include "../managers/BetterInfoStats.h"
#include "../layers/PaginatedFLAlert.h"

std::string LevelProgressDialog::printableProgress(std::string personalBests, int percentage){

    std::stringstream bestsStream(personalBests);
    std::string currentBest;
    std::deque<int> progresses;
    while(getline(bestsStream, currentBest, ',')){
        try {
            progresses.push_front(std::stoi(currentBest));
            //contentStream << percentage << "% ";
        }catch(...){}
    }
    std::string printable;
    //std::reverse(std::begin(progresses), std::end(progresses));
    for(auto i : progresses){
        printable = std::to_string(percentage) + "% " + printable;
        percentage -= i;
    }

    return printable;
}

void LevelProgressDialog::show(GJGameLevel* level){

    if(level == nullptr) return;


    if(level->m_levelID == -1){
        FLAlertLayer::create(nullptr, "It's a secret...",  "<cr>Roses are red</c>\n<cl>Violets are blue</c>\n<cg>Welcome to</c>\n<cy>2.2</c>", "OK", nullptr, 360)->show();
        return;
    }

    if(level->m_levelID == -2){
        FLAlertLayer::create(nullptr, "The Tower",  "The path leads to an <cr>old tower</c>. It's been left alone for <cg>years</c>, with little reason to <co>explore</c>.", "OK", nullptr, 360)->show();
        return;
    }

    std::ostringstream contentStream, secondStream;
    contentStream << "<cg>Total Attempts</c>: " << level->m_attempts
        << "\n<cl>Total Jumps</c>: " << level->m_jumps
        << "\n<cy>Clicks (best att.)</c>: " << level->m_clicks
        << "\n<cr>Time (best att.)</c>: " << TimeUtils::workingTime(level->m_attemptTime);

    if(level->m_levelType != GJLevelType::Editor){
        auto stats = BetterInfoStats::sharedState();
        //auto statsV2 = BetterInfoStatsV2::sharedState();

        if(level->isPlatformer()) {
            //TODO: better platformer formatting
            contentStream << "\n<cp>Best Time</c>: " << TimeUtils::platformerTime(level->m_bestTime)
            << "\n<co>Best Points</c>: " << level->m_bestPoints;
        } else {
            contentStream << "\n<cp>Normal</c>: " << level->m_normalPercent
            << "%\n<co>Practice</c>: " << level->m_practicePercent << "%";
            if(level->m_orbCompletion != level->m_normalPercent) contentStream << "\n<cj>Orbs</c>: " << level->m_orbCompletion << "%";
            if(level->m_newNormalPercent2 != level->m_orbCompletion) contentStream << "\n<cr>Leaderboard</c>: " << level->m_newNormalPercent2 << "%";

        }

        auto normalAttempts = stats->getAttempts(level, false);
        auto practiceAttempts = stats->getAttempts(level, true);
        //auto commonFail = statsV2->getCommonFail(level);
        secondStream << "<cp>Attempts (normal)</c>: " << normalAttempts << "\n";
        secondStream << "<co>Attempts (practice)</c>: " << practiceAttempts << "\n";
        if((normalAttempts + practiceAttempts) != level->m_attempts) secondStream << "<cy>Attempts (unknown)</c>: " << (level->m_attempts - practiceAttempts - normalAttempts) << "\n";
        secondStream << "\n";
        if(stats->getPlay(level, false) != 0) secondStream << "<cg>First played</c>: " << TimeUtils::timeToString(stats->getPlay(level, false)) << "\n";
        if(stats->getPlay(level, true) != 0) secondStream << "<cl>Last played</c>: " << TimeUtils::timeToString(stats->getPlay(level, true)) << "\n";
        if(stats->getCompletion(level, false) > 0) secondStream << "<cp>Completed</c>: " << TimeUtils::timeToString(stats->getCompletion(level, false)) << "\n";
        if(stats->getCompletion(level, true) > 0) secondStream << "<co>Completed (practice)</c>: " << TimeUtils::timeToString(stats->getCompletion(level, true)) << "\n";
        //secondStream << "<co>Common fail:</c> " << commonFail.first << "% (" << commonFail.second << "x)\n";
    }else{
        contentStream << "\n<cp>Objects</c>: " << level->m_objectCount
            << "\n<cr>In Editor</c>: " << TimeUtils::workingTime(level->m_workingTime) << "\n";
        if(level->m_workingTime2) contentStream << "<cr>Editor (C)</c>: " << TimeUtils::workingTime(level->m_workingTime2) << "\n";
        ;
    }

    std::string progresses;
    if(std::string(level->m_personalBests) != ""){
        progresses = printableProgress(level->m_personalBests, level->m_newNormalPercent2);
        contentStream << "\n\n<cy>Progresses</c>: " << progresses;
    }

    std::vector<std::string> contents;
    contents.push_back(contentStream.str());
    if(!(secondStream.str().empty())) contents.push_back(secondStream.str());

    /*float dialogWidth = 350;
    if(contentStream.str().length() > 250) dialogWidth = 400;*/
    //FLAlertLayer::create(nullptr, level->m_levelName.c_str(), "OK", nullptr, dialogWidth, contentStream.str().length() > 250, 300, contentStream.str())->m_show();
    PaginatedFLAlert::create(level->m_levelName, contents)->show();
}