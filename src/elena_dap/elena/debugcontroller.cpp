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
#include "engine/elenacommon.h"
#include "ldebugger/debuginfoprovider.h"

using namespace elena_lang;

// --- DebugInfoProvider ---

bool DebugInfoProvider :: provideFullPath(ustr_t ns, ustr_t sourcePath, std::string& output)
{
   if (_project->isIncluded(ns)) {
      PathString fullPath(*_project->projectPath, sourcePath);

      IdentifierString fullPathStr(*fullPath);

      output.assign(*fullPathStr);

      return true;
   }
   else {
      PathString fullPath;

   //   for (auto ref_it = model.referencePaths.start(); !ref_it.eof(); ++ref_it) {
   //      ustr_t extPackage = ref_it.key();
   //      if (NamespaceString::isIncluded(extPackage, ns)) {
   //         fullPath.copy(*model.projectPath);
   //         PathUtil::combineCanonicalized(fullPath, *ref_it);
   //         fullPath.combine(path);

   //         return;
   //      }
   //   }

      fullPath.copy(*_project->librarySourceRoot);
      // HOTFIX : ignore sub ns
      size_t index = ns.find('\'');
      if (index != NOTFOUND_POS) {
         fullPath.combine(ns, index);
      }
      else fullPath.combine(ns);
      fullPath.combine(sourcePath);

      IdentifierString fullPathStr(*fullPath);
      output.assign(*fullPathStr);

      return true;
   }

   return false;
}

void DebugInfoProvider::retrievePath(ustr_t name, PathString& path, path_t extension)
{
   ustr_t package = _project->getPackage();

   // if it is the project package
   if (isEqualOrSubSetNs(package, name)) {
      defineModulePath(name, path, *_project->projectPath, *_project->outputPath, extension);
   }
   else {
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

      // if file doesn't exist use package root
      path.copy(*_project->libraryRoot);

      ReferenceName::nameToPath(path, name);
      path.appendExtension(extension);
   }
}

// --- DebugController ---

void DebugController :: loadDebugInfo(path_t debuggee)
{
   DebugInfoProviderBase::loadDebugInfo(debuggee, &_provider, _process);
}

void DebugController :: runDebugEvent()
{
   switch (_process->proceed(100)) {
      case DAPDebugProcessBase::ProceedMode::Trapped:
         _debugActive.reset();
         onStep();
         break;
      case DAPDebugProcessBase::ProceedMode::NewThread:
         newThread();
         _process->run();
         break;
      case DAPDebugProcessBase::ProceedMode::Stopped:
         onStopped();
         break;
      default:
         _process->run();
         break;
   }
}

void DebugController :: onLaunch()
{
   _loaded = true;

   addr_t entryAddr = _provider.getEntryPoint();
   _process->setBreakpoint(entryAddr, false);
}

void DebugController :: onStopped()
{
   _running = false;
   _loaded = false;
}

bool DebugController :: launch(bool noDebug)
{
   _debugThread = std::thread([this] {
      if (!_process->startProcess(*_project->debuggee, *_project->arguments))
         return;

      _running = true;
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
   if (!_loaded) {
      onLaunch();
   }

   std::unique_lock<std::mutex> lock(_mutex);

   EventInfo info = { _process->getCurrentThreadId() };

   _threads[info.threadId] = {};

   lock.unlock();

   _onEvent(EventType::NewThread, info);
}

void DebugController :: stepInto()
{
   //if (!_started) {
   //   if (_provider.getDebugInfoPtr() == 0 && _provider.getEntryPoint() != 0) {
   //      _process->setBreakpoint(_provider.getEntryPoint(), false);
   //      _postponed.setStepMode();
   //   }
   //   _started = true;
   //}
   //else {
   //   DebugLineInfo* lineInfo = _provider.seekDebugLineInfo((addr_t)_process->getState());

   //   //// if debugger should notify on the step result
   //   //if (test(lineInfo->symbol, dsProcedureStep))
   //   //   _debugger.setCheckMode();

   //   DebugLineInfo* nextStep = _provider.getNextStep(lineInfo, false);
   //   // if the address is the same perform the virtual step
   //   if (nextStep && nextStep->addresses.step.address == lineInfo->addresses.step.address) {
   //      _process->setStepMode();
   //   }
   //   /*else if (test(lineInfo->symbol, dsAtomicStep)) {
   //      _debugger.setBreakpoint(nextStep->addresses.step.address, true);
   //   }*/
   //   // else set step mode
      /*else */_process->setStepMode();
   //}

   _debugActive.fire();
}

void DebugController :: pause() 
{
   EventInfo info = {};

   _onEvent(EventType::Paused, info);
}

void DebugController :: onStep()
{
   std::unique_lock<std::mutex> lock(_mutex);

   if (_process->isTrapped()) {
      IdentifierString moduleName;
      ustr_t sourcePath = nullptr;

      threadid_t threadId = _process->getCurrentThreadId();

      DebugLineInfo* lineInfo = _provider.seekDebugLineInfo((addr_t)_process->getState(), moduleName, sourcePath);
      if (lineInfo && _provider.provideFullPath(*moduleName, sourcePath, _threads[threadId].path)) {
         _threads[threadId].moduleName.assign(moduleName.str());
         _threads[threadId].row = lineInfo->row;

         EventInfo info = { threadId };
         _onEvent(EventType::Stepped, info);
      }
      else {
         // if the module was not found - continue running

         _process->setStepMode();
         _debugActive.fire();
      }

      //if (_postponed.autoNextLine) {
      //   if (lineInfo->row == _postponed.row) {
      //      autoStepOver();

      //      return;
      //   }

      //   _postponed.autoNextLine = false;
      //}
   }

   lock.unlock();
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