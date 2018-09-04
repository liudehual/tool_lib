#include "OSCodeFragment.h"
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

OSCodeFragment::OSCodeFragment(const char* inPath)
: fFragmentP(NULL)
{
    fFragmentP = ::dlopen(inPath, RTLD_NOW | RTLD_GLOBAL);
}

OSCodeFragment::~OSCodeFragment()
{
    if (fFragmentP == NULL)
        return;
    ::dlclose(fFragmentP);
}

void*   OSCodeFragment::GetSymbol(const char* inSymbolName)
{
    if (fFragmentP == NULL)
        return NULL;
    return ::dlsym(fFragmentP, inSymbolName);
}
