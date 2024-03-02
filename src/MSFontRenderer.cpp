#include "MSFontRenderer.h"
#include "fontData_normal.h"
#include "UnicodeUtil.h"

namespace MiniScript {

MSFontRenderer::MSFontRenderer(TMSFont font)
{
	charWidth = 16;
	charHeight = 24;
	charSpacing = 14;
	FontData_Normal::init();
}

MSFontRenderer::~MSFontRenderer(void)
{
}

// Low-level drawing routine.  Y coordinate is measured from the bottom,
// and refers to the bottom of the character bounds (so y=0 places the 
// character at the very bottom of the screen).  Returns without effect if 
// the character bounds is at all out of bounds for the given buffer.
void MSFontRenderer::DrawChar(TScreenColor* buffer, unsigned bufWidth, unsigned bufHeight, 
		int codePoint, int x, int y, TScreenColor foreColor, TScreenColor backColor)
{
	// bail out if at all out of bounds
 	if (x < 0 || x+charWidth >= bufWidth
 	 || y < 0 || y+charHeight >= bufHeight) return;
	
	// font data indexed by [charCode-32]:
	uint32_t** lowCharData = FontData_Normal::lowCharData;
 	uint32_t* charData = nullptr;
 	if (codePoint < 32 || codePoint > 255) {
 		// ToDo: other chars!
 	} else {
 		// common characters (32 through 224) we can look up directly:
		charData = lowCharData[codePoint - 32];
	}
	
	// calculate starting point in buffer (adjusting for bottom-up Y)
	TScreenColor *lineStart = buffer + (bufHeight - y - charHeight) * bufWidth + x;
	for (int y = 0; y < charHeight; y++) {
		TScreenColor *p = lineStart;
		uint32_t charBits = charData ? charData[y] << (32-charWidth) : 0;
		for (int x = 0; x < charWidth; x++) {
			*p++ = (charBits & 0x80000000) ? foreColor : backColor;
			charBits <<= 1;
		}
		lineStart += bufWidth;
	}
}

void MSFontRenderer::PrintAtPixelPos(C2DGraphics gfx, String s,
		int x, int y, TScreenColor foreColor, TScreenColor backColor)
{
	unsigned char *charBuf = (unsigned char*)s.c_str();
	unsigned char *bufEnd = charBuf + s.LengthB();
	
	TScreenColor* screenBuf = gfx.GetBuffer();
	unsigned bufWidth = gfx.GetWidth();
	unsigned bufHeight = gfx.GetHeight();
	
	while (charBuf < bufEnd) {
		unsigned long codePoint = UTF8DecodeAndAdvance(&charBuf);
		DrawChar(screenBuf, bufWidth, bufHeight, (int)codePoint,
				 x, y, foreColor, backColor);
		x += charSpacing;
		if (x + charWidth > bufWidth) {
			x = 0;
			y += charHeight;
		}
	}
}

void MSFontRenderer::PrintAt(C2DGraphics gfx, String s,
		int column, int row, TScreenColor foreColor, TScreenColor backColor)
{
	PrintAtPixelPos(gfx, s, column * charWidth, row * charHeight, foreColor, backColor);
}

} // namespace MiniScript
