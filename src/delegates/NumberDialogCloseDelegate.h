#pragma once

class NumberDialogCloseDelegate {
public:
	virtual void onNumberDialogClosed(int number, int additional) = 0;
};