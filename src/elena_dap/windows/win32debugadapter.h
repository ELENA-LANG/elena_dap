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
   class Win32DebugAdapter : public DAPDebugProcessBase
   {
      Win32DebugProcess _process;

      bool readDump(addr_t address, char* s, pos_t length) override;

      void addStep(addr_t address, void* current) override;

   public:
      bool startProcess(path_t debugee, path_t arguments) override;

      ProceedMode proceed(int timeout) override;

      void run() override;

      void setBreakpoint(addr_t address, bool withStackLevelControl) override;

      Win32DebugAdapter();
      virtual ~Win32DebugAdapter() = default;
   };
}

#endif // ELENA_WIN_DEBUG_ADAPTER_H