// Manages the display screen (eventually, combining up to 8 display layers
// of various types and blitting to the screen).

#ifndef _screenmanager_h
#define _screenmanager_h

#define DEPTH 32	// Our color depth is ALWAYS 32.


#include <circle/screen.h>

class ScreenManager
{
public:
	/// \param nWidth   Screen width in pixels (0 to detect)
	/// \param nHeight  Screen height in pixels (0 to detect)
	/// \param bVSync   TRUE to enable VSync and HW double buffering
	/// \param nDisplay Zero-based display number (for Raspberry Pi 4)
	ScreenManager (unsigned nWidth, unsigned nHeight, boolean bVSync = TRUE, unsigned nDisplay = 0);

	~ScreenManager (void);

	/// \return Operation successful?
	boolean Initialize (void);

	/// \param nWidth  New screen width in pixels
	/// \param nHeight New screen height in pixels
	/// \return Operation successful?
	/// \note When FALSE is returned, the width and/or height are not supported.\n
	///	  The object is in an uninitialized state then and must not be used,\n
	///	  but Resize() can be called again with other parameters.
	boolean Resize (unsigned nWidth, unsigned nHeight);

	/// \return Screen width in pixels
	unsigned GetWidth () const { return m_nWidth; }

	/// \return Screen height in pixels
	unsigned GetHeight () const { return m_nHeight; }

	/// \brief Gets raw access to the (offscreen) drawing buffer
	/// \return Pointer to the buffer
	TScreenColor* GetBuffer() const { return m_Buffer; }
	
	/// \brief Gets access to the CBcmFrameBuffer, for VSync etc.
	/// \return Pointer to the CBcmFrameBuffer.
	CBcmFrameBuffer* GetFrameBuffer() const { return m_pFrameBuffer; }

	/// \brief Clears the screen
	/// \param Color Color used to clear the screen
	void ClearScreen(TScreenColor Color);
	
	/// \brief Draws a filled rectangle
	/// \param nX Start X coordinate
	/// \param nY Start Y coordinate
	/// \param nWidth Rectangle width
	/// \param nHeight Rectangle height
	/// \param Color Rectangle color
	void FillRect(unsigned nX, unsigned nY, unsigned nWidth, unsigned nHeight, TScreenColor Color);
	
	/// \brief Once everything has been drawn, updates the display to show the contents on screen
	/// \param bWaitForVSync  If true, wait for vertical blank before updating
	void UpdateDisplay(bool bWaitForVSync=true);

private:
	unsigned m_nWidth;
	unsigned m_nHeight;
	unsigned m_nDisplay;
	CBcmFrameBuffer	*m_pFrameBuffer;
	TScreenColor *m_baseBuffer;
	TScreenColor *m_Buffer;

};

#endif // _screenmanager_h

