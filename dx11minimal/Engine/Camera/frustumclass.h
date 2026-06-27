////////////////////////////////////////////////////////////////////////////////
// Filename: frustumclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FRUSTUMCLASS_H_
#define _FRUSTUMCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "../dx11.h"
#include "../Types/Point3d.h"
#include "../Lib/isingleton.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: FrustumClass
////////////////////////////////////////////////////////////////////////////////
class FrustumClass : public ISingleton
{
public:
    FrustumClass();
    FrustumClass(const FrustumClass&);
    ~FrustumClass();

    void ConstructFrustum(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float screenDepth);

    bool CheckPoint(point3d pos);
    bool CheckCube(point3d pos, float radius);
    bool CheckSphere(point3d pos, float radius);
    bool CheckRectangle(point3d pos, point3d size);

private:
    XMFLOAT4 planes[6];
};

#endif