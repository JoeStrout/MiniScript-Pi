
#include "Console.h"
#include "kernel.h"
#include "TextDisplay.h"

using namespace MiniScript;

static Console* console = nullptr;

// Key input constants (as they come from the USB keyboard)
const String kESC = "\u001b";
const String kLeftArrow = kESC + "[D";
const String kRightArrow = kESC + "[C";
const String kUpArrow = kESC + "[A";
const String kDownArrow = kESC + "[B";
const String kTab = "\u0009";
const String kEnter = "\u000A";
const String kControlA = "\u0001";
const String kControlC = "\u0003";
const String kControlE = "\u0005";
const String kControlK = "\u000B";
const String kControlU = "\u0015";
const String kBackspace = "\u007F";
const String kFwdDelete = kESC + "[3~";

static void Print(String s) {
	TextDisplay::main->Print(s);
}

void Console::KeyPressedHandler (const char *pString)
{
#ifdef EXPAND_CHARACTERS
	while (*pString) {
	  CString s;
	  s.Format ("'%c' %d %02X\n", *pString, *pString, *pString);
          pString++;
	  s_pThis->m_Screen.Write (s, strlen (s));
          }
#else
	console->HandleKey(pString);
#endif
}

bool Console::Initialize(CUSBKeyboardDevice* pKeyboard) {
	console = this;
	if (!pKeyboard) return false;
	
	pKeyboard->RegisterKeyPressedHandler(Console::KeyPressedHandler);
	return true;
}

void Console::Update() {
}

void Console::StartInput() {
	inputStartPos = TextDisplay::main->GetCursor();
	//ClearSelection();
	TextDisplay::main->ShowCursor();
	inputBuf = "";
	inputIndex = 0;
	inInputMode = true;
	historyIndex = history.size();
}

void Console::AbortInput() {
	inInputMode = false;
	TextDisplay::main->HideCursor();
	keyBuffer.deleteAll();
}

void Console::HandleKey(String keyChar) {
	if (keyChar == kControlC) {
		if (onControlC && onControlC()) return;
	}
	if (!inInputMode) {
		// When we're not in input mode, just buffer up the keys for future consumption.
		keyBuffer.push_back(keyChar);
		return;
	}
	TextDisplay* display = TextDisplay::main;
	int keyInt = keyChar.CodePoint();
	bool byWord = false;		// ToDo
	if (keyChar == kBackspace) {		// Backspace
		int stop = PrevInputStop(inputIndex, byWord);
		if (stop < inputIndex) {
			inputBuf = inputBuf.Substring(0, stop) + inputBuf.Substring(inputIndex);
			int delCount = inputIndex - stop;
			display->Backup(delCount);
			display->Print(inputBuf.Substring(stop));
			for (int i=0; i<delCount; i++) display->PutChar(32);
			inputIndex = stop;
		}
	} else if (keyInt >= 32 and keyInt < 127) {	// ToDo: remove "and" clause
		// Regular ol' printing characters
		inputBuf = inputBuf.Substring(0, inputIndex) + keyChar + inputBuf.Substring(inputIndex);
		display->Print(inputBuf.Substring(inputIndex));
		inputIndex++;
	} else if (keyChar == kFwdDelete) {
		int stop = NextInputStop(inputIndex, byWord);
		if (stop > inputIndex) {
			inputBuf = inputBuf.Substring(0, inputIndex) + inputBuf.Substring(stop);
			display->Print(inputBuf.Substring(inputIndex));
			for (int i=0; i<stop-inputIndex; i++) display->PutChar(' ');
		}
	} else if (keyChar == kEnter) {
		CommitInput();
	} else if (keyChar == kLeftArrow) {
		inputIndex = PrevInputStop(inputIndex, byWord);
	} else if (keyChar == kRightArrow) {
		inputIndex = NextInputStop(inputIndex, byWord);
	} else if (keyChar == kUpArrow) {
		if (historyIndex > 0) {
			historyIndex--;
			ReplaceInput(history[historyIndex]);
		}
	} else if (keyChar == kDownArrow) {
		if (historyIndex < history.size()) {
			historyIndex++;
			ReplaceInput(historyIndex < history.size() ? history[historyIndex] : "");
		}
	} else if (keyChar == kControlA) {
		inputIndex = 0;
	} else if (keyChar == kControlE) {
		inputIndex = inputBuf.Length();
	} else if (keyChar == kControlK) {	// del to end of line
		ReplaceInput(inputBuf.Substring(0, inputIndex));
	} else if (keyChar == kControlU) {	// del to start of line
		ReplaceInput(inputBuf.Substring(inputIndex));
	} else {
		// Dump codes for debugging
		for (int i=0; i < keyChar.Length(); i++) {
			String c = keyChar.Substring(i, 1);
			if (i > 0) Print(", ");
			Print(String::Format((long)c.CodePoint()));
			if (c.CodePoint() >= 32) {
				TextDisplay::main->inverse = true;
				TextDisplay::main->Print(c);
				TextDisplay::main->inverse = false;				
			}
		}
		Print("\n");
	}
	
	if (inInputMode) SetCursorForInput();
}

void Console::CommitInput() {
	// ClearSelection();
	inputIndex = inputBuf.Length();
	SetCursorForInput();
	if (!inputBuf.empty()) history.push_back(inputBuf);
	TextDisplay::main->HideCursor();
	TextDisplay::main->NextLine();
	inInputMode = false;
	if (onInputDone) onInputDone(inputBuf);
}

void Console::SetCursorForInput() {
	TextDisplay* display = TextDisplay::main;
	int curRow = inputStartPos.row;
	int curCol = inputStartPos.col + inputIndex;
	while (curCol >= display->cols) {
		curRow--;
		curCol -= display->cols;
	}
	display->SetCursor(curRow, curCol);
}

void Console::ReplaceInput(String newInput) {
	TextDisplay* display = TextDisplay::main;
	display->SetCursor(inputStartPos);
	for (int i=0; i<inputBuf.Length(); i++) display->PutChar(32);
	inputBuf = newInput;
	display->SetCursor(inputStartPos);
	display->Print(inputBuf);
	inputIndex = inputBuf.Length();
	SetCursorForInput();
}

int Console::PrevInputStop(int index, bool byWord) {
	if (index <= 0) return index;
	index--;
	if (byWord) { // ToDo
/*		// skip any nonword characters; then continue till we get to nonword chars again
		bool numeric = false;
		while (!Miniscript.CodeEditor.IsTokenChar(inputBuf[index], ref numeric) && index > 0) {
			index--;
		}
		while (index > 0 && Miniscript.CodeEditor.IsTokenChar(inputBuf[index-1], ref numeric)) {
			index--;
		}			
*/
	}
	return index;
}

int Console::NextInputStop(int index, bool byWord) {
	int maxi = inputBuf.Length();
	if (index >= maxi) return index;
	index++;
	if (byWord) { // ToDo
/*
		// skip any nonword characters; then continue till we get to nonword chars again
		bool numeric = false;
		while (index < maxi && !Miniscript.CodeEditor.IsTokenChar(inputBuf[index], ref numeric)) {
			index++;
		}
		while (index < maxi && Miniscript.CodeEditor.IsTokenChar(inputBuf[index-1], ref numeric)) {
			index++;
		}			
*/
	}
	return index;
}
