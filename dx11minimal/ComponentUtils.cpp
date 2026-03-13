#include "componentutils.h"


XMMATRIX GetWorldMatrix(Transform worldTransform) {
	XMMATRIX rotateMatrix = worldTransform.mRotation;
	XMMATRIX scaleMatrix = XMMatrixScaling(worldTransform.scale.x, worldTransform.scale.y, worldTransform.scale.z);
	XMMATRIX translateMatrix = XMMatrixTranslation(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z);

	XMMATRIX srMatrix = scaleMatrix * rotateMatrix;
	XMMATRIX worldMatrix = srMatrix * translateMatrix;

	return XMMatrixTranspose(worldMatrix);
}