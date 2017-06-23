#include "Helper.h"

DebugCallback gDebugCallback;
CreateTextureCallback gCreateTextureCallback;

extern "C" VOID __declspec(dllexport) RegisterDebugCallback(DebugCallback callback)
{
	if (callback)
	{
		gDebugCallback = callback;
	}
}

extern "C" VOID __declspec(dllexport) RegisterCreateTextureCallback(CreateTextureCallback callback)
{
	if (callback)
	{
		gCreateTextureCallback = callback;
	}
}

void DebugInUnity(std::string message)
{
	if (gDebugCallback)
	{
		gDebugCallback(message.c_str());
	}
}

void UpdateVideoPropertiesInUnity() {
	if (gCreateTextureCallback) {
		gCreateTextureCallback();
	}
}

ULONG GetRefCount(IUnknown* pObj)
{
	pObj->AddRef();
	ULONG	ref = pObj->Release();

	return ref;
}