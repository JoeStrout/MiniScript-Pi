#include "Shell.h"

// For now at least, we need a static instance because the callbacks
// (like Console::onInputDone) require a static method.  So:
static Shell* instance = nullptr;

static void Print(String s, bool lineBreak=true) {
	TextDisplay::main->Print(s);
	if (lineBreak) TextDisplay::main->PutChar(10);
}

void Shell::HandleConsoleCommand(String s) {
	instance->HandleCommand(s);
}

bool Shell::HandleControlC(void) {
	return instance->Break(false);
}

Shell::~Shell() {
	// We own our own interpreter, but not the console or text display.
	delete interpreter;	interpreter = nullptr;
}

bool Shell::Initialize(Console* console) {
	instance = this;
	
	this->console = console;
	console->onInputDone = HandleConsoleCommand;
	console->onControlC = HandleControlC;
	
	interpreter = new Interpreter;
	
	interpreter->implicitOutput = Print;
	interpreter->standardOutput = Print;	// ToDo: why is this not in MMM?
	interpreter->errorOutput = Print;		// ToDo: ditto
	
	interpreter->REPL("");		// (forces creation of a VM)
	
	Boot();
	
	return true;
}


void Shell::Boot() {
	AddIntrinsics();

	TextDisplay::main->Clear();
	TextDisplay::main->textColor = COLOR32(128,128,128,255);
	Print("Welcome to ", false);
	TextDisplay::main->textColor = COLOR32(128,128,255,255);
	Print("MiniScript-Pi", false);
	TextDisplay::main->textColor = COLOR32(128,128,128,255);
	Print("!\n\n", false);
	TextDisplay::main->textColor = COLOR32(255,128,0,255);
}

void Shell::AddIntrinsics() {
	if (intrinsicsAdded) return;
}

void Shell::Update() {
	if (interpreter == nullptr) return;
	if (interpreter->NeedMoreInput()) {
		GetCommand();	// (though in this case, this really means: get ANOTHER command!)
	} else if (!interpreter->Done()) {
		// continue the running code
		interpreter->RunUntilDone(0.03);
	} else {		// ToDo: runProgram and doLaunchShell cases
		// nothing running; get a command!
		GetCommand();
	}
}

void Shell::GetCommand() {
	if (console->InputInProgress()) return;		// already working on it!
//ToDo: custom shell support 	isRunningShell = false;
	TextDisplay* disp = TextDisplay::main;
	if (disp->GetCursor().col != 0) disp->NextLine();
	String prompt = "]";
	// ToDo: fancy envMap custom prompt stuff
	if (interpreter->NeedMoreInput()) prompt = "...]";
	disp->Print(prompt);
	// ToDo: autocompletion setup
	console->StartInput();
}

void Shell::HandleCommand(String command) {
	if (!interpreter->Done() && !interpreter->NeedMoreInput()) {
		// busy now, save command for later
		inputReceived = command;
		return;
	}
	
	command = command.Trim();
	interpreter->REPL(command, 0.1);
}

bool Shell::Break(bool silent) {
	interpreter->Stop();
	console->AbortInput();
	Print("BREAK");
	
	// Reset the interpreter (creating a new VM), but copy the globals
	// and various type maps out of the old one.
	Machine* oldVM = interpreter->vm;
	// ...ToDo
	interpreter->Reset();
	interpreter->REPL("");	// (forces creation of a VM)
	
	return true;
}