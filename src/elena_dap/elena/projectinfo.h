//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains the project model
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PROJECT_INFO
#define PROJECT_INFO

#include "common\common.h"

namespace elena_lang
{
   class ProjectModel
   {
   public:
      PathString           projectPath;
      PathString           outputPath;
      PathString           debuggee;
      PathString           arguments;

      ustr_t getPackage()
      {
         return "sandbox"; // !! temporal
      }

      void assignSingleFileProject(std::string source)
      {
         debuggee.copy(source.c_str());
         debuggee.changeExtension("exe");

         projectPath.copySubPath(*debuggee, false);
         outputPath.copySubPath(*debuggee, false);
      }
   };
}

#endif // PROJECT_INFO
