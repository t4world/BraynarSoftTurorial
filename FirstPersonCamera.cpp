#include "FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera()
{
}

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3Transform(defaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);
	XMMATRIX tempRotationY = XMMatrixRotationY(camYaw);
	camRight = XMVector3Transform(defaultRight, tempRotationY);
	camForward = XMVector3Transform(defaultForward, tempRotationY);

	camPosition += moveLeftRight * camRight;
	camPosition += moveBackForward *camForward;

	camTarget = camPosition + camTarget;

	viewMatrix = XMMatrixLookAtLH(camPosition, camTarget, camUp);

	moveLeftRight = 0;
	moveBackForward = 0;

}
