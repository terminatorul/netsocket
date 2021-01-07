#if !defined(WINSOCK2_CXX_SOCKET_EVENT_HANDLE)
#define WINSOCK2_CXX_SOCKET_EVENT_HANDLE

#include <WinSock2.h>

#include "SocketLibrary.hpp"

namespace netsocket
{
    class EventHandle
    {
	friend class Library;
	friend class EventLoop;
	friend class Event;
	friend class OverlappedHandler;

    protected:
	WSAEVENT hEvent;

	EventHandle(WSAEVENT hEvent);
	operator WSAEVENT() const;

    public:
	void set();
	EventHandle &operator ++();
	EventHandle  operator ++(int);

	void reset();
	EventHandle &operator --();
	EventHandle  operator --(int);

	EventHandle(EventHandle const &other) = default;
	EventHandle(EventHandle &&other) = default;
	EventHandle &operator =(EventHandle const &other) = default;
	EventHandle &operator =(EventHandle &&other) = default;
    };
}

inline netsocket::EventHandle::EventHandle(WSAEVENT hEvent)
    : hEvent(hEvent)
{
}

inline netsocket::EventHandle::operator WSAEVENT() const
{
    return hEvent;
}

inline void netsocket::EventHandle::set()
{
    wsa_call<FALSE>(::WSASetEvent, hEvent);
}

inline netsocket::EventHandle &netsocket::EventHandle::operator ++()
{
    set();
    return *this;
}

inline netsocket::EventHandle netsocket::EventHandle::operator ++(int)
{
    set();
    return *this;
}

inline void netsocket::EventHandle::reset()
{
    wsa_call<FALSE>(::WSAResetEvent, hEvent);
}

inline netsocket::EventHandle & netsocket::EventHandle::operator --()
{
    reset();
    return *this;
}

inline netsocket::EventHandle netsocket::EventHandle::operator --(int)
{
    reset();
    return *this;
}

#endif // !defined(WINSOCK2_CXX_SOCKET_EVENT_HANDLE)