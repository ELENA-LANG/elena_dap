//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains the main body of the win32 / win64 
//    ELENA Debugger Adapter
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <string>

#include "elena/debugcontroller.h"

using namespace elena_lang;

// --- DebugController ---

void DebugController :: defineTargetFile(std::string source)
{
   size_t extIndex = source.find_last_of('.');
   if (extIndex != std::string::npos) {
      _target = source.substr(0, extIndex);
      _target.append(".exe");
   }   
}

bool DebugController :: launch(bool noDebug)
{
   return _process->startProcess(_target);
}