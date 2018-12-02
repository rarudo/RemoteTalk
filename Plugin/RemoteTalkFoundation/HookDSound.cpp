#include "pch.h"
#include "Hook.h"
#include "HookKernel.h"
#include "HookDSound.h"


static std::vector<DSoundHandlerBase*> g_dsoundhandlers;
#define Call(Name, ...) for(auto *handler : g_dsoundhandlers) { handler->Name(__VA_ARGS__); }

static void SetHook_IDirectSound8(IDirectSound8 *dst);
static void SetHook_IDirectSoundBuffer(IDirectSoundBuffer *dst);

static HRESULT(WINAPI *DirectSoundEnumerateA_orig)(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext);
static HRESULT WINAPI DirectSoundEnumerateA_hook(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext)
{
    auto ret = DirectSoundEnumerateA_orig(pDSEnumCallback, pContext);
    return ret;
}

static HRESULT(WINAPI *DirectSoundCreate_orig)(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
static HRESULT WINAPI DirectSoundCreate_hook(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
    auto ret = DirectSoundCreate_orig(pcGuidDevice, ppDS, pUnkOuter);
    Call(afterDirectSoundCreate, pcGuidDevice, ppDS, pUnkOuter, ret);
    return ret;
}

static HRESULT(WINAPI *DirectSoundCreate8_orig)(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter);
static HRESULT WINAPI DirectSoundCreate8_hook(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter)
{
    auto ret = DirectSoundCreate8_orig(pcGuidDevice, ppDS8, pUnkOuter);
    SetHook_IDirectSound8(*ppDS8);
    Call(afterDirectSoundCreate8, pcGuidDevice, ppDS8, pUnkOuter, ret);
    return ret;
}

static HRESULT(WINAPI *DirectSoundFullDuplexCreate_orig)(LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice, LPCDSCBUFFERDESC pcDSCBufferDesc, LPCDSBUFFERDESC pcDSBufferDesc, HWND hWnd, DWORD dwLevel, LPDIRECTSOUNDFULLDUPLEX* ppDSFD, LPDIRECTSOUNDCAPTUREBUFFER8 *ppDSCBuffer8, LPDIRECTSOUNDBUFFER8 *ppDSBuffer8, LPUNKNOWN pUnkOuter);
static HRESULT WINAPI DirectSoundFullDuplexCreate_hook(LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice, LPCDSCBUFFERDESC pcDSCBufferDesc, LPCDSBUFFERDESC pcDSBufferDesc, HWND hWnd, DWORD dwLevel, LPDIRECTSOUNDFULLDUPLEX* ppDSFD, LPDIRECTSOUNDCAPTUREBUFFER8 *ppDSCBuffer8, LPDIRECTSOUNDBUFFER8 *ppDSBuffer8, LPUNKNOWN pUnkOuter)
{
    auto ret = DirectSoundFullDuplexCreate_orig(pcGuidCaptureDevice, pcGuidRenderDevice, pcDSCBufferDesc, pcDSBufferDesc, hWnd, dwLevel, ppDSFD, ppDSCBuffer8, ppDSBuffer8, pUnkOuter);
    //Call(afterDirectSoundCreate8, pcGuidDevice, ppDS8, pUnkOuter, ret);
    return ret;
}


// IDirectSound8
static HRESULT(WINAPI *IDirectSound8_CreateSoundBuffer_orig)(IDirectSound8 *_this, LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter);
static HRESULT WINAPI IDirectSound8_CreateSoundBuffer_hook(IDirectSound8 *_this, LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter)
{
    auto ret = IDirectSound8_CreateSoundBuffer_orig(_this, pcDSBufferDesc, ppDSBuffer, pUnkOuter);
    SetHook_IDirectSoundBuffer(*ppDSBuffer);
    Call(afterIDirectSound8_CreateSoundBuffer, _this, pcDSBufferDesc, ppDSBuffer, pUnkOuter, ret);
    return ret;
}

static void SetHook_IDirectSound8(IDirectSound8 *dst)
{
    if (!dst || IDirectSound8_CreateSoundBuffer_orig != nullptr)
        return;

    void **&vtable = ((void***)dst)[0];

    (void*&)IDirectSound8_CreateSoundBuffer_orig = vtable[3];
    ForceWrite(vtable[3], (void*)&IDirectSound8_CreateSoundBuffer_hook);
}


// IDirectSoundBuffer

static HRESULT(WINAPI *IDirectSoundBuffer_Lock_orig)(IDirectSoundBuffer *_this, DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags);
static HRESULT WINAPI IDirectSoundBuffer_Lock_hook(IDirectSoundBuffer *_this, DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags)
{
    auto ret = IDirectSoundBuffer_Lock_orig(_this, dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags);
    Call(afterIDirectSoundBuffer_Lock, _this, dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags, ret);
    return ret;
}

static HRESULT(WINAPI *IDirectSoundBuffer_Play_orig)(IDirectSoundBuffer *_this, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags);
static HRESULT WINAPI IDirectSoundBuffer_Play_hook(IDirectSoundBuffer *_this, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags)
{
    auto ret = IDirectSoundBuffer_Play_orig(_this, dwReserved1, dwPriority, dwFlags);
    Call(afterIDirectSoundBuffer_Play, _this, dwReserved1, dwPriority, dwFlags, ret);
    return ret;
}

static HRESULT(WINAPI *IDirectSoundBuffer_SetCurrentPosition_orig)(IDirectSoundBuffer *_this, DWORD dwNewPosition);
static HRESULT WINAPI IDirectSoundBuffer_SetCurrentPosition_hook(IDirectSoundBuffer *_this, DWORD dwNewPosition)
{
    auto ret = IDirectSoundBuffer_SetCurrentPosition_orig(_this, dwNewPosition);
    Call(afterIDirectSoundBuffer_SetCurrentPosition, _this, dwNewPosition, ret);
    return ret;
}

static HRESULT(WINAPI *IDirectSoundBuffer_SetFormat_orig)(IDirectSoundBuffer *_this, LPCWAVEFORMATEX pcfxFormat);
static HRESULT WINAPI IDirectSoundBuffer_SetFormat_hook(IDirectSoundBuffer *_this, LPCWAVEFORMATEX pcfxFormat)
{
    auto ret = IDirectSoundBuffer_SetFormat_orig(_this, pcfxFormat);
    Call(afterIDirectSoundBuffer_SetFormat, _this, pcfxFormat, ret);
    return ret;
}

static HRESULT(WINAPI *IDirectSoundBuffer_SetVolume_orig)(IDirectSoundBuffer *_this, LONG lVolume);
static HRESULT WINAPI IDirectSoundBuffer_SetVolume_hook(IDirectSoundBuffer *_this, LONG lVolume)
{
    auto ret = IDirectSoundBuffer_SetVolume_orig(_this, lVolume);
    Call(afterIDirectSoundBuffer_SetVolume, _this, lVolume, ret);
    return ret;
}

static HRESULT(WINAPI *IDirectSoundBuffer_SetPan_orig)(IDirectSoundBuffer *_this, LONG lPan);
static HRESULT WINAPI IDirectSoundBuffer_SetPan_hook(IDirectSoundBuffer *_this, LONG lPan)
{
    auto ret = IDirectSoundBuffer_SetPan_orig(_this, lPan);
    Call(afterIDirectSoundBuffer_SetPan, _this, lPan, ret);
    return ret;
}

static HRESULT(WINAPI *IDirectSoundBuffer_SetFrequency_orig)(IDirectSoundBuffer *_this, DWORD dwFrequency);
static HRESULT WINAPI IDirectSoundBuffer_SetFrequency_hook(IDirectSoundBuffer *_this, DWORD dwFrequency)
{
    auto ret = IDirectSoundBuffer_SetFrequency_orig(_this, dwFrequency);
    Call(afterIDirectSoundBuffer_SetFrequency, _this, dwFrequency, ret);
    return ret;
}

static HRESULT(WINAPI *IDirectSoundBuffer_Stop_orig)(IDirectSoundBuffer *_this);
static HRESULT WINAPI IDirectSoundBuffer_Stop_hook(IDirectSoundBuffer *_this)
{
    auto ret = IDirectSoundBuffer_Stop_orig(_this);
    Call(afterIDirectSoundBuffer_Stop, _this, ret);
    return ret;
}

static HRESULT(WINAPI *IDirectSoundBuffer_Unlock_orig)(IDirectSoundBuffer *_this, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2);
static HRESULT WINAPI IDirectSoundBuffer_Unlock_hook(IDirectSoundBuffer *_this, LPVOID pvAudioPtr1, DWORD dwAudioBytes1, LPVOID pvAudioPtr2, DWORD dwAudioBytes2)
{
    Call(beforeIDirectSoundBuffer_Unlock, _this, pvAudioPtr1, dwAudioBytes1, pvAudioPtr2, dwAudioBytes2);
    auto ret = IDirectSoundBuffer_Unlock_orig(_this, pvAudioPtr1, dwAudioBytes1, pvAudioPtr2, dwAudioBytes2);
    Call(afterIDirectSoundBuffer_Unlock, _this, pvAudioPtr1, dwAudioBytes1, pvAudioPtr2, dwAudioBytes2, ret);
    return ret;
}

static HRESULT(WINAPI *IDirectSoundBuffer_Restore_orig)(IDirectSoundBuffer *_this);
static HRESULT WINAPI IDirectSoundBuffer_Restore_hook(IDirectSoundBuffer *_this)
{
    auto ret = IDirectSoundBuffer_Restore_orig(_this);
    Call(afterIDirectSoundBuffer_Restore, _this, ret);
    return ret;
}

static void SetHook_IDirectSoundBuffer(IDirectSoundBuffer *dst)
{
    if (!dst || IDirectSoundBuffer_Lock_orig != nullptr)
        return;

    void **&vtable = ((void***)dst)[0];

#define Hook(Name, N)\
    (void*&)IDirectSoundBuffer_##Name##_orig = vtable[N];\
    ForceWrite(vtable[N], (void*)&IDirectSoundBuffer_##Name##_hook);

    Hook(Lock, 11);
    Hook(Play, 12);
    Hook(SetCurrentPosition, 13);
    Hook(SetFormat, 14);
    Hook(SetVolume, 15);
    Hook(SetPan, 16);
    Hook(SetFrequency, 17);
    Hook(Stop, 18);
    Hook(Unlock, 19);
    Hook(Restore, 20);
#undef Hook
}



#define EachFunctions(Body)\
    Body(DirectSoundEnumerateA);\
    Body(DirectSoundCreate);\
    Body(DirectSoundCreate8);\
    Body(DirectSoundFullDuplexCreate);

static const char DSound_DLL[] = "dsound.dll";

class LoadLibraryHandler_DSound : public LoadLibraryHandlerBase
{
public:
    void afterLoadLibrary(HMODULE& mod) override
    {
        hook(mod);
    }

    static void hook(HMODULE mod)
    {
        if (!IsValidModule(mod))
            return;
#define Override(Name) OverrideIAT(mod, DSound_DLL, #Name, Name##_hook)
        EachFunctions(Override);
#undef Override
    }
} static g_loadlibraryhandler_dsound;

class CoCreateHandler_DSound : public CoCreateHandlerBase
{
public:
    void afterCoCreateInstance(REFCLSID rclsid, LPUNKNOWN& /*pUnkOuter*/, DWORD& /*dwClsContext*/, REFIID /*riid*/, LPVOID *&ppv, HRESULT& ret) override
    {
        static const GUID CLSID_DirectSound_ = { 0x47d4d946, 0x62e8, 0x11cf, 0x93, 0xbc, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 };
        static const GUID CLSID_DirectSound8_ = { 0x3901cc3f, 0x84b5, 0x4fa4, 0xba, 0x35, 0xaa, 0x81, 0x72, 0xb8, 0xa0, 0x9b };

        if (rclsid == CLSID_DirectSound_) {
            Call(afterCCIDirectSound, (LPDIRECTSOUND*&)ppv, ret);
        }
        else if (rclsid == CLSID_DirectSound8_) {
            Call(afterCCIDirectSound8, (LPDIRECTSOUND8*&)ppv, ret);
            SetHook_IDirectSound8(*(LPDIRECTSOUND8*)ppv);
        }
    }
} static g_cocreatehandler_dsound;

bool AddDSoundHandler(DSoundHandlerBase *handler, bool load_dll)
{
    g_dsoundhandlers.push_back(handler);

    // setup hooks
    auto dsound = load_dll ? ::LoadLibraryA(DSound_DLL) : ::GetModuleHandleA(DSound_DLL);
    if (!dsound)
        return false;

    if (!DirectSoundEnumerateA_orig) {
        auto jumptable = AllocExecutableForward(1024, dsound);
#define Override(Name) (void*&)Name##_orig = OverrideEAT(dsound, #Name, Name##_hook, jumptable)
        EachFunctions(Override);
#undef Override

        AddLoadLibraryHandler(&g_loadlibraryhandler_dsound);
        AddCoCreateHandler(&g_cocreatehandler_dsound);
        EnumerateModules([](HMODULE mod) { LoadLibraryHandler_DSound::hook(mod); });
    }
    return true;
}
