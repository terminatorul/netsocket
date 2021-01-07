#if !defined(WINSOCK2_CXX_SOCKET_EVENT)
#define WINSOCK2_CXX_SOCKET_EVENT

#include <WinSock2.h>
#include <Windows.h>

#include <exception>
#include <utility>

#include "SocketLibrary.hpp"
#include "SocketError.hpp"
#include "SocketEventHandle.hpp"

namespace netsocket
{
    class Event: public EventHandle
    {
    public:
	~Event();
	EventHandle handle();
	Event(Library &socketLib);
    };
}

inline netsocket::Event::~Event()
{
    BOOL bEventClosed = wsa_fn_call<FALSE>(::WSACloseEvent, hEvent);

    if (!bEventClosed && !std::uncaught_exceptions())
	raiseError();
}

inline netsocket::EventHandle netsocket::Event::handle()
{
    return std::move(EventHandle(hEvent));
}

inline netsocket::Event::Event(Library &)
    : EventHandle(wsa_call<WSA_INVALID_EVENT>(::WSACreateEvent))
{
}

#endif // !defined(WINSOCK2_CXX_SOCKET_EVENT)
