//
// kernel.cpp
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "kernel.h"
#include "SimpleString.h"
#include "MiniscriptInterpreter.h"
#include "MSFontRenderer.h"
#include "TextDisplay.h"
#include "Console.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>

#if DEPTH != 32
	#warning DEPTH is not 32!!!  We need DEPTH defined as 32 for proper color.
#endif

using namespace MiniScript;

CKernel* kernel = nullptr;

CKernel::CKernel (void)
        : CStdlibAppStdio("MS-Pi"),
        m_frameCount(0),
        m_ScreenManager(m_Options.GetWidth(), m_Options.GetHeight())
{
	kernel = this;
}

CKernel::~CKernel (void)
{
	ShutdownTextDisplay();
	if (kernel == this) kernel = nullptr;
}

boolean CKernel::Initialize (void)
{
	CStdlibAppStdio::Initialize();
	bool ok = m_ScreenManager.Initialize();

	SetupTextDisplay(&m_ScreenManager);
	
  	if (ok) ok = console.Initialize((CUSBKeyboardDevice*)mDeviceNameService.GetDevice("ukbd1", FALSE));
	
	if (ok) {
		shell = new Shell();
		ok = shell->Initialize(&console);
	}
	
	if (!ok) SOS();
	return ok;
}

void CKernel::SOS() {
	while (true) {
		mActLED.Blink(3, 250,100);	// S
		mTimer.MsDelay(500);
		mActLED.Blink(3, 500,100);	// O
		mTimer.MsDelay(500);
		mActLED.Blink(3, 250,100);	// S
		mTimer.MsDelay(1000);		
	}
}

CStdlibApp::TShutdownMode CKernel::Run (void)
{
	m_ScreenManager.ClearScreen(BLACK_COLOR);
	shell->Boot();

	while (1) {
		shell->Update();
 		console.Update();
		TextDisplay::main->Update();
 		m_ScreenManager.UpdateDisplay();

		// Blink the LED once per second (much like a cursor!)
		unsigned long frameInSec = m_frameCount % 60;
		if (frameInSec == 0) mActLED.On();
		else if (frameInSec == 42) mActLED.Off();
		m_frameCount++;
	}
		
	mLogger.Write (GetKernelName (), LogNotice, "Bare-Metal MiniScript Demo finished");

	return ShutdownHalt;
}
