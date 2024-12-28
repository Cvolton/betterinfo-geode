# Changelog
## <cg>v4.3.3</c> (2024-12-28)
* <cg>Fixed</c> a few <cr>bugs</c> related to <co>Saved Level Filtering</c>

## <cg>v4.3.2</c> (2024-12-21)
* <cg>Added</c> <cl>Bonus Levels</c> to <co>BI menu</c>
* <cg>Removed</c> <cl>derated counter</c> from <co>Demons Info</c> on profiles (temporarily)
  * This is until I can figure out why event levels sometimes _but not always_ count towards derated
* Other bugfixes & improvements

## <cg>v4.3.1</c> (2024-12-11)
* <cg>Fixed</c> <cl>compatibility</c> with <co>AlertLayerTweaks</c>

## <cg>v4.3.0</c> (2024-12-09)
* <cg>Added</c> <cl>Featured Lite</c> (Weekly Levels) to <co>BI menu</c>
* <cg>Added</c> <cl>integration</c> with <co>Eclipse Menu</c> and <co>QOLmod</c>
* <cg>Added</c> <cl>estimated game version</c> to <co>Extended Level Info</c>
* <cg>Added</c> <cl>more info</c> for <co>sent friend requests</c> and <co>messages</c>
* <cg>Disabled</c> <cj>auto submit to leaderboards</c> for <co>classic levels</c> on <cp>mobile devices</c>
  * This is because Rob's anti-cheat marks all level completions from mobile as cheated, which means you can't appear in top 100 if you submit a score like this
* <cg>Made</c> <cl>player icons</c> in <co>chest history</c> clickable
* <cg>Made</c> <cl>Extended Level Info</c> and <cl>User Profile Info</c> details copyable
* <cg>Improved</c> <cl>GDPS support</c>
* <cg>Improved</c> the <cl>reliability</c> of <co>rated list reward notifications</c>
* <cg>Updated</c> for Geode 4.0.0 and GD 2.207
* Other bugfixes & improvements

## <cg>v4.2.13</c> (2024-11-09)
* <cg>Fixed</c> an <cr>oversight</c> that made the <cl>BI button</c> open <co>Event Levels</c> instead
This is the actual last release for GD 2.206

## <cg>v4.2.12</c> (2024-11-09)
* <cg>Added</c> proper support for <co>Event Levels</c>
This is the last release for GD 2.206

## <cg>v4.2.11</c> (2024-10-18)
* <cg>Fixed</c> <cr>incorrect popup ordering</c> in <co>Filtered Search</c>
* <cg>Removed</c> <cl>filter button</c> from <co>Favorite Lists</c> \
(it didn't work anyway)

## <cg>v4.2.10</c> (2024-09-28)
* <cg>Added</c> <cl>Hall of Fame Label</c> to <co>Hall of Fame</c>
* <cg>Fixed</c> <cr>a rare crash</c> that could happen on startup
* <cg>Fixed</c> <cl>claimable lists</c> possibly showing <cr>unrated lists</c>
* <cg>Fixed</c> <cl>claimable lists</c> sometimes showing <cr>an empty list</c> of lists
* <cg>Removed</c> <cl>unnecessary buttons</c> from <co>claimable lists</c> 

## <cg>v4.2.9</c> (2024-09-17)
* <cg>Hidden</c> <cl>Rate Limit Notifications</c> while <co>Playing</c> or <co>Editing</c> levels
* <cg>Fixed</c> <cr>a crash</c> when exiting <cl>View Leaderboard As</c> too early
* <cg>Updated</c> for changes in <co>Node IDs</c> 1.14.0

## <cg>v4.2.8</c> (2024-08-19)
* <cg>Fixed</c> <cr>a crash</c> caused by the user recklessly deleting resources
* <cg>Fixed</c> <cr>a visual glitch</c> where <cl>unfavorited lists</c> appear in <co>favorited lists tab</c>

## <cg>v4.2.7</c> (2024-08-08)
* <cg>Fixed</c> <cr>a crash</c> when <cj>Last Page Search</c> reaches infinite pages
* <cg>Fixed</c> a rare <cr>startup crash</c> caused by <cj>Level Lists</c>
* <cg>Changed</c> <cj>Creator Point Breakdown</c> so it doesn't consider Legendary levels Epic
  * This is consistent with how the search filters behave
* Other bugfixes & improvements

## <cg>v4.2.6</c> (2024-08-04)
* <cg>Fixed</c> the following <cr>issues</c> on <cp>macOS</c>
  * <cg>Fixed</c> <cr>a crash</c> when opening <cl>level filters</c>
  * <cg>Fixed</c> the game creating <cr>10 empty created lists</c> on every startup

## <cg>v4.2.5</c> (2024-07-24)
* <cg>Fixed</c> <cr>a crash</c> when opening <co>Extended Level Info</c> on <cp>M1 macOS</c>

## <cg>v4.2.4</c> (2024-07-14)
* <cg>Fixed</c> <cr>incorrect orb counts</c> in <co>Daily History</c>
* <cg>Enabled</c> <cp>macOS</c> support
  * Experimental, may not get all future updates

## <cg>v4.2.3</c> (2024-07-11)
* <cg>Added</c> <cl>Unverified Coins</c> filter to <co>Filtered Search</c>
* <cg>Fixed</c> <cl>No Coins</c> filter in <co>Filtered Search</c>
* <cg>Fixed</c> <cl>Followed List</c> granting <co>Friends Achievements</c>
* <cg>Fixed</c> <cl>Icon Glow</c> in <co>Followed List</c>
* Other bugfixes & improvements

## <cg>v4.2.1 / v4.2.2</c> (2024-07-08)
* <cg>Adjusted</c> internal rate-limits for BetterInfo caching
* Other bugfixes & improvements

## <cg>v4.2.0</c> (2024-07-07)
* <cg>Added</c> <cl>list folders</c>
* <cg>Added</c> <cl>derated counter</c> to <co>Demons Info</c> on profiles
* <cg>Added</c> <cl>notification</c> when you can claim a <co>rated list reward</c>
* <cg>Added</c> <cl>followed user list</c> to <co>friends menu</c>
* <cg>Added</c> more <cl>Level Progress</c> information to <co>Editor levels</c>
  * "Editor Level IDs API" mod is now required by BI to allow this
* <cg>Added</c> <cl>Coin Count</c> and <cl>Folder</c> filters to <cj>Filtered Search</c>
* <cg>Added</c> <cl>Show Level IDs</c> option to the <cj>Mod Settings</c>
* <cg>Fixed</c> issues caused by <cl>No Transition</c> in mod menus
* <cg>Fixed</c> various <cr>stability issues</c>
* Other bugfixes & improvements

From previous alpha/beta releases:
* <cg>Updated</c> for Geode 3.0.0 and GD 2.206
  * Mac support is temporarily disabled

Also the changelogs of pre-Geode versions of BetterInfo have been added for your reading pleasure.

[See v4.2.0 Beta changelogs here](https://github.com/Cvolton/betterinfo-geode/blob/v4.2.0-beta.3/changelog.md)

[See v4.2.0 Alpha changelogs here](https://github.com/Cvolton/betterinfo-geode/blob/0b4fc2c76d6f81faaf1c939b8964515bffa15258/changelog.md)

## <cg>v4.1.13</c> (2024-05-25)
* <cg>Fixed</c> the <cl>fix</c> of the <co>fix</c>

## <cg>v4.1.12</c> (2024-05-25)
* <cg>Fixed</c> the fix i dont even care anymore

## <cg>v4.1.11</c> (2024-05-25)
* <cg>Fixed</c> another <cr>crash</c> only 2 people ever got

## <cg>v4.1.10</c> (2024-04-30)
* <cg>Fixed</c> a <cr>crash</c> only 2 people ever got
* <cg>Fixed</c> <cj>random button</c> sometimes jumping to an <co>empty page</c>

## <cg>v4.1.9</c> (2024-04-21)
* <cg>Fixed</c> <cr>random crashes</c> caused by conflicts with other mods
* <cg>Added</c> more <cj>skibidi rizz</c> (requested by Kynex7510)

## <cg>v4.1.8</c> (2024-04-15)
* <cj>Jump to Last Page</c> button is now <cp>pink</c> if it opens <co>Last Page Search</c>
* <cg>Made</c> the <cj>Filters popup</c> in <cj>Filtered Search</c> open immediately
* <cg>Made</c> the <cj>Last Page Search</c> start immediately
* <cg>Fixed</c> <cj>Filtered Search</c> button missing from <co>user level search</c>
* <cg>Fixed</c> <cj>completion times</c> being shown in <cr>UTC</c>
* <cg>Exported</c> <cj>function symbols</c> (experimental, for developers)
* Other bugfixes & improvements

## <cg>v4.1.7</c> (2024-03-28)
* <cg>Fixed</c> a few other obscure <cr>crashes</c>
* <cg>Made</c> the mod <cj>early-load</c> to make cache load faster
* Other bugfixes & improvements

## <cg>v4.1.6</c> (2024-03-23)
* <cg>Fixed</c> a few obscure <cr>crashes</c>
* Other bugfixes & improvements

## <cg>v4.1.5</c> (2024-03-16)
* <cg>Fixed</c> some filters not working properly in <cj>Filtered Search</c>

## <cg>v4.1.4</c> (2024-03-16)
* <cg>Fixed</c> potential <co>startup</c> <cr>crash and lag issues</c>
* <cg>Optimized</c> <cj>Filtered Search</c>
* Other bugfixes & improvements

## <cg>v4.1.3</c> (2024-03-08)
* <cg>Added</c> <cj>"Hide Platformer Time"</c> <co>Settings Option</c>
* <cg>Fixed</c> several <cp>Android-only</c> <cr>crashes</c>
* <cg>Fixed</c> <cj>Completed with Coins</c> <cr>only</c> working for <co>Saved Levels</c>
* <cg>Improved</c> the <cj>accuracy</c> of <co>Exact Level Length</c>
* Other bugfixes & improvements

## <cg>v4.1.2</c> (2024-03-01)
* <cg>Added</c> <cj>better descriptions</c> for <co>Cloudflare errors</c>
  * This shows how much time is left when you're rate limited for example
* <cg>Fixed</c> <cr>several issues</c> related to <co>Filtered Search</c>
* Other bugfixes & improvements

## <cg>v4.1.1</c> (2024-02-28)
* <cg>Fixed</c> a <cr>rare crash</c> in <cj>Creator Point Breakdown</c>
* Other bugfixes & improvements

## <cg>v4.1.0</c> (2024-02-28)
This update is largely based on [your suggestions](https://twitter.com/Misabr0penguin/status/1757481064254034421)!
* <cg>Added</c> <cj>Creator Point Breakdown</c> to <co>Profiles</c>
* <cg>Added</c> <cj>Queued Quests view</c> and <cj>Quests History</c>
* <cg>Added</c> <cj>"Found in"</c> to <co>Icon Info</c>
* <cg>Added</c> <cj>Game Version filter</c> to <co>Filtered Search</c>
* <cg>Added</c> <cj>Jump to Last Page</c> for <co>Comments</c>
* <cg>Added</c> <cj>Favorite</c> <co>level search</c>
* <cg>Improved</c> <cj>Chest Reward History</c>
* Other bugfixes & improvements

**For texture pack creators:** This update changes the alignment of some textures. Texture packs modifying BetterInfo's textures may need to be updated to avoid minor visual glitches.

## <cg>v4.0.0</c> (2024-02-12)
* Fixed demon difficulty filtering in Filtered Search
* Other bugfixes & improvements

## <cg>v4.0.0-alpha.13</c> (2024-02-04)
* Added Mac support
* Fixed Extended Level Stats not working properly for old daily levels
* Other bugfixes & improvements

## <cg>v4.0.0-alpha.12</c> (2024-02-04)
* Added a temporary bugfix for a Geode Loader bug

## <cg>v4.0.0-alpha.11</c> (2024-02-03)
* Minor bugfixes & improvements

## <cg>v4.0.0-alpha.10</c> (2024-02-02)
* Fixed Legendary & Mythic filters in BI Filtered Search
* Other bugfixes & improvements

## <cg>v4.0.0-alpha.9</c> (2024-01-24)
* Updated for changes in Geode v2.0.0-beta.8

## <cg>v4.0.0-alpha.8</c> (2024-01-24)
* Bugfixes & improvements

## <cg>v4.0.0-alpha.7</c> (2024-01-23)
* Fixed remaining Android-specific issues

## <cg>v4.0.0-alpha.6</c> (2024-01-22)
* Added Android GD 2.205 support

## <cg>v4.0.0-alpha.5</c> (2024-01-21)
* Further bugfixes

## <cg>v4.0.0-alpha.4</c> (2024-01-21)
* Fixed even more crashing issues

## <cg>v4.0.0-alpha.3</c> (2024-01-21)
* Fixed more crashing issues
* Added exact upload dates to List Info
* Re-added BI's Level Progress dialog to Editor Levels

## <cg>v4.0.0-alpha.2</c> (2024-01-20)
* Fixed most crashing issues
* Re-added page limit bypass

## <cg>v4.0.0-alpha.1</c> (2024-01-19)
* Added 2.204 support
* Added info buttons to The Tower levels
* Added StarsInfoPopup (click Star on your profile to see)
* Added icon unlock info to the leaderboards

### Missing features
* Page limit bypass
* Song Browser lag fix

# BetterInfo for Geode (GD 2.1)
## <cg>v3.1.1</c> (2023-12-21)
This is the last update for 2.1. It changes defaults to better align with what people would want on private servers, since it is assumed that most people on real GD would be on 2.2 by now.

* Removed 2.2 level submission
* Disabled green username restoration by default
* Enabled View Leaderboard As for GDPS (Windows Only)

## <cg>v3.1.0</c> (2023-12-19)
* Added saved songs button to level search filters
* Added Platformer length marking
* Added 2.2 level submission
* Minor improvements to BI list layers
* Fixed corners
* Fixed level ID positioning for NA levels
* Fixed Song Browser lag (RobTop bug)
* Internal structure changes, bugfixes & improvements
(if you're a mod developer who depends on BetterInfo's node IDs, this update changes all of them, so make sure to update your mods accordingly)

## <cg>v3.0.13</c> (2023-12-14)
* Added Account ID search to "ID Search" in BetterInfo Menu
* Settings button in BI Menu now actually opens settings

## <cg>v3.0.12</c> (2023-12-09)
* Fixed misaligned mod badges on profiles
* Fixed level names in Daily History
* Other bugfixes & improvements

## <cg>v3.0.11</c> (2023-12-07)
* Added demon difficulty to Weekly History
* Added highlighting to View Leaderboard As
* Added refresh button to View Leaderboard As
* Fixed most blank cells in View Leaderboard As (fix backported from 2.2)
* Fixed Jump to page on Daily and Reward Histories
* Hidden 2.2 profile info on your own profile (it's always default)
* Other bugfixes & improvements

## <cg>v3.0.10</c> (2023-11-19)
* Added 2.2 profile info to user profiles (press 'i' button to show)

## <cg>v3.0.9</c> (2023-10-17)
* Fixed a bug causing coins not to get submitted to leaderboards for old daily levels
* Fixed chest icons in chest history
* Fixed certain platform-specific issues on Windows, Mac and Android

## <cg>v3.0.8</c> (2023-10-06)
* Initial Android release
* Bugfixes & Improvements

## <cg>v3.0.7</c> (2023-09-19)
* Bugfixes & Improvements

## <cg>v3.0.6</c> (2023-09-19)
* Added level length to Level Info Layer (online level play screen)

## <cg>v3.0.5</c> (2023-09-14)
* Fixes a crash when opening Extended Level Info on certain levels

## <cg>v3.0.4</c> (2023-09-12)
Extended Level Info:
* Added exact upload dates
* Added caching of relative upload dates
* Improved loading performance

View Leaderboard As:
* Enabled on Mac
* Added loading circle

## <cg>v3.0.3</c> (2023-08-25)
* Initial Mac release ("view leaderboard as" is missing in the Mac version)
* Minor bugfixes & improvements

## <cg>v3.0.2</c> (2023-08-19)
* Fixed a bug causing normal mode percentages to appear negative
* Fixed a crash caused by using filtered level search

## <cg>v3.0.1</c> (2023-08-04)
* Fixed a crashing bug

## <cg>v3.0.0</c> (2023-08-01)
* Initial release based on the v2.4.7 feature set

### Changes from v2.4.7
* Green usernames are now loaded from GDHistory directly

# BetterInfo Standalone
## <cg>v2.4.10</c> (2023-12-09)
- <cg>Added</c> <cj>demon difficulty</c> to <co>Weekly History</c>
- <cg>Added</c> <cj>highlighting</c> to <co>View Leaderboard As</c>
- <cg>Fixed</c> <cj>level names</c> in <co>Daily History</c>
- <cg>Improved</c> <co>GDPS support</c>
- Other bugfixes & improvements

Remember how 2.2 was supposed to be in October...
Sorry, I mean November...
    
## <cg>v2.4.9</c> (2023-10-18)
- <cg>Fixed</c> <co>chest history</c> <cj>icons</c>
- <cg>Fixed</c> <cj>submitting coins</c> to old <co>daily levels leaderboards</c>

BetterInfo is now also available on Geode with a few more minor features added in.

Thank you all for using BI, see you in 2.2 :) --Cvolton
    
## <cg>v2.4.8</c> (2023-08-19)
- Fixed a potential crash caused by filtered level search
    
## <cg>v2.4.7</c> (2023-07-30)
- Removed "Name Contains", "No ID Search" and "No Forced Star" due to server-side changes
- Removed "GDW Most Liked" due to server-side changes
- Using a faster search method for some filtered searches
    
## <cg>v2.4.6</c> (2023-06-11)
- Added "no coins" filter (thanks acaruso and RandomB)
- Bugfixes & improvements
    
## <cg>v2.4.5</c> (2023-02-03)
- Songs not served from NG highlighted in orange/red/brown/pink/whatever that color is now (idk its a weird shade, i copied it from gdbrowser)
- Fixed a crash when exiting user leaderboards
- Fixed a crash when exiting filtered level search
    
## <cg>v2.4.4</c> (2023-01-16)
- Added visible comment IDs (optional)
- Fixed a crash when viewing some level descriptions
- Bugfixes & improvements
    
## <cg>v2.4.3</c> (2022-11-21)
- Increased delay between requests to reduce/prevent rate-limit IP bans
- Bugfixes & improvements
    
## <cg>v2.4.2</c> (2022-10-26)
- <co>RobTop Bug Fix:</c> Made <cr>unloadable</c> profiles <cg>loadable</c> (but still not searchable)
- <cy>Delayed</c> <cj>auto submit to leaderboards</c> to <co>level exit</c> (+1s)
- <cg>Added</c> a <co>time left indicator</c> to <cj>Finding Last Page</c>
- Bugfixes & improvements
    
## <cg>v2.4.1</c> (2022-08-24)
- <cg>Fixed</c> <cr>spacebar crash</c> in <cj>paginated alerts</c>
- <cg>Added</c> the ability to <co>change pages</c> using <cy>left/right keys</c> in <cj>paginated alerts</c>
- Click <cy>level IDs</c> in <co>created levels</c> to go to the <cj>uploaded version</c> of the level
- Bugfixes & improvements
    
## <cg>v2.4.0</c> (2022-08-17)
- <cg>Added</c> <co>custom</c> <cj>level search filtering</c>
- <cg>Added</c> <co>played & completed</c> <cj>level search</c>
- <cj>No ID search</c> <cg>can</c> now be <co>combined</c> with other BI filters
- Bugfixes & improvements
    
## <cg>v2.3.3-r3</c> (2022-07-31)
- Fixed a PlayLayer crash (this update is silently pushed into wrapper without notifying users updating from original 2.3.3)

## <cg>v2.3.3</c> (2022-07-31)
- <cg>Added</c> separate <cj>normal & practice mode</c> <co>attempts</c>
- <cg>Replaced</c> <cj>scrolling alerts</c> with <co>paginated</c> ones
- <cg>Fixed</c> a <cr>startup crash</c> for people with <cl>more</c> than ~450 <co>daily levels</c>
- <cg>Fixed</c> <cj>auto submit to leaderboards</c> <cr>not</c> submitting coins
- <cg>:)</c>
    
## <cg>v2.3.2</c> (2022-07-22)
- Added <cj>even more</c> <co>Saved Filters</c> (again)
- Added <cj>first/last played, completed dates</c> to <co>Level Stats</c>
- Added <cj>auto submit to level leaderboards</c> (<cr>disabled</c> by default, <cg>enable</c> in the BI settings)
- Bugfixes & improvements

## <cg>v2.3.1</c> (2022-06-28)
- Added <cj>even more</c> <co>Saved Filters</c>
- Added a <co>percentage</c> <cj>Completed Mode</c>
- Bugfixes & improvements
    
## <cg>v2.3.0</c> (2022-06-26)
- Added <cj>Claimed Chest</c> <co>history</c>
- Added <cj>more</c> <cj>Saved Filters</c>
- Added <co>Live</c> <cj>Comment Chat</c> <co>refreshing</c>
- Added <co>more stuff</c> to <cj>Extended Level Info</c>
- Added a <cj>white level ID option</c>
- Introduced the auto updater!

## <cg>v2.2.4</c> (2022-06-15)
- Fixed Copy to Clipboard crashes
- Fixed conflicts with other mods
- Fixed UI misalignments

## <cg>v2.2.3</c> (2022-06-05)
- Moved star filter from profiles to level browser
- Added Copy to clipboard for Extended Level Info and User Profiles
- Bugfixes & improvements

## <cg>v2.2.2</c> (2022-05-28)
- Added full Daily and Weekly History (The Safe from 2.2)
- Bugfixes & improvements

## <cg>v2.2.1</c> (2022-05-23)
- Added star filter for user levels
- Bugfixes & improvements

## <cg>v2.2.0</c> (2022-05-16)
- Added more advanced level search options
- Added jump to last page in level search
- Added saved levels filters
- Added view global leaderboard as other players
- Fixed crashes related to missing resources
- Other bugfixes & improvements

## <cg>v2.1.1</c> (2022-04-25)
- Fixed misalignments in Daily level history
- Added bootups to own user info
- Added a refresh button for leaderboards
- Other bugfixes & improvements

## <cg>v2.1.0</c> (2022-04-15)
New:
  - View Daily/Weekly level history
  - Clicks & best time in level stats
  - Random page button in level search
  - View daily leaderboards for any level
  - Exclamation mark in the creator menu when quests are done
  - Improved user profiles

Fixes:
  - Better progresses calculation
  - Fixed certain layering issues

## <cg>v2.0.1</c> (2022-04-01)
- Fixed a crash related to rendering usernames longer than 15 characters
- Added a workaround for layering issues with the custom green profile dialog

## <cg>v2.0.0</c> (2022-04-01)
Features:
- Profiles for unregistered users
  - Browse levels made by green users
  - Browse comments posted by green users
- Extended level info
- Extended user info
- List of personal bests in the attempts dialog
- Fix green user usernames
- Jump to page for comments
- Uncapped jump to page for levels
- View level comments directly from comment history
- Level IDs in LevelCell
- Visible daily IDs
- Search menu for individual levels
  - Same Song
  - Similar
  - Neighbors
- A custom page that allows you to go to this content
  - GDW Featured
  - GDW Most Liked
  - View the comment section of any arbitrary level ID
  - Settings (disable green username fix, update check and/or extended level info)

# Clickable Green Usernames + Similar Mod (more-button)
## <cg>v1.0.2</c> (2022-03-28)
- Fixed green username positioning in comments

## <cg>v1.0.1</c> (2022-02-18)
* Fix similar button showing on user comment pages
* Fix name alignment with small comments mode off
Features

## <cg>v1.0.0</c> (2022-02-18)
* Browse green user levels by clicking their names anywhere
* Find similar levels by clicking the "Similar" button to the level info screen.
  * Real feature scrapped from the game!