// This class is responsible for rendering the built-in fonts (the same
// ones used in the TextDisplay).  It's designed for speed and simplicity
// (and so has a very narrow feature set).

#ifndef _msfontrenderer
#define _msfontrenderer

#include <circle/2dgraphics.h>
#include "SimpleString.h"

#define CLEAR_COLOR			COLOR32(0, 0, 0, 0)


namespace MiniScript {

class MSFontRenderer 
{
public:
	enum TMSFont
	{
		Small,
		Medium,
		Normal,
		Large
	};

	MSFontRenderer(TMSFont font = TMSFont::Normal);
	~MSFontRenderer(void);
	
	// Low-level drawing routine.  Y coordinate is measured from the bottom,
	// and refers to the bottom of the character bounds (so y=0 places the 
	// character at the very bottom of the screen).  Returns without effect if 
	// the character bounds is at all out of bounds for the given buffer.
	void DrawChar(TScreenColor* buffer, unsigned bufWidth, unsigned bufHeight, 
		int codePoint, int x, int y, TScreenColor foreColor, TScreenColor backColor);
	
	
	// Higher-level drawing routines.
	void PrintAtPixelPos(C2DGraphics gfx, MiniScript::String s,
		int x, int y, TScreenColor foreColor, TScreenColor backColor);

	void PrintAt(C2DGraphics gfx, String s,
		int column, int row, TScreenColor foreColor, TScreenColor backColor);
	
	// Inspectors
	int GetCharHeight() { return charHeight; }
	int GetCharWidth() { return charWidth; }
	int GetCharSpacing() { return charSpacing; }
	int GetKerning() { return charWidth - charSpacing; }	

private:
	int charHeight;
	int charWidth;
	int charSpacing;
};

} // namespace MiniScript


#endif // _msfontrenderer