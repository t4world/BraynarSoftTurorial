#include "DirectXInput.h"



DirectXInput::DirectXInput()
{
}


DirectXInput::~DirectXInput()
{
}

bool DirectXInput::InitDirectInput(HINSTANCE hInstance,HWND hwnd)
{
	HRESULT hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);
	hr = DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboad, NULL);
	hr = DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);
	hr = DIKeyboad->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboad->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	return true;
}

void DirectXInput::DetectInput(double time,HWND hwnd)
{
	DIMOUSESTATE mouseCurrentState;
	BYTE keyboardState[256];
	DIKeyboad->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrentState);
	DIKeyboad->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
	{
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}

	if (keyboardState[DIK_LEFT] & 0x80)
	{
		rotationZ -= 1.0f * time;
	}

	if (keyboardState[DIK_RIGHT] & 0x80)
	{
		rotationZ += 1.0f * time;
	}

	if (keyboardState[DIK_UP] & 0x80)
	{
		rotationX += 1.0f * time;
	}

	if (keyboardState[DIK_DOWN] & 0x80)
	{
		rotationX -= 1.0f * time;
	}

	if (mouseCurrentState.lX != mouseLastState.lX)
	{
		scaleX -= mouseCurrentState.lX * 0.001f;
	}

	if (mouseCurrentState.lY != mouseLastState.lX)
	{
		scaleY -= mouseCurrentState.lY * 0.001f;
	}

	if (rotationX > 6.28)
	{
		rotationX -= 6.28;
	}
	else if (rotationX < 0)
	{
		rotationX += 6.28;
	}

	if (rotationZ > 6.28)
	{
		rotationZ -= 6.28;
	}
	else if (rotationZ < 0)
	{
		rotationZ += 6.28;
	}
	mouseLastState = mouseCurrentState;
}

XMMATRIX DirectXInput::RotationX()
{
	XMVECTOR rotateXAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	return XMMatrixRotationAxis(rotateXAxis, rotationX);
}

XMMATRIX DirectXInput::RotationZ()
{
	XMVECTOR rotateZAxis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	return XMMatrixRotationAxis(rotateZAxis, rotationZ);
}

float DirectXInput::GetScaleX()
{
	return scaleX;
}

float DirectXInput::GetScaleY()
{
	return scaleY;
}

void DirectXInput::Release()
{
	DIKeyboad->Unacquire();
	DIMouse->Unacquire();
	DirectInput->Release();
}
