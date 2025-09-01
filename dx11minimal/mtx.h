#ifndef MTX_H
#define MTX_H

#include "Point4d.h"
#include "DirectXMath.h"

using namespace DirectX;

namespace mtx {
	point4d* SplitMatrix(XMMATRIX matrix) {
		XMVECTOR v1 = matrix.r[0];
		XMVECTOR v2 = matrix.r[1];
		XMVECTOR v3 = matrix.r[2];
		XMVECTOR v4 = matrix.r[3];

		point4d array[4] = {
			point4d(XMVectorGetX(v1), XMVectorGetY(v1), XMVectorGetZ(v1), XMVectorGetW(v1)),
			point4d(XMVectorGetX(v2), XMVectorGetY(v2), XMVectorGetZ(v2), XMVectorGetW(v2)),
			point4d(XMVectorGetX(v3), XMVectorGetY(v3), XMVectorGetZ(v3), XMVectorGetW(v3)),
			point4d(XMVectorGetX(v4), XMVectorGetY(v4), XMVectorGetZ(v4), XMVectorGetW(v4))
		};

		return array;
	}

	XMMATRIX BuildMatrix(point4d p1 = point4d(), point4d p2 = point4d(), point4d p3 = point4d(), point4d p4 = point4d()) {
		return XMMatrixSet(
			p1.x, p1.y, p1.z, p1.w,
			p2.x, p2.y, p2.z, p2.w,
			p3.x, p3.y, p3.z, p3.w,
			p4.x, p4.y, p4.z, p4.w
		);
	}
}

#endif // !MTX_H
