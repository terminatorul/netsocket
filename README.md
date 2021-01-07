# netsocket

C++ library to provide bindings for a socket event loop based on WSAEVENTs in WinSock2 API. Intended to supoprt the use of Overlapped Operations in WinSock2 API.
Includes:

	- Event and EventHandler classes for `WSAEVENT` objects
	- EventLoop class based on `::WSAWaitForMultipleObjects()`
	- Specific error class based on `::WSAGetLastError()` values, with a C++ `std::system_error` base class
		- all errors in the library are also derived from netsocket::BaseException
	- A wrapper class EventLibrary, around `::WSAStartup()` / `::WSACleanup()` calls.

## Building
The source files are provided in a Visual Studio 2019 project file. No solution file provided to hold the project, create one yourself if needed.

## Installing
C++17 is required. Add the project to your own solution file, or manually copy the source files to your source directory.
Link with Ws2_32.lib and make sure to always include <WinSock2.h> before <Windows.h> throughout your project.