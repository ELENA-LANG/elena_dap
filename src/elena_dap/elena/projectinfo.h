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
#include "engine\elenacommon.h"

namespace elena_lang
{
   class ProjectModel
   {
   public:
      PathString           libraryRoot;
      PathString           librarySourceRoot;
      PathString           projectPath;
      PathString           outputPath;
      PathString           debuggee;
      PathString           arguments;

      bool isIncluded(ustr_t ns)
      {
         return NamespaceString::isIncluded(getPackage(), ns);
      }

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

         libraryRoot.copy("C:\\Alex\\elena-lang\\lib60");
         librarySourceRoot.copy("C:\\Alex\\elena-lang\\src60");
      }
   };
}

#endif // PROJECT_INFO
