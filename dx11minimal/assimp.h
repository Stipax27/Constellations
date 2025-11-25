#ifndef _ASSIMP_H_
#define _ASSIMP_H_

#include "windows.h"

namespace Assimp {
	typedef HRESULT(CALLBACK* LPFNDLLFUNC1)(DWORD, UINT*);

	HRESULT LoadAssimpFunctions(DWORD dwParam1, UINT* puParam2);
}


#endif // !_ASSIMP_H_
