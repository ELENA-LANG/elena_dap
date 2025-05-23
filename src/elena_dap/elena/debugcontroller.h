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

#include <mutex>
#include <functional>
#include <unordered_set>

#include "common\common.h"
#include "ldebugger\ldbg_common.h"
#include "ldebugger\debuginfoprovider.h"

namespace elena_lang
{
   // !! temporal
   // Total number of newlines in source.
   constexpr int64_t numSourceLines = 7;

   // sourceContent holds the synthetic file source.
   constexpr char sourceContent[] = R"(// Hello Debugger!

This is a synthetic source file provided by the DAP debugger.

You can set breakpoints, and single line step.

You may also notice that the locals contains a single variable for the currently executing line number.)";

   class DebugInfoProvider : public DebugInfoProviderBase
   {
   private:
      void retrievePath(ustr_t name, PathString& path, path_t extension) override;
   };

   class DAPDebugProcessBase : public DebugProcessBase
   {
   public:
      enum class ProceedMode
      {
         Running,
         Trapped,
         NewThread
      };

      virtual bool startProcess(path_t debugee, path_t arguments) = 0;

      virtual void run() = 0;

      virtual ProceedMode proceed(int timeout) = 0;
   };

   // Event provides a basic wait and signal synchronization primitive.
   class Event
   {
      std::mutex              mutex;
      std::condition_variable cv;
      bool                    fired = false;

   public:
      // wait() blocks until the event is fired.
      void wait()
      {
         std::unique_lock<std::mutex> lock(mutex);
         cv.wait(lock, [&] { return fired; });
      }

      void reset()
      {
         std::unique_lock<std::mutex> lock(mutex);
         fired = false;
      }

      // fire() sets signals the event, and unblocks any calls to wait().
      void fire()
      {
         std::unique_lock<std::mutex> lock(mutex);
         fired = true;
         cv.notify_all();
      }
   };

   class DebugController
   {
   public:
      enum class EventType { NewThread, BreakpointHit, Stepped, Paused };

      using EventHandler = std::function<void(EventType)>;

   private:
      PathString           _debuggee;
      PathString           _arguments;

      DAPDebugProcessBase* _process;

      DebugInfoProvider    _provider;

      EventHandler         _onEvent;

      Event                _debugActive;
      std::thread          _debugThread;
      std::mutex           _mutex;

      bool                 _running = false;

      // !! temporal
      int64_t           _line = 1;
      std::unordered_set<int64_t> _breakpoints;

      void onLaunch();

      void defineTargetFile(std::string source);

      void runDebugEvent();

      void newThread();

   public:
      void loadDebugInfo();

      bool launch(bool noDebug);

      // run() instructs the debugger to continue execution.
      void run();

      // pause() instructs the debugger to pause execution.
      void pause();

      // currentLine() returns the currently executing line number.
      int64_t currentLine();

      // stepForward() instructs the debugger to step forward one line.
      void stepForward();

      // clearBreakpoints() clears all set breakpoints.
      void clearBreakpoints();

      // addBreakpoint() sets a new breakpoint on the given line.
      void addBreakpoint(int64_t line);

      DebugController(DAPDebugProcessBase* process, std::string source, const EventHandler& onEvent)
         : _process(process), _onEvent(onEvent)
      {
         defineTargetFile(source);
      }
      virtual ~DebugController() = default;
   };
}

#endif // ELENA_DEBUG_CONTROLLER
