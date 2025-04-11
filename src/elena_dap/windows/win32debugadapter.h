//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains the main body of the win32 / win64 
//    ELENA Debugger Adapter
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENA_WIN_DEBUG_ADAPTER_H
#define ELENA_WIN_DEBUG_ADAPTER_H

#include "elena/debugcontroller.h"

#include "ldebugger/windows/win32debugprocess.h"

namespace elena_lang
{
   class Win32DebugAdapter : public DebugProcessBase
   {
      Win32DebugProcess _process;

   public:
      bool startProcess(path_t debugee, path_t arguments) override;

      ProceedMode proceed(int timeout) override;

      void run() override;

      Win32DebugAdapter();
      virtual ~Win32DebugAdapter() = default;
   };
}

#endif // ELENA_WIN_DEBUG_ADAPTER_H