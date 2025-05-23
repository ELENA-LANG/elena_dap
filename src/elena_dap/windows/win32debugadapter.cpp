//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains the main body of the win32 / win64 
//    ELENA Debugger Adapter
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <string>

#include "windows/win32debugadapter.h"

using namespace elena_lang;

// --- Win32DebugAdapter ---

Win32DebugAdapter::Win32DebugAdapter()
   : _process("Please press any key")
{   
}

bool Win32DebugAdapter :: startProcess(path_t debuggee, path_t arguments)
{
   return _process.startProcess(debuggee.str(), arguments.str(), nullptr, false, false);
}

DAPDebugProcessBase::ProceedMode Win32DebugAdapter :: proceed(int timeout)
{
   _process.processEvent(timeout);
   if (_process.isNewThread())
      return ProceedMode::NewThread;

   return _process.isTrapped() ? ProceedMode::Trapped : ProceedMode::Running;
}

void Win32DebugAdapter :: run()
{
   _process.continueProcess();
}

bool Win32DebugAdapter :: readDump(addr_t address, char* s, pos_t length)
{
   return _process.readDump(address, s, length);
}

void Win32DebugAdapter :: addStep(addr_t address, void* state)
{
   _process.addStep(address, state);
}

void Win32DebugAdapter::setBreakpoint(addr_t address, bool withStackLevelControl)
{
   _process.setBreakpoint(address, withStackLevelControl);
}

void Win32DebugAdapter :: setStepMode()
{
   _process.setStepMode();
}
