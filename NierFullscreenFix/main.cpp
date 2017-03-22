#include <Windows.h>
#include "MinHook.h"
#include "dxgitype.h"

typedef UINT DXGI_USAGE;
typedef UINT DXGI_SWAP_EFFECT;
typedef struct DXGI_SWAP_CHAIN_DESC
{
	DXGI_MODE_DESC BufferDesc;
	DXGI_SAMPLE_DESC SampleDesc;
	DXGI_USAGE BufferUsage;
	UINT BufferCount;
	HWND OutputWindow;
	BOOL Windowed;
	DXGI_SWAP_EFFECT SwapEffect;
	UINT Flags;
} 	DXGI_SWAP_CHAIN_DESC;

typedef HRESULT(__stdcall* _CreateDXGIFactory)(REFIID riid, void **ppFactory);
typedef HRESULT(__stdcall* _CreateSwapChain)(void *that, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, void **ppSwapChain);

_CreateSwapChain CreateSwapChain_Original;

HMODULE GetDXGIModule()
{
	static HMODULE module = NULL;
	if (module)
		return module;

	wchar_t path[MAX_PATH];
	GetWindowsDirectory(path, MAX_PATH);
	wcsncat_s(path, L"\\System32\\dxgi.dll", MAX_PATH);
	module = LoadLibrary(path);
	return module;
}

HRESULT STDMETHODCALLTYPE CreateSwapChain_Hook(void *that, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, void **ppSwapChain)
{
	pDesc->BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	return CreateSwapChain_Original(that, pDevice, pDesc, ppSwapChain);
}

HRESULT WINAPI CreateDXGIFactory(REFIID riid, _Out_ void **ppFactory)
{
	static _CreateDXGIFactory func = NULL;
	if (!func)
		func = (_CreateDXGIFactory)GetProcAddress(GetDXGIModule(), "CreateDXGIFactory");

	HRESULT hr = func(riid, ppFactory);

    if (SUCCEEDED(hr))
    {
        MH_Initialize();
        MH_CreateHookVirtual(*ppFactory, 10, CreateSwapChain_Hook, (void**)&CreateSwapChain_Original);
        MH_EnableHook(MH_ALL_HOOKS);
    }

	return hr;
}

HRESULT WINAPI CreateDXGIFactory1(REFIID riid, _Out_ void **ppFactory)
{
	static _CreateDXGIFactory func = NULL;
	if (!func)
		func = (_CreateDXGIFactory)GetProcAddress(GetDXGIModule(), "CreateDXGIFactory1");

	return func(riid, ppFactory);
}

HRESULT WINAPI CreateDXGIFactory2(REFIID riid, _Out_ void **ppFactory)
{
	static _CreateDXGIFactory func = NULL;
	if (!func)
		func = (_CreateDXGIFactory)GetProcAddress(GetDXGIModule(), "CreateDXGIFactory2");

	return func(riid, ppFactory);
}
