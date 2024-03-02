#ifndef _shell_h
#define _shell_h

#include "Console.h"
#include "TextDisplay.h"
#include "MiniscriptInterpreter.h"

using namespace MiniScript;

class Shell
{
public:
	Shell() : console(nullptr), interpreter(nullptr), runProgram(false) {}
	~Shell();
	bool Initialize(Console* console);
	void Boot();
	void Update();
	
	static void HandleConsoleCommand(String s);
	static bool HandleControlC(void);
	
private:
	void AddIntrinsics();
	void GetCommand();
	void HandleCommand(String command);
	bool Break(bool silent=false);
	
	Console* console;
	Interpreter* interpreter;
	static bool intrinsicsAdded;
	String inputReceived;	// stores input while app is running, for _input intrinsic
	bool runProgram;
};

#endif // _shell_h
