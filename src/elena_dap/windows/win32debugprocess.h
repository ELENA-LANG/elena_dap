//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains the main body of the win32 / win64 
//    ELENA Debugger Adapter
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENA_WIN_DEBUG_CONTROLLER
#define ELENA_WIN_DEBUG_CONTROLLER

#include "elena/debugcontroller.h"

namespace elena_lang
{
   class Win32DebugAdapter : public DebugProcessBase
   {
   public:
      bool startProcess(std::string target) override;

      Win32DebugAdapter() = default;
      virtual ~Win32DebugAdapter() = default;
   };
}

#endif // ELENA_WIN_DEBUG_CONTROLLER