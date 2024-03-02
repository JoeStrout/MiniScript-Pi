#ifndef _console_h
#define _console_h

#include <circle/usb/usbkeyboard.h>
#include <circle/input/keyboardbuffer.h>

#include "SimpleString.h"
#include "SimpleVector.h"
#include "TextDisplay.h"

using namespace MiniScript;

typedef void (*StringHandler)(String str);
typedef bool (*ControlCHandler)(void);

class Console
{
public:
	Console() : onInputDone(nullptr), keyboard(nullptr), keyboardBuffer(nullptr),
		inInputMode(false), inputStartPos(0,0), historyIndex(0) {}
	
	bool Initialize(CUSBKeyboardDevice* pKeyboard);
	
	void Update();

	void StartInput();
	bool InputInProgress() { return inInputMode; }
	void CommitInput();
	void AbortInput();

	StringHandler onInputDone;
	ControlCHandler onControlC;
	
private:
	static void KeyPressedHandler (const char *pString);

	void HandleKey(String key);
	void SetCursorForInput();
	void ReplaceInput(String newInput);
	int PrevInputStop(int index, bool byWord);
	int NextInputStop(int index, bool byWord);

	CUSBKeyboardDevice* keyboard;
	CKeyboardBuffer* keyboardBuffer;
	bool inInputMode;
	SimpleVector<String> keyBuffer;
	RowCol inputStartPos;			// where on the screen we started taking input
	String inputBuf;				// input taken so far
	int inputIndex;					// where in that input buffer the cursor is
	SimpleVector<String> history;	// past inputs
	int historyIndex;				// where we are in the history
};

#endif  // _console_h
