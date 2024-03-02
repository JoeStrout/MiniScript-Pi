//
//  TextDisplay.h

#ifndef _textdisplay_h
#define _textdisplay_h

#include <circle_stdlib_app.h>
#include <circle/2dgraphics.h>

#include "SimpleVector.h"
#include "MSFontRenderer.h"

class ScreenManager;

namespace MiniScript {

void SetupTextDisplay(ScreenManager* gfx);
void ShutdownTextDisplay();

struct CellContent {
	uint32_t character;
	TScreenColor foreColor;
	TScreenColor backColor;
	bool inverse;		// used only for the hardware cursor!

	void Set(uint32_t aCharacter, TScreenColor aForeColor, TScreenColor aBackColor) {
		character = aCharacter;
		foreColor = aForeColor;
		backColor = aBackColor;
	}
	
	void Clear() { character = 0; }
};

struct RowCol {
	RowCol(int pRow, int pCol) : row(pRow), col(pCol) {}
	int row;
	int col;
};

class TextDisplay {
public:
	static TextDisplay* main;

	TextDisplay(ScreenManager* gfx);
	void Clear();
	void SetCharAtPosition(uint32_t unicodeChar, int row, int column);
	void PutChar(uint32_t unicodeChar);
	void Backup(int count=1);
	void NextLine();
	void Print(MiniScript::String s, bool addEOL=false);
	void UpdateCell(int row, int column);
	void RedrawAll();
	void Update();
	
	void NoteWindowSizeChange(int newWidth, int newHeight);
	
	int GetRow() const { return cursorY; }
	void SetRow(int value) { cursorY = value < 0 ? 0 : (value >= rows ? rows-1 : value); }
	int GetColumn() const { return cursorX; }
	void SetColumn(int value) { cursorX = value < 0 ? 0 : (value >= cols ? cols-1 : value); }
	
	void HideCursor();
	void ShowCursor();
	RowCol GetCursor();
	void SetCursor(RowCol pos) { SetCursor(pos.row, pos.col); }
	void SetCursor(int row, int col);
	
	// Public data:
	int rows;
	int cols;
	TScreenColor textColor;
	TScreenColor backColor;
	bool inverse;
	SimpleVector<SimpleVector<CellContent>> cells;		// indexed by [row][column]

private:
	ScreenManager* gfx;
	MSFontRenderer font;
	
//	void SetStringAtPosition(const char* s, int stringBytes, int row, int column);
//	void SetStringAtPosition(MiniScript::String s, int row, int column);
	void RenderCharAtPosition(uint32_t character, int row, int column, int windowHeight);
	void ScrollUp();
	void HideCursorVisual();
	
	int cursorX;
	int cursorY;
	bool cursorShown = false;		// whether the cursor should be shown at all (except for blinking)
	bool cursorBlinking = false;	// whether cursor is currently hidden just due to blinking
	int cursorFrames;				// how many frames the cursor has been on or off
};

}


#endif // _textdisplay_h
