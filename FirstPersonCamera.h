#pragma once
#ifndef _FIRSTPERSON_CAMERA_
#define  _FIRSTPERSON_CAMERA_
#include <xnamath.h>
class FirstPersonCamera
{
public:
	FirstPersonCamera();
	~FirstPersonCamera();
	void UpdateCamera();

private: 
	XMVECTOR camTarget = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMMATRIX viewMatrix;
	XMMATRIX camRotationMatrix;
	XMMATRIX groundWorld;
	float moveLeftRight = 0.0f;
	float moveBackForward = 0.0f;

	float camYaw = 0.0f;
	float camPitch = 0.0f;


};
#endif

