# WinRT IExplorerCommand Implementation Sample

## Overview

This sample provides a basic implementation of `IExplorerCommand` completely in WinRT, getting rid of most of the boilerplate in WRL. To simulate an app providing an explorer command, a UWP app is packaged in a WAP, together with a full trust DLL which implements the COM server. The command is then declared in the manifest of the WAP project.

## IDL And Classic COM

Runtimeclass can implement classic COM interface without declaring them in `.idl`. As long as the class instantiated by the COM server has the required interface (i.e. `IExplorerCommand`), it just works™️.

See [Author COM components](https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/author-coclasses) for more details.

## COM Server

This sample only declare one top-level explorer command in the manifest. Thus, for simplicity, the COM server just return `ExplorerCommandBase` directly. In a real scenario where an app exposes several commands, the COM server would need to instantiate different classes based on the requested clsid.

Since the cppwinrt activation factory `WINRT_GetActivationFactory` already does most of the heavy lifting, a simple clsid to runtime class name map will do. Maybe cppwinrt could be extended to generate this mapping in the future.

### CLSID In The Manifest Can Be Ad-hoc In WinRT

The clsid in the manifest is just a random uuid. The COM server just cast to `IExplorerCommand` and it just works.

## Future Work

While the API `IExplorerCommand` works, the style is old (e.g. still using out parameter and returning `HRESULT` manually), and require tons of boilerplate by modern standard. The `ExplorerCommandBase` class in this sample can be extended to completely wrap `IExplorerCommand` and provided as a simple nuget package. Implementing `IExplorerCommand` would then be as simple as adding a dependency and overriding `ExplorerCommandBase` methods with all the WinRT goodies.

TODO: Come up with a good way to handle COM server boilerplate. Project template? How about the clsid to runtime class name mapping?
