//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains the main body of the win32 / win64 
//    ELENA Debugger Adapter
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//#define HANG_ON_START

#ifdef _MSC_VER

#include <fcntl.h>  // _O_BINARY
#include <io.h>     // _setmode

#endif              

#include <stdio.h>
#include <iostream>

#include "dap/session.h"
#include "dap/stream.h"
#include "dap/protocol.h"

#include "elena/debugcontroller.h"
#include "windows/win32debugadapter.h"
#include "common.h"

using namespace elena_lang;

int main(int argn, char* argv[])
{
   std::cout << "ELENA Debugger Adapter v.1." << ELENA_DAP_REVISION << std::endl;

   if (argn < 2)
      return -1;

   ProjectModel project;

   std::string source;
   //source.assign(argv[1]);
   // !! temporal
   source.assign("C:\\Alex\\elena-lang\\tests60\\sandbox\\sandbox.l");

   project.assignSingleFileProject(source);

   // ==== Debug code ===
#ifdef HANG_ON_START
   int n = 0;
   while (true)
   {
      n++;
   }
#endif
   // =================

#ifdef _MSC_VER
   _setmode(_fileno(stdin), _O_BINARY);
   _setmode(_fileno(stdout), _O_BINARY);
#endif

   std::shared_ptr<dap::Writer> log;
#ifdef LOG_TO_FILE
   log = dap::file(LOG_TO_FILE);
#endif

   // !! testing constants
   const dap::integer frameId = 200;
   const dap::integer variablesReferenceId = 300;
   const dap::integer sourceReferenceId = 400;

   // Create the DAP session.
   // This is used to implement the DAP server.
   auto session = dap::Session::create();

   // Signal events
   Event terminate;
   Event configured;

   // Event handlers from the Debugger.
   auto onDebuggerEvent = [&](DebugController::EventType onEvent, DebugController::EventInfo info) {
      switch (onEvent) {
         case DebugController::EventType::NewThread: {
            // Broadcast the existance of the single thread to the client.
            dap::ThreadEvent threadStartedEvent;
            threadStartedEvent.reason = "started";
            threadStartedEvent.threadId = info.threadId;
            session->send(threadStartedEvent);

            break;
         }
         case DebugController::EventType::Stepped: {
            // The debugger has single-line stepped. Inform the client.
            dap::StoppedEvent event;
            event.reason = "step";
            event.threadId = info.threadId;
            session->send(event);
            break;
         }
         case DebugController::EventType::BreakpointHit: {
            // The debugger has hit a breakpoint. Inform the client.
            dap::StoppedEvent event;
            event.reason = "breakpoint";
            event.threadId = info.threadId;
            session->send(event);
            break;
         }
         case DebugController::EventType::Paused: {
            // The debugger has been suspended. Inform the client.
            dap::StoppedEvent event;
            event.reason = "pause";
            event.threadId = info.threadId;
            session->send(event);
            break;
         }
      }};

   // Construct the debugger.
   elena_lang::DebugController debugger(new elena_lang::Win32DebugAdapter(), &project, onDebuggerEvent);
   //Debugger debugger(onDebuggerEvent);
   debugger.loadDebugInfo(*project.debuggee);

   // Handle errors reported by the Session. These errors include protocol
   // parsing errors and receiving messages with no handler.
   session->onError([&](const char* msg) {
      if (log) {
         dap::writef(log, "dap::Session error: %s\n", msg);
         log->close();
      }
      terminate.fire();
      });

   // The Initialize request is the first message sent from the client and
   // the response reports debugger capabilities.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_Initialize
   session->registerHandler([](const dap::InitializeRequest& request) {
      dap::InitializeResponse response;
      response.supportsConfigurationDoneRequest = true;
      return response;
   });

   // When the Initialize response has been sent, we need to send the initialized
   // event.
   // We use the registerSentHandler() to ensure the event is sent *after* the
   // initialize response.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Events_Initialized
   session->registerSentHandler(
      [&](const dap::ResponseOrError<dap::InitializeResponse>& request) {
         session->send(dap::InitializedEvent());
      });

   // The Threads request queries the debugger's list of active threads.
   // This example debugger only exposes a single thread.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_Threads
   session->registerHandler([&](const dap::ThreadsRequest&) {
      dap::ThreadsResponse response;

      std::list<threadid_t> threads;
      debugger.loadThreadList(threads);
      for (auto it = threads.begin(); it != threads.end(); ++it) {
         dap::Thread thread;

         thread.id = *it;
         response.threads.push_back(thread);
      }

      return response;
      });

   // The StackTrace request reports the stack frames (call stack) for a given
   // thread. This example debugger only exposes a single stack frame for the
   // single thread.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_StackTrace
   session->registerHandler(
      [&](const dap::StackTraceRequest& request)
      -> dap::ResponseOrError<dap::StackTraceResponse> {
         //if (request.threadId != threadId) {
         //   return dap::Error("Unknown threadId '%d'", int(request.threadId));
         //}

         std::string path;
         std::string name;
         int row;
         debugger.loadSourcePath(request.threadId, path, name, row);

         dap::Source source;
         source.path = path;
         source.name = name;

         dap::StackFrame frame;
         frame.line = row;
         frame.column = 1;
         frame.name = name;
         frame.id = frameId;
         frame.source = source;

         dap::StackTraceResponse response;
         response.stackFrames.push_back(frame);
         return response;
      });

   // The Scopes request reports all the scopes of the given stack frame.
   // This example debugger only exposes a single 'Locals' scope for the single
   // frame.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_Scopes
   session->registerHandler([&](const dap::ScopesRequest& request)
      -> dap::ResponseOrError<dap::ScopesResponse> {
         if (request.frameId != frameId) {
            return dap::Error("Unknown frameId '%d'", int(request.frameId));
         }

         dap::Scope scope;
         scope.name = "Locals";
         scope.presentationHint = "locals";
         scope.variablesReference = variablesReferenceId;

         dap::ScopesResponse response;
         response.scopes.push_back(scope);
         return response;
      });

   // The Variables request reports all the variables for the given scope.
   // This example debugger only exposes a single 'currentLine' variable for the
   // single 'Locals' scope.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_Variables
   session->registerHandler([&](const dap::VariablesRequest& request)
      -> dap::ResponseOrError<dap::VariablesResponse> {
         if (request.variablesReference != variablesReferenceId) {
            return dap::Error("Unknown variablesReference '%d'",
               int(request.variablesReference));
         }

         dap::Variable currentLineVar;
         currentLineVar.name = "currentLine";
         currentLineVar.value = std::to_string(debugger.currentLine());
         currentLineVar.type = "int";

         dap::VariablesResponse response;
         response.variables.push_back(currentLineVar);
         return response;
      });

   // The Pause request instructs the debugger to pause execution of one or all
   // threads.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_Pause
   session->registerHandler([&](const dap::PauseRequest&) {
      debugger.pause();
      return dap::PauseResponse();
      });

   // The Continue request instructs the debugger to resume execution of one or
   // all threads.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_Continue
   session->registerHandler([&](const dap::ContinueRequest&) {
      debugger.run();
      return dap::ContinueResponse();
      });

   // The Next request instructs the debugger to single line step for a specific
   // thread.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_Next
   session->registerHandler([&](const dap::NextRequest&) {
      debugger.stepInto();
      return dap::NextResponse();
      });

   // The StepIn request instructs the debugger to step-in for a specific thread.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_StepIn
   session->registerHandler([&](const dap::StepInRequest&) {
      // Step-in treated as step-over as there's only one stack frame.
      debugger.stepInto();
      return dap::StepInResponse();
      });

   // The StepOut request instructs the debugger to step-out for a specific
   // thread.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_StepOut
   session->registerHandler([&](const dap::StepOutRequest&) {
      // Step-out is not supported as there's only one stack frame.
      return dap::StepOutResponse();
      });

   // The SetBreakpoints request instructs the debugger to clear and set a number
   // of line breakpoints for a specific source file.
   // This example debugger only exposes a single source file.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_SetBreakpoints
   session->registerHandler([&](const dap::SetBreakpointsRequest& request) {
      dap::SetBreakpointsResponse response;

      auto breakpoints = request.breakpoints.value({});
      if (request.source.sourceReference.value(0) == sourceReferenceId) {
         debugger.clearBreakpoints();
         response.breakpoints.resize(breakpoints.size());
         for (size_t i = 0; i < breakpoints.size(); i++) {
            debugger.addBreakpoint(breakpoints[i].line);
            response.breakpoints[i].verified = breakpoints[i].line < numSourceLines;
         }
      }
      else {
         response.breakpoints.resize(breakpoints.size());
      }

      return response;
      });

   // The SetExceptionBreakpoints request configures the debugger's handling of
   // thrown exceptions.
   // This example debugger does not use any exceptions, so this is a no-op.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_SetExceptionBreakpoints
   session->registerHandler([&](const dap::SetExceptionBreakpointsRequest&) {
      return dap::SetExceptionBreakpointsResponse();
      });

   // The Source request retrieves the source code for a given source file.
   // This example debugger only exposes one synthetic source file.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_Source
   session->registerHandler([&](const dap::SourceRequest& request)
      -> dap::ResponseOrError<dap::SourceResponse> {
         if (request.sourceReference != sourceReferenceId) {
            return dap::Error("Unknown source reference '%d'",
               int(request.sourceReference));
         }

         dap::SourceResponse response;
         response.content = sourceContent;
         return response;
      });

   // The Launch request is made when the client instructs the debugger adapter
   // to start the debuggee. This request contains the launch arguments.
   // This example debugger does nothing with this request.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_Launch
   session->registerHandler(
      [&](const dap::LaunchRequest& request) 
      { 
         debugger.launch(request.noDebug.value(false));

         return dap::LaunchResponse(); 
      });

   // Handler for disconnect requests
   session->registerHandler([&](const dap::DisconnectRequest& request) {
      if (request.terminateDebuggee.value(false)) {
         terminate.fire();
      }
      return dap::DisconnectResponse();
      });

   // The ConfigurationDone request is made by the client once all configuration
   // requests have been made.
   // This example debugger uses this request to 'start' the debugger.
   // https://microsoft.github.io/debug-adapter-protocol/specification#Requests_ConfigurationDone
   session->registerHandler([&](const dap::ConfigurationDoneRequest& request) {
      configured.fire();
      return dap::ConfigurationDoneResponse();
      });

   // All the handlers we care about have now been registered.
   // We now bind the session to stdin and stdout to connect to the client.
   // After the call to bind() we should start receiving requests, starting with
   // the Initialize request.
   std::shared_ptr<dap::Reader> in = dap::file(stdin, false);
   std::shared_ptr<dap::Writer> out = dap::file(stdout, false);
   if (log) {
      session->bind(spy(in, log), spy(out, log));
   }
   else {
      session->bind(in, out);
   }

   // Wait for the ConfigurationDone request to be made.
   configured.wait();

   debugger.run();

   // Block until we receive a 'terminateDebuggee' request or encounter a session
   // error.
   terminate.wait();

   return 0;
}