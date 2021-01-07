#if !defined(WINSOCK2_CXX_SOCKET_LIBRARY)
#define WINSOCK2_CXX_SOCKET_LIBRARY

#include <WinSock2.h>
#include <Windows.h>
#include <cstdint>

#include <utility>
#include <exception>
#include <system_error>

#include "SocketError.hpp"

namespace netsocket
{
    class Library: protected WSADATA
    {
    public:
	std::uint_least16_t maxVersion() const;
	std::uint_least16_t version() const;
	char const *description() const;
	char const *status() const;
 
	~Library();
	Library(std::uint_least8_t versionMajor = 2U, std::uint_least8_t versionMinor = 2U);
    };

    static unsigned	       const MAX_IN_PROGRESS_RETRY_COUNT   = 600;	// 1 min
    static std::uint_least32_t const IN_PROGRESS_SLEEP_INTERVAL_MS = 100;

    template <auto errorResult, typename Result, typename... Args, typename... CallArgs>
        Result wsa_fn_call(Result(*WsaFunction)(Args...), CallArgs... args);

    template <auto errorResult, typename Result, typename... Args, typename... CallArgs>
	Result wsa_call(Result(*WsaFunction)(Args...), CallArgs... args);

    template <typename Result, typename... Args, typename... CallArgs>
	Result wsa_sys_call(Result(*WsaFunction)(Args...), CallArgs... args);

    template <typename Result, typename... Args, typename... CallArgs>
	Result wsa_api_call(Result(*WsaFunction)(Args...), CallArgs... args);
}

inline std::uint_least16_t netsocket::Library::maxVersion() const
{
    return wHighVersion;
}

inline std::uint_least16_t netsocket::Library::version() const
{
    return wVersion;
}

inline char const *netsocket::Library::description() const
{
    return szDescription;
}

inline char const *netsocket::Library::status() const
{
    return szSystemStatus;
}

template <auto errorResult, typename Result, typename... Args, typename... CallArgs>
    Result netsocket::wsa_fn_call(Result (*WsaFunction)(Args...), CallArgs... args)
{
    Result result = WsaFunction(args...);
    auto   retryCount = MAX_IN_PROGRESS_RETRY_COUNT;

    while (errorResult == result && ::WSAGetLastError() == +Error::EInProgress && --retryCount)
    {
	::Sleep(IN_PROGRESS_SLEEP_INTERVAL_MS);
	result = WsaFunction(args...);
    }

    return std::move(result);
}

template <auto errorResult, typename Result, typename... Args, typename... CallArgs>
    Result netsocket::wsa_call(Result(*WsaFunction)(Args...), CallArgs... args)
{
    Result result = wsa_fn_call<errorResult>(WsaFunction, std::forward<CallArgs>(args)...);

    if (errorResult == result)
	raiseError();

    return std::move(result);
}

template <typename Result, typename... Args, typename... CallArgs>
    Result netsocket::wsa_sys_call(Result (*WsaFunction)(Args...), CallArgs... args)
{
    Result wsaError = WsaFunction(args...);
    int retryCount = MAX_IN_PROGRESS_RETRY_COUNT;

    while (+Error::EInProgress == wsaError && --retryCount)
    {
	::Sleep(IN_PROGRESS_SLEEP_INTERVAL_MS);
	wsaError = WsaFunction(args...);
    }

    return std::move(wsaError);
}

template <typename Result, typename... Args, typename... CallArgs>
    Result netsocket::wsa_api_call(Result(*WsaFunction)(Args...), CallArgs... args)
{
    Result result = wsa_sys_call(WsaFunction, std::forward<CallArgs>(args)...);
    checkError<>(result);

    return std::move(result);
}

inline netsocket::Library::Library(std::uint_least8_t versionMajor, std::uint_least8_t versionMinor)
{
    wsa_api_call(::WSAStartup, MAKEWORD(versionMajor, versionMinor), static_cast<WSADATA *>(this));
}

inline netsocket::Library::~Library()
{
    int wsaError = wsa_sys_call(::WSACleanup);

    if (!std::uncaught_exceptions())
	checkError<>(wsaError);
}

#endif // !defined(WINSOCK2_CXX_SOCKET_LIBRARY)