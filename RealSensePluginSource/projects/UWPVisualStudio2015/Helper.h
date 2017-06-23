#pragma once
#include <Windows.h>
#include <iostream>

typedef void(__stdcall * DebugCallback) (const char * str);
extern DebugCallback gDebugCallback;
typedef void(__stdcall * CreateTextureCallback) ();
extern CreateTextureCallback gCreateTextureCallback;

extern "C" VOID __declspec(dllexport) RegisterDebugCallback(DebugCallback callback);

extern "C" VOID __declspec(dllexport) RegisterCreateTextureCallback(CreateTextureCallback callback);

void DebugInUnity(std::string message);

void UpdateVideoPropertiesInUnity();

ULONG GetRefCount(IUnknown* pObj);