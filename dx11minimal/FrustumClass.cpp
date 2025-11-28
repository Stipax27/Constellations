////////////////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "frustumclass.h"


FrustumClass::FrustumClass()
{
}


FrustumClass::FrustumClass(const FrustumClass& other)
{
}


FrustumClass::~FrustumClass()
{
}


void FrustumClass::ConstructFrustum(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float screenDepth) {
    XMMATRIX finalMatrix;
    XMFLOAT4X4 projMatrix, matrix;
    float zMinimum, r, t;


    // Load the projection matrix into a XMFLOAT4X4 structure.
    XMStoreFloat4x4(&projMatrix, projectionMatrix);

    // Calculate the minimum Z distance in the frustum.
    zMinimum = -projMatrix._43 / projMatrix._33;
    r = screenDepth / (screenDepth - zMinimum);
    projMatrix._33 = r;
    projMatrix._43 = -r * zMinimum;

    // Load the updated XMFLOAT4X4 back into the original projection matrix.
    projectionMatrix = XMLoadFloat4x4(&projMatrix);

    // Create the frustum matrix from the view matrix and updated projection matrix.
    finalMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

    // Load the final matrix into a XMFLOAT4X4 structure.
    XMStoreFloat4x4(&matrix, finalMatrix);

    // Get the near plane of the frustum.
    planes[0].x = matrix._13;
    planes[0].y = matrix._23;
    planes[0].z = matrix._33;
    planes[0].w = matrix._43;

    // Normalize it.
    t = (float)sqrt((planes[0].x * planes[0].x) + (planes[0].y * planes[0].y) + (planes[0].z * planes[0].z));
    planes[0].x /= t;
    planes[0].y /= t;
    planes[0].z /= t;
    planes[0].w /= t;

    // Calculate the far plane of frustum.
    planes[1].x = matrix._14 - matrix._13;
    planes[1].y = matrix._24 - matrix._23;
    planes[1].z = matrix._34 - matrix._33;
    planes[1].w = matrix._44 - matrix._43;

    // Normalize it.
    t = (float)sqrt((planes[1].x * planes[1].x) + (planes[1].y * planes[1].y) + (planes[1].z * planes[1].z));
    planes[1].x /= t;
    planes[1].y /= t;
    planes[1].z /= t;
    planes[1].w /= t;

    // Calculate the left plane of frustum.
    planes[2].x = matrix._14 + matrix._11;
    planes[2].y = matrix._24 + matrix._21;
    planes[2].z = matrix._34 + matrix._31;
    planes[2].w = matrix._44 + matrix._41;

    // Normalize it.
    t = (float)sqrt((planes[2].x * planes[2].x) + (planes[2].y * planes[2].y) + (planes[2].z * planes[2].z));
    planes[2].x /= t;
    planes[2].y /= t;
    planes[2].z /= t;
    planes[2].w /= t;

    // Calculate the right plane of frustum.
    planes[3].x = matrix._14 - matrix._11;
    planes[3].y = matrix._24 - matrix._21;
    planes[3].z = matrix._34 - matrix._31;
    planes[3].w = matrix._44 - matrix._41;

    // Normalize it.
    t = (float)sqrt((planes[3].x * planes[3].x) + (planes[3].y * planes[3].y) + (planes[3].z * planes[3].z));
    planes[3].x /= t;
    planes[3].y /= t;
    planes[3].z /= t;
    planes[3].w /= t;

    // Calculate the top plane of frustum.
    planes[4].x = matrix._14 - matrix._12;
    planes[4].y = matrix._24 - matrix._22;
    planes[4].z = matrix._34 - matrix._32;
    planes[4].w = matrix._44 - matrix._42;

    // Normalize it.
    t = (float)sqrt((planes[4].x * planes[4].x) + (planes[4].y * planes[4].y) + (planes[4].z * planes[4].z));
    planes[4].x /= t;
    planes[4].y /= t;
    planes[4].z /= t;
    planes[4].w /= t;

    // Calculate the bottom plane of frustum.
    planes[5].x = matrix._14 + matrix._12;
    planes[5].y = matrix._24 + matrix._22;
    planes[5].z = matrix._34 + matrix._32;
    planes[5].w = matrix._44 + matrix._42;

    // Normalize it.
    t = (float)sqrt((planes[5].x * planes[5].x) + (planes[5].y * planes[5].y) + (planes[5].z * planes[5].z));
    planes[5].x /= t;
    planes[5].y /= t;
    planes[5].z /= t;
    planes[5].w /= t;
}


bool FrustumClass::CheckPoint(point3d pos) {
    int i;


    // Check if the point is inside all six planes of the view frustum.
    for (i = 0; i < 6; i++)
    {
        if (((planes[i].x * pos.x) + (planes[i].y * pos.y) + (planes[i].z * pos.z) + planes[i].w) < 0.0f)
        {
            return false;
        }
    }

    return true;
}


bool FrustumClass::CheckCube(point3d center, float radius) {
    int i;


    // Check if any one point of the cube is in the view frustum.
    for (i = 0; i < 6; i++)
    {
        if (planes[i].x * (center.x - radius) +
            planes[i].y * (center.y - radius) +
            planes[i].z * (center.z - radius) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x + radius) +
            planes[i].y * (center.y - radius) +
            planes[i].z * (center.z - radius) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x - radius) +
            planes[i].y * (center.y + radius) +
            planes[i].z * (center.z - radius) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x + radius) +
            planes[i].y * (center.y + radius) +
            planes[i].z * (center.z - radius) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x - radius) +
            planes[i].y * (center.y - radius) +
            planes[i].z * (center.z + radius) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x + radius) +
            planes[i].y * (center.y - radius) +
            planes[i].z * (center.z + radius) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x - radius) +
            planes[i].y * (center.y + radius) +
            planes[i].z * (center.z + radius) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x + radius) +
            planes[i].y * (center.y + radius) +
            planes[i].z * (center.z + radius) + planes[i].w >= 0.0f)
        {
            continue;
        }

        return false;
    }

    return true;
}


bool FrustumClass::CheckSphere(point3d center, float radius) {
    int i;

    // Check if the radius of the sphere is inside the view frustum.
    for (i = 0; i < 6; i++)
    {
        if (((planes[i].x * center.x) + (planes[i].y * center.y) + (planes[i].z * center.z) + planes[i].w) < -radius)
        {
            return false;
        }
    }

    return true;
}


bool FrustumClass::CheckRectangle(point3d center, point3d size) {
    int i;


    // Check if any of the 6 planes of the rectangle are inside the view frustum.
    for (i = 0; i < 6; i++)
    {
        if (planes[i].x * (center.x - size.x) +
            planes[i].y * (center.y - size.y) +
            planes[i].z * (center.z - size.z) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x + size.x) +
            planes[i].y * (center.y - size.y) +
            planes[i].z * (center.z - size.z) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x - size.x) +
            planes[i].y * (center.y + size.y) +
            planes[i].z * (center.z - size.z) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x - size.x) +
            planes[i].y * (center.y - size.y) +
            planes[i].z * (center.z + size.z) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x + size.x) +
            planes[i].y * (center.y + size.y) +
            planes[i].z * (center.z - size.z) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x + size.x) +
            planes[i].y * (center.y - size.y) +
            planes[i].z * (center.z + size.z) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x - size.x) +
            planes[i].y * (center.y + size.y) +
            planes[i].z * (center.z + size.z) + planes[i].w >= 0.0f)
        {
            continue;
        }

        if (planes[i].x * (center.x + size.x) +
            planes[i].y * (center.y + size.y) +
            planes[i].z * (center.z + size.z) + planes[i].w >= 0.0f)
        {
            continue;
        }

        return false;
    }

    return true;
}