#ifndef _ASSIMP_H_
#define _ASSIMP_H_

#include "windows.h"

typedef HRESULT(CALLBACK* LPFNDLLFUNC1)(DWORD, UINT*);

HRESULT LoadAssimpFunctions(DWORD dwParam1, UINT* puParam2)
{
//    HINSTANCE hDLL;               // Handle to DLL
//    LPFNDLLFUNC1 lpfnDllFunc1;    // Function pointer
//    HRESULT hrReturnVal;
//
//    hDLL = LoadLibrary("MyDLL");
//    if (NULL != hDLL)
//    {
//        lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL, "DLLFunc1");
//        if (NULL != lpfnDllFunc1)
//        {
//            // call the function
//            hrReturnVal = lpfnDllFunc1(dwParam1, puParam2);
//        }
//        else
//        {
//            // report the error
//            hrReturnVal = ERROR_DELAY_LOAD_FAILED;
//        }
//        FreeLibrary(hDLL);
//    }
//    else
//    {
//        hrReturnVal = ERROR_DELAY_LOAD_FAILED;
//    }
    return NULL;
}

#endif // !_ASSIMP_H_
