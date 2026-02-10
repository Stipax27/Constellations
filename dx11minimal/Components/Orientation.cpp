#include "Orientation.h"

void Orientation::SetFromTransform(Transform& transform)
{
	forward = transform.GetLookVector();
	right = transform.GetRightVector();
	up = transform.GetUpVector();
}

void Orientation::SetFromDirections(const point3d& f, const point3d& r, const point3d& u)
{
	forward = f;
	right = r;
	up = u;
}

void Orientation::Orthogonalize()
{
	forward = forward.normalized();
	right = (right - forward * forward.dot(right)).normalized();
	up = forward.cross(right);
}