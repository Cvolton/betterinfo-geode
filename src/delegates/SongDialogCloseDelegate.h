#pragma once

class SongDialogCloseDelegate {
public:
	virtual void onSongDialogClosed(bool custom, int songID) = 0;
};