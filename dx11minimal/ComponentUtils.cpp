#include "componentutils.h"


XMMATRIX GetWorldMatrix(Transform worldTransform) {
	XMMATRIX rotateMatrix = worldTransform.mRotation;
	XMMATRIX scaleMatrix = XMMatrixScaling(worldTransform.scale.x, worldTransform.scale.y, worldTransform.scale.z);
	XMMATRIX translateMatrix = XMMatrixTranslation(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z);

	XMMATRIX srMatrix = scaleMatrix * rotateMatrix;
	XMMATRIX worldMatrix = srMatrix * translateMatrix;

	return XMMatrixTranspose(worldMatrix);
}


Transform GetTransformDelta(Transform worldTransform1, Transform worldTransform2) {
	Transform delta = Transform();

	delta.position = worldTransform1.position - worldTransform2.position;
	delta.scale = worldTransform1.scale / worldTransform2.scale;
	delta.mRotation = worldTransform1.mRotation * worldTransform1.mRotation;

	return delta;
}