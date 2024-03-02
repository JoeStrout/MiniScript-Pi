#include "ScreenManager.h"

#include <circle/screen.h>
#include <circle/bcmpropertytags.h>
#include <circle/util.h>

ScreenManager::ScreenManager (unsigned nWidth, unsigned nHeight, boolean bVSync, unsigned nDisplay)
: 	m_nWidth(nWidth),
	m_nHeight(nHeight),
	m_nDisplay (nDisplay),
	m_pFrameBuffer(nullptr),
	m_Buffer(nullptr)
{

}

ScreenManager::~ScreenManager (void)
{
	if (m_pFrameBuffer) delete m_pFrameBuffer;
}

boolean ScreenManager::Initialize (void)
{
	m_pFrameBuffer = new CBcmFrameBuffer (m_nWidth, m_nHeight, 
	   DEPTH, m_nWidth, 2*m_nHeight, m_nDisplay, true);

	if (!m_pFrameBuffer || !m_pFrameBuffer->Initialize ()) return false;

	m_baseBuffer = (TScreenColor *) (uintptr) m_pFrameBuffer->GetBuffer();
	m_nWidth = m_pFrameBuffer->GetWidth();
	m_nHeight = m_pFrameBuffer->GetHeight();
	m_Buffer = m_baseBuffer + m_nWidth * m_nHeight;
	
	return true;
}

boolean ScreenManager::Resize (unsigned nWidth, unsigned nHeight)
{
	delete m_pFrameBuffer;
	m_pFrameBuffer = nullptr;
	m_Buffer = nullptr;

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	return Initialize ();
}

void ScreenManager::ClearScreen(TScreenColor color)
{
	FillRect(0, 0, m_nWidth, m_nHeight, color);
}

void ScreenManager::FillRect (unsigned nX, unsigned nY, unsigned nWidth, unsigned nHeight, TScreenColor color)
{
	if (nX + nWidth > m_nWidth || nY + nHeight > m_nHeight) {
		// Just bail out if coordinates are at all out of bounds, apparently.
		// (This explains the little flicker I once saw when bouncing a rect
		// around the screen.)
		return;
	}
	
	for (unsigned i = nY; i < nY + nHeight; i++) {
		for (unsigned j = nX; j < nX + nWidth; j++) {
			m_Buffer[i * m_nWidth + j] = color;
		}
	}
}

void ScreenManager::UpdateDisplay(bool bWaitForVSync)
{
	if (bWaitForVSync) {
		m_pFrameBuffer->WaitForVerticalSync();
	}
	
	// Unlike C2DGraphics, we always copy from the offscreen buffer to
	// the screen.  This way, both buffers (after this update) are
	// identical, and you can do further drawing as needed.
	memcpy(m_baseBuffer, m_Buffer, m_nWidth * m_nHeight * sizeof(TScreenColor));
}
