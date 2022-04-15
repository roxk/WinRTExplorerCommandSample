#include "pch.h"
#include <winrt/Windows.ApplicationModel.Core.h>
#include "ExplorerCommandBase.h"
#include <mutex>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;

bool __stdcall winrt_can_unload_now() noexcept;

struct Factory : implements<Factory, IClassFactory>
{
    HRESULT __stdcall CreateInstance(
        IUnknown* outer,
        GUID const& iid,
        void** result) noexcept final
    {
        *result = nullptr;

        if (outer)
        {
            return CLASS_E_NOAGGREGATION;
        }

        auto command = make_self<CommandFactory::implementation::ExplorerCommandBase>();
        auto convertResult = command.as(guid_of<IExplorerCommand>(), result);
        return S_OK;
    }

    HRESULT __stdcall LockServer(BOOL) noexcept final
    {
        return S_OK;
    }
};

STDAPI DllCanUnloadNow()
{
    return winrt_can_unload_now() ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _COM_Outptr_ void** instance)
{
    return make<Factory>().as(riid, instance);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}