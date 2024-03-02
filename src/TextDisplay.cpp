//
//  TextDisplay.cpp
//  This module implements the TextDisplay class, which displays text on the game window
//	using one of the built-in monospaced fonts.

#include "TextDisplay.h"
#include "UnicodeUtil.h"
#include "ScreenManager.h"


namespace MiniScript {

// Public data
TextDisplay* TextDisplay::main = nullptr;

// Constants
#define CURSOR_ON_FRAMES 42
#define CURSOR_OFF_FRAMES 18

// Private data

// Forward declarations


//--------------------------------------------------------------------------------
// Public method implementations
//--------------------------------------------------------------------------------

TextDisplay::TextDisplay(ScreenManager* pGfx) : rows(26), cols(68) {
	gfx = pGfx;
	textColor = COLOR32(255, 128, 0, 255);
	backColor = COLOR32(0,0,0,0);
	cells.resize(rows);
	for (int row=0; row<rows; row++) cells[row].resize(cols);
	Clear();
}

void SetupTextDisplay(ScreenManager* gfx) {
	TextDisplay::main = new TextDisplay(gfx);
}

void ShutdownTextDisplay() {
	delete TextDisplay::main;	TextDisplay::main = nullptr;
}

void TextDisplay::NoteWindowSizeChange(int newWidth, int newHeight) {
	int prevRows = rows;
	int prevCols = cols;
	rows = newHeight / 22;
	cols = newWidth / 14;
	if (rows == prevRows && cols == prevCols) return;
	cells.resize(rows);
	for (int row=0; row<rows; row++) {
		cells[row].resize(cols);
		if (row >= prevRows) prevCols = 0;
		for (int col=prevCols; col<cols; col++) cells[row][col].Clear();
	}
	if (cursorY >= rows) cursorY = rows-1;
	if (cursorX >= cols) cursorX = cols-1;
}

void TextDisplay::RedrawAll() {
	for (int row=0; row<rows; row++) {
		for (int col=0; col<cells[row].size(); col++) {
			UpdateCell(row, col);
		}
	}
}

void TextDisplay::Clear() {
	for (int row=0; row<rows; row++) {
		for (int col=0; col<cols; col++) cells[row][col].Clear();
	}
	cursorX = 0;
	cursorY = rows - 1;
	RedrawAll();
}

void TextDisplay::SetCharAtPosition(uint32_t unicodeChar, int row, int column) {
	if (row >= rows || column >= cols) return;	// out of bounds

	if (inverse) {
		cells[row][column].Set(unicodeChar, backColor, textColor);
	} else {
		cells[row][column].Set(unicodeChar, textColor, backColor);
	}
	UpdateCell(row, column);
}

void TextDisplay::Backup(int count) {
	HideCursorVisual();
	for (int i=0; i<count; i++) {
		cursorX--;
		if (cursorX < 0) {
			if (cursorY >= rows-1) {
				cursorX = 0;
				return;
			}
			cursorY++;
			cursorX = cols - 1;
		}
	}
}

void TextDisplay::UpdateCell(int row, int column) {
	CellContent& cc = cells[row][column];
	font.DrawChar(gfx->GetBuffer(), gfx->GetWidth(), gfx->GetHeight(), 
			cc.character, column * font.GetCharSpacing(), row * font.GetCharHeight(),
			cc.inverse ? cc.backColor : cc.foreColor,
			cc.inverse ? cc.foreColor : cc.backColor);
}

//void TextDisplay::SetStringAtPosition(const char *unicodeString, int stringBytes, int row, int column) {
//	unsigned char *c = (unsigned char*)unicodeString;
//	const unsigned char *end = c + stringBytes;
//	while (c < end) {
//		SetCharAtPosition(UTF8DecodeAndAdvance(&c), row, column++);
//		if (column >= cols) {
//			column = 0;
//			row = row - 1;
//			if (row < 0) break;
//		}
//	}
//}
//
//void TextDisplay::SetStringAtPosition(String s, int row, int column) {
//	SetStringAtPosition(s.c_str(), (int)s.LengthB(), row, column);
//}

void TextDisplay::ScrollUp() {
	for (int row=rows-1; row>0; row--) {
		for (int col=0; col<cols; col++) cells[row][col] = cells[row-1][col];
	}
	for (int col=0; col<cols; col++) cells[0][col].Clear();
	RedrawAll();
	if (cursorY < rows-1) cursorY++;
}

void TextDisplay::PutChar(uint32_t unicodeChar) {
	if (unicodeChar == 9) {						// Tab
		cursorX = ((cursorX+3)/4) * 4;
	} else if (unicodeChar == 10 || unicodeChar == 13) {	// Line break
		cursorX = cols + 1;
	} else if (unicodeChar == 134) {
		inverse = true;
	} else if (unicodeChar == 135) {
		inverse = false;
	} else {
		SetCharAtPosition(unicodeChar, cursorY, cursorX++);
	}
	
	if (cursorX >= cols) {
		cursorX = 0;
		cursorY = cursorY - 1;
		if (cursorY < 0) {
			ScrollUp();
			cursorY = 0;
		}
	}
}

void TextDisplay::Print(String s, bool addEOL) {
	HideCursorVisual();
	if (cursorY >= rows) cursorY = rows-1;
	if (cursorX >= cols) cursorX = cols-1;

	unsigned char *c = (unsigned char*)(s.c_str());
	const unsigned char *end = c + s.LengthB();
	while (c < end) {
		PutChar(UTF8DecodeAndAdvance(&c));
	}
	if (addEOL) PutChar('\n');
}

void TextDisplay::NextLine() {
	cursorY--;
	while (cursorY < 0) ScrollUp();
	cursorX = 0;
}

void TextDisplay::Update() {
	if (cursorShown) {
		// Make the cursor blink!
		cursorFrames++;
		if (!cursorBlinking && cursorFrames >= CURSOR_ON_FRAMES) {
			HideCursor();
			cursorShown = true;
			cursorBlinking = true;
		} else if (cursorBlinking && cursorFrames >= CURSOR_OFF_FRAMES) {
			ShowCursor();
			cursorBlinking = false;
		}
	}
}

void TextDisplay::HideCursor() {
	HideCursorVisual();
	cursorFrames = 0;
	cursorShown = false;
}

void TextDisplay::TextDisplay::ShowCursor() {
	cells[cursorY][cursorX].inverse = true;
	if (cells[cursorY][cursorX].foreColor != textColor) {
		// Looks ike the cursor is over some weird-colored text...
		// probably from Autocomplete.  Adjust colors accordingly.
		cells[cursorY][cursorX].foreColor = textColor;
		cells[cursorY][cursorX].backColor = backColor; // ToDo: Color.Lerp(backColor, textColor, 0.75f);
	}
	UpdateCell(cursorY, cursorX);
	cursorFrames = 0;
	cursorShown = true;
}

RowCol TextDisplay::GetCursor() {
	return RowCol(cursorY, cursorX);
}

void TextDisplay::SetCursor(int row, int col) {
	bool wasShown = cursorShown;
	if (cursorShown) HideCursor();
	cursorX = col;
	if (cursorX < 0) cursorX = 0;
	else if (cursorX >= cols) cursorX = cols - 1;
	cursorY = row;
	if (cursorY < 0) cursorY = 0;
	else if (cursorY >= rows) cursorY = rows - 1;
	if (wasShown) ShowCursor();
}

//--------------------------------------------------------------------------------
// Private method implementations
//--------------------------------------------------------------------------------

/// Hide the visual display of the cursor, if any.
/// Does not actually turn the cursor off.
void TextDisplay::HideCursorVisual() {
	if (!cursorShown) return;
	cells[cursorY][cursorX].inverse = false;
	if (cells[cursorY][cursorX].backColor != backColor) {
		// Looks like the cursor is over some weird-colored text...
		// probably from Autocomplete.  Adjust colors accordingly.
		cells[cursorY][cursorX].foreColor = textColor; // ToDo: Color.Lerp(textColor, backColor, 0.75f);
		cells[cursorY][cursorX].backColor = backColor;
	}
	UpdateCell(cursorY, cursorX);
}


} // namespace MiniScript
