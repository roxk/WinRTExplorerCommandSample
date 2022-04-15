#include "pch.h"
#include "ExplorerCommandBase.h"
#if __has_include("ExplorerCommandBase.g.cpp")
#include "ExplorerCommandBase.g.cpp"
#endif
#if __has_include("SubCommand.g.cpp")
#include "SubCommand.g.cpp"
#endif

namespace winrt::CommandFactory::implementation
{
	Windows::Foundation::Collections::IVectorView<CommandFactory::ExplorerCommandBase> ExplorerCommandBase::SubCommands()
	{
		return single_threaded_vector<CommandFactory::ExplorerCommandBase>({make<SubCommand>(), make<SubCommand>()}).GetView();
	}
}
