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

// Generated with protocol_gen.go -- do not edit this file.
//   go run scripts/protocol_gen/protocol_gen.go
//
// DAP version 1.65.0

#include "dap/protocol.h"

namespace dap 
{
   DAP_IMPLEMENT_STRUCT_TYPEINFO(
      InitializeResponse,
      "",
      DAP_FIELD(additionalModuleColumns, "additionalModuleColumns"),
      DAP_FIELD(breakpointModes, "breakpointModes"),
      DAP_FIELD(completionTriggerCharacters, "completionTriggerCharacters"),
      DAP_FIELD(exceptionBreakpointFilters, "exceptionBreakpointFilters"),
      DAP_FIELD(supportSuspendDebuggee, "supportSuspendDebuggee"),
      DAP_FIELD(supportTerminateDebuggee, "supportTerminateDebuggee"),
      DAP_FIELD(supportedChecksumAlgorithms, "supportedChecksumAlgorithms"),
      DAP_FIELD(supportsBreakpointLocationsRequest,
         "supportsBreakpointLocationsRequest"),
      DAP_FIELD(supportsCancelRequest, "supportsCancelRequest"),
      DAP_FIELD(supportsClipboardContext, "supportsClipboardContext"),
      DAP_FIELD(supportsCompletionsRequest, "supportsCompletionsRequest"),
      DAP_FIELD(supportsConditionalBreakpoints, "supportsConditionalBreakpoints"),
      DAP_FIELD(supportsConfigurationDoneRequest,
         "supportsConfigurationDoneRequest"),
      DAP_FIELD(supportsDataBreakpoints, "supportsDataBreakpoints"),
      DAP_FIELD(supportsDelayedStackTraceLoading,
         "supportsDelayedStackTraceLoading"),
      DAP_FIELD(supportsDisassembleRequest, "supportsDisassembleRequest"),
      DAP_FIELD(supportsEvaluateForHovers, "supportsEvaluateForHovers"),
      DAP_FIELD(supportsExceptionFilterOptions, "supportsExceptionFilterOptions"),
      DAP_FIELD(supportsExceptionInfoRequest, "supportsExceptionInfoRequest"),
      DAP_FIELD(supportsExceptionOptions, "supportsExceptionOptions"),
      DAP_FIELD(supportsFunctionBreakpoints, "supportsFunctionBreakpoints"),
      DAP_FIELD(supportsGotoTargetsRequest, "supportsGotoTargetsRequest"),
      DAP_FIELD(supportsHitConditionalBreakpoints,
         "supportsHitConditionalBreakpoints"),
      DAP_FIELD(supportsInstructionBreakpoints, "supportsInstructionBreakpoints"),
      DAP_FIELD(supportsLoadedSourcesRequest, "supportsLoadedSourcesRequest"),
      DAP_FIELD(supportsLogPoints, "supportsLogPoints"),
      DAP_FIELD(supportsModulesRequest, "supportsModulesRequest"),
      DAP_FIELD(supportsReadMemoryRequest, "supportsReadMemoryRequest"),
      DAP_FIELD(supportsRestartFrame, "supportsRestartFrame"),
      DAP_FIELD(supportsRestartRequest, "supportsRestartRequest"),
      DAP_FIELD(supportsSetExpression, "supportsSetExpression"),
      DAP_FIELD(supportsSetVariable, "supportsSetVariable"),
      DAP_FIELD(supportsSingleThreadExecutionRequests,
         "supportsSingleThreadExecutionRequests"),
      DAP_FIELD(supportsStepBack, "supportsStepBack"),
      DAP_FIELD(supportsStepInTargetsRequest, "supportsStepInTargetsRequest"),
      DAP_FIELD(supportsSteppingGranularity, "supportsSteppingGranularity"),
      DAP_FIELD(supportsTerminateRequest, "supportsTerminateRequest"),
      DAP_FIELD(supportsTerminateThreadsRequest,
         "supportsTerminateThreadsRequest"),
      DAP_FIELD(supportsValueFormattingOptions, "supportsValueFormattingOptions"),
      DAP_FIELD(supportsWriteMemoryRequest, "supportsWriteMemoryRequest"));

   DAP_IMPLEMENT_STRUCT_TYPEINFO(ThreadsResponse,
      "",
      DAP_FIELD(threads, "threads"));

   DAP_IMPLEMENT_STRUCT_TYPEINFO(StackTraceResponse,
      "",
      DAP_FIELD(stackFrames, "stackFrames"),
      DAP_FIELD(totalFrames, "totalFrames"));

   DAP_IMPLEMENT_STRUCT_TYPEINFO(ScopesResponse, "", DAP_FIELD(scopes, "scopes"));

   DAP_IMPLEMENT_STRUCT_TYPEINFO(VariablesResponse,
      "",
      DAP_FIELD(variables, "variables"));

   DAP_IMPLEMENT_STRUCT_TYPEINFO(PauseResponse, "");

   DAP_IMPLEMENT_STRUCT_TYPEINFO(ContinueResponse,
      "",
      DAP_FIELD(allThreadsContinued,
         "allThreadsContinued"));

   DAP_IMPLEMENT_STRUCT_TYPEINFO(NextResponse, "");

   DAP_IMPLEMENT_STRUCT_TYPEINFO(StepBackResponse, "");

   DAP_IMPLEMENT_STRUCT_TYPEINFO(StepInResponse, "");

   DAP_IMPLEMENT_STRUCT_TYPEINFO(StepOutResponse, "");

   DAP_IMPLEMENT_STRUCT_TYPEINFO(SetBreakpointsResponse,
      "",
      DAP_FIELD(breakpoints, "breakpoints"));

   DAP_IMPLEMENT_STRUCT_TYPEINFO(SetExceptionBreakpointsResponse,
      "",
      DAP_FIELD(breakpoints, "breakpoints"));

   DAP_IMPLEMENT_STRUCT_TYPEINFO(SourceResponse,
      "",
      DAP_FIELD(content, "content"),
      DAP_FIELD(mimeType, "mimeType"));

   DAP_IMPLEMENT_STRUCT_TYPEINFO(LaunchResponse, "");

   DAP_IMPLEMENT_STRUCT_TYPEINFO(DisconnectResponse, "");

   DAP_IMPLEMENT_STRUCT_TYPEINFO(ConfigurationDoneResponse, "");

}  // namespace dap
