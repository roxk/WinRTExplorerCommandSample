#pragma once

#include <shobjidl_core.h>
#include <shlwapi.h>
#include "ExplorerCommandBase.g.h"
#include "SubCommand.g.h"
#include <wil/resource.h>
#include <wil/filesystem.h>
#include <filesystem>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Resources.h>
#include <sstream>

namespace winrt::CommandFactory::implementation
{
    struct SubMenu : winrt::implements<SubMenu, IEnumExplorerCommand>
    {
        SubMenu(Windows::Foundation::Collections::IVectorView<CommandFactory::ExplorerCommandBase> commands) : mCommands(std::move(commands))
        {
        }

        // IEnumExplorerCommand
        IFACEMETHODIMP Next(ULONG celt, __out_ecount_part(celt, *pceltFetched) IExplorerCommand** apUICommand, __out_opt ULONG* pceltFetched)
        {
            const auto oldIndex = mIndex;
            const auto endIndex = mIndex + celt;
            const auto commandCount = mCommands.Size();
            for (; mIndex < endIndex && mIndex < commandCount; mIndex++)
            {
                mCommands.GetAt(mIndex).try_as<IExplorerCommand>().copy_to(apUICommand);
            }

            const auto fetched = mIndex - oldIndex;
            wil::assign_to_opt_param(pceltFetched, static_cast<ULONG>(fetched));
            return (fetched == celt) ? S_OK : S_FALSE;
        }

        IFACEMETHODIMP Skip(ULONG /*celt*/) { return E_NOTIMPL; }
        IFACEMETHODIMP Reset()
        {
            mIndex = 0;
            return S_OK;
        }
        IFACEMETHODIMP Clone(__deref_out IEnumExplorerCommand** ppenum) { *ppenum = nullptr; return E_NOTIMPL; }

    private:
        Windows::Foundation::Collections::IVectorView<CommandFactory::ExplorerCommandBase> mCommands;
        uint32_t mIndex{};
    };

    struct ExplorerCommandBase : ExplorerCommandBaseT<ExplorerCommandBase, IExplorerCommand>
    {
        ExplorerCommandBase() = default;
        hstring Title() { return L"Forgot to override Title?"; }
        hstring Icon() { return L""; }
        ExplorerCommandState State() { return ExplorerCommandState::Enabled; }
        virtual Windows::Foundation::Collections::IVectorView<CommandFactory::ExplorerCommandBase> SubCommands();
        void Execute() {}

        IFACEMETHODIMP GetTitle(_In_opt_ IShellItemArray* items, _Outptr_result_nullonfailure_ PWSTR* name)
        {
            return SHStrDup(Title().c_str(), name);
        }
        IFACEMETHODIMP GetIcon(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* icon)
        {
            return SHStrDup(Icon().c_str(), icon);
        }
        IFACEMETHODIMP GetToolTip(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* infoTip) { *infoTip = nullptr; return E_NOTIMPL; }
        IFACEMETHODIMP GetCanonicalName(_Out_ GUID* guidCommandName) { *guidCommandName = GUID_NULL;  return S_OK; }
        IFACEMETHODIMP GetState(_In_opt_ IShellItemArray* selection, _In_ BOOL okToBeSlow, _Out_ EXPCMDSTATE* cmdState)
        {
            *cmdState = static_cast<EXPCMDSTATE>(State());
            return S_OK;
        }
        IFACEMETHODIMP Invoke(_In_opt_ IShellItemArray* selection, _In_opt_ IBindCtx*) noexcept try
        {
            constexpr auto uuid = winrt::guid_of<ExplorerCommandBase>();
            Execute();
            return S_OK;
        }
        CATCH_RETURN()

        IFACEMETHODIMP GetFlags(_Out_ EXPCMDFLAGS* flags)
        {
            *flags = !HasSubCommands() ? ECF_DEFAULT : ECF_HASSUBCOMMANDS;
            return S_OK;
        }
        IFACEMETHODIMP EnumSubCommands(_COM_Outptr_ IEnumExplorerCommand** enumCommands)
        {
            *enumCommands = nullptr;
            if (HasSubCommands())
            {
                auto subMenu = make<SubMenu>(SubCommands());
                auto result = subMenu.as(IID_PPV_ARGS(enumCommands));
                return result;
            }
            return E_NOTIMPL;
        }
        bool HasSubCommands()
        {
            auto subCommands = SubCommands();
            return subCommands != nullptr && subCommands.Size() > 0;
        }
    };

    struct SubCommand : SubCommandT<SubCommand, ExplorerCommandBase>
    {
        Windows::Foundation::Collections::IVectorView<CommandFactory::ExplorerCommandBase> SubCommands()
        {
            return nullptr;
        }
    };
}

namespace winrt::CommandFactory::factory_implementation
{
    struct ExplorerCommandBase : ExplorerCommandBaseT<ExplorerCommandBase, implementation::ExplorerCommandBase>
    {
    };
}
