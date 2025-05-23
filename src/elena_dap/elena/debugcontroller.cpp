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
#include "ldebugger/debuginfoprovider.h"

using namespace elena_lang;

// --- DebugInfoProvider ---

void DebugInfoProvider::retrievePath(ustr_t name, PathString& path, path_t extension)
{
   //ustr_t package = _model->getPackage();

   //// if it is the project package
   //if (isEqualOrSubSetNs(package, name)) {
   //   defineModulePath(name, path, *_model->projectPath, _model->getOutputPath(), extension);
   //}
   //else {
   //   // check external libraries
   //   for (auto ref_it = _model->referencePaths.start(); !ref_it.eof(); ++ref_it) {
   //      ustr_t extPackage = ref_it.key();
   //      if (isEqualOrSubSetNs(extPackage, name)) {
   //         path.copy(*_model->projectPath);
   //         path.combine(*ref_it);

   //         ReferenceName::nameToPath(path, name);
   //         path.appendExtension(extension);

   //         return;
   //      }
   //   }

   //   // if file doesn't exist use package root
   //   path.copy(*_model->paths.libraryRoot);

   //   ReferenceName::nameToPath(path, name);
   //   path.appendExtension(extension);
   //}
}

// --- DebugController ---

void DebugController :: loadDebugInfo()
{
   DebugInfoProviderBase::loadDebugInfo(*_debuggee, &_provider, _process);
}

void DebugController :: defineTargetFile(std::string source)
{
   _debuggee.copy(source.c_str());
   _debuggee.changeExtension("exe");
}

void DebugController :: runDebugEvent()
{
   switch (_process->proceed(100)) {
      case DAPDebugProcessBase::ProceedMode::Trapped:
         _debugActive.reset();

         //processStep();
         break;
      case DAPDebugProcessBase::ProceedMode::NewThread:
         newThread();
         _process->run();
         break;
      default:
         _process->run();
         break;
   }
}

void DebugController :: onLaunch()
{
   addr_t entryAddr = _provider.getEntryPoint();
   _process->setBreakpoint(entryAddr, false);
}

bool DebugController :: launch(bool noDebug)
{
   _debugThread = std::thread([this] {
      if (!_process->startProcess(*_debuggee, *_arguments))
         return;

      _running = true;

      onLaunch();

      while (_running) {
         _debugActive.wait();

         runDebugEvent();
      }
      });

   return true;
}

void DebugController :: run()
{
   std::unique_lock<std::mutex> lock(_mutex);
   //for (int64_t i = 0; i < numSourceLines; i++) {
   //   int64_t l = ((_line + i) % numSourceLines) + 1;
   //   if (_breakpoints.count(l)) {
   //      _line = l;
   //      lock.unlock();
   //      _onEvent(EventType::BreakpointHit);
   //      return;
   //   }
   //}

   _debugActive.fire();
}

void DebugController :: newThread()
{
   _onEvent(EventType::NewThread);
}

void DebugController :: pause() 
{
   _onEvent(EventType::Paused);
}

int64_t DebugController::currentLine()
{
   std::unique_lock<std::mutex> lock(_mutex);
   return _line;
}

void DebugController::stepForward() 
{
   std::unique_lock<std::mutex> lock(_mutex);
   _line = (_line % numSourceLines) + 1;
   lock.unlock();
   _onEvent(EventType::Stepped);
}

void DebugController::clearBreakpoints() 
{
   std::unique_lock<std::mutex> lock(_mutex);
   this->_breakpoints.clear();
}

void DebugController::addBreakpoint(int64_t l)
{
   std::unique_lock<std::mutex> lock(_mutex);
   this->_breakpoints.emplace(l);
}