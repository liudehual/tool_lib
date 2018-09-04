#ifndef _OS_CODEFRAGMENT_H_
#define _OS_CODEFRAGMENT_H_
#include <stdlib.h>
class OSCodeFragment
{
    public:
      OSCodeFragment(const char* inPath);
      ~OSCodeFragment();
      unsigned short  IsValid() { return (fFragmentP != NULL); }
      void* GetSymbol(const char* inSymbolName);
    private:
      void*   fFragmentP;
};
#endif//_OS_CODEFRAGMENT_H_
