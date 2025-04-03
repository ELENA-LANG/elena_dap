//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains the main body of the win32 / win64 
//    ELENA Debugger Adapter
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENA_DEBUG_CONTROLLER
#define ELENA_DEBUG_CONTROLLER

namespace elena_lang
{
   class DebugProcessBase
   {
   public:
      virtual bool startProcess(std::string target) = 0;

      virtual ~DebugProcessBase() = default;
   };

   class DebugController
   {
      std::string       _target;

      DebugProcessBase* _process;

      void defineTargetFile(std::string source);

   public:
      bool launch(bool noDebug);

      DebugController(DebugProcessBase* process, std::string source)
         : _process(process)
      {
         defineTargetFile(source);
      }
   };
}

#endif // ELENA_DEBUG_CONTROLLER
