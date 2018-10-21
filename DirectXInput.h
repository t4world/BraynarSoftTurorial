#pragma once
#ifndef _DIRECTX_INPUT_
#define _DIRECTX_INPUT_

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#include <dinput.h>
#include <xnamath.h>
class DirectXInput
{
public:
	DirectXInput();
	~DirectXInput();

	bool InitDirectInput(HINSTANCE hInstance, HWND hwnd);
	void DetectInput(double time, HWND hwnd);
	XMMATRIX RotationX();
	XMMATRIX RotationZ();
	float GetScaleX();
	float GetScaleY();
	void Release();
private:
	IDirectInputDevice8* DIKeyboad;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;
	float rotationX;
	float rotationZ;
	float scaleX = 1.0f;
	float scaleY = 1.0f;
};
#endif // !_DIRECTX_INPUT_

