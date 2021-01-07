#include <WinSock2.h>
#include <Windows.h>

#include <cstdint>
#include <string>
#include <utility>
#include <exception>
#include <stdexcept>
#include <system_error>

#include "SocketError.hpp"
#include "SocketEventLoop.hpp"

using std::uint_least32_t;
using std::move;
using std::string;
using std::exception;
using std::runtime_error;

netsocket::BaseException::Tag const
    netsocket::EventLoop::multipleHandlersTag	    { "Multiple handlers in event loop" },
    netsocket::EventLoop::eventCountExceededTag	    { "WinSock2 limit" },
    netsocket::EventLoop::invalidApiReturnValueTag  { "WinSock2 API" },
    netsocket::EventLoop::quitLoopTag		    { "WinSock2 event loop" };

string netsocket::EventLoop::MultipleHandlers::handlerTypeString(HandlerType handlerType)
{
    using namespace std::literals::string_literals;

    switch (handlerType)
    {
    case HandlerType::Event:
	return "event"s;
    case HandlerType::Timer:
	return "timer"s;
    case HandlerType::Exception:
	return "exception"s;
    case HandlerType::Iteration:
	return "iteration"s;
    }

    return move(string());
}

void netsocket::EventLoop::doTimerEvent()
{
    bool keepTimer = false;
    TimerHandler *currentTimer = timer;
    timer = nullptr;

    try
    {
	keepTimer = currentTimer->onTimerTrigger(*this);
    }
    catch (QuitLoop const &)
    {
	loopRunning = false;
    }
    catch (...)
    {
	if (exceptionHandler)
	    exceptionHandler->onException(*this);
    }

    if (keepTimer && currentTimer != freeList)
	addTimerHandler(*currentTimer);
}

void netsocket::EventLoop::doIterationStartEvent()
try
{
    if (iterationHandler)
	iterationHandler->onNextIteration(*this);
}
catch (QuitLoop const&)
{
    loopRunning = false;
}
catch (...)
{
    if (exceptionHandler)
	exceptionHandler->onException(*this);
}

void netsocket::EventLoop::doSocketEvent(unsigned idx)
{
    bool keepHandler = false;
    EventHandler
	*&handler     = handlers[events[idx]],
	* baseHandler = handler;
   
    handler = nullptr;

    try
    {
	keepHandler = baseHandler->onEventTrigger(*this);
    }
    catch (QuitLoop const &)
    {
	loopRunning = false;
    }
    catch (...)
    {
	if (exceptionHandler)
	    exceptionHandler->onException(*this);
    }

    if (keepHandler && freeList != baseHandler)
	if (handler)
	{
	    WSAEVENT wsaEvent = events[idx];
	    events.erase(events.begin() + idx);

	    throw MultipleHandlers(wsaEvent);
	}
	else
	    handler = baseHandler;
    else
    {
	if (!handler)
	    handlers.erase(events[idx]);

	events.erase(events.begin() + idx);
    }
}

DWORD netsocket::EventLoop::doProcessElapsedTime()
{
    if (timer)
    {
	ULONGLONG currentTickCountMs = GetTickCount64();

	if (currentTickCountMs >= timerTargetTickCountMs)
	{
	    doTimerEvent();

	    if (timer)
		return timer->timerIntervalMs();
	}
	else
	    return static_cast<DWORD>(timerTargetTickCountMs - currentTickCountMs);
    }

    return WSA_INFINITE;
}

void netsocket::EventLoop::doWaitForSocketEvent()
{
    while (true)
    {
	DWORD dwTimeoutMs = doProcessElapsedTime();

	switch (DWORD dwWait = ::WSAWaitForMultipleEvents(static_cast<DWORD>(events.size()), events.data(), false, dwTimeoutMs, true))
	{
	case WSA_WAIT_IO_COMPLETION:
	    continue;

	case WSA_WAIT_TIMEOUT:
	    if (timer)
		doTimerEvent();
	    break;

	case WSA_WAIT_FAILED:
	    if (+checkError<Error::EInProgress>())
	    {
		::Sleep(IN_PROGRESS_SLEEP_INTERVAL_MS);
		continue;
	    }

	    break;

	default:
	    if (WSA_WAIT_EVENT_0 <= dwWait && dwWait < WSA_WAIT_EVENT_0 + events.size())
	    {
		doSocketEvent(dwWait - WSA_WAIT_EVENT_0);

		if (timer && ::GetTickCount64() >= timerTargetTickCountMs)
		    doTimerEvent();
	    }
	    else
		throw InvalidAPIFunctionReturn(dwWait, "WSAWaitForMultipleEvents");

	    break;
	}

	break;
    }
}

void netsocket::EventLoop::doDisposeFreeList()
{
    BaseHandler *handler;

    while (freeList)
    {
	handler = freeList->next;
	delete freeList;
	freeList = handler;
    }
}

void netsocket::EventLoop::triggerNextEvent()
{
    if (events.empty() && !timer)
    {
	loopRunning = false;
	return;
    }

    doIterationStartEvent();
    doDisposeFreeList();

    if (events.empty())
    {
	::Sleep(timer->timerIntervalMs());
	doTimerEvent();
    }
    else
	doWaitForSocketEvent();
}

void netsocket::EventLoop::doQuitEvent()
{
    for (auto &handler: handlers)
	try
	{
	    handler.second->onLoopQuit(*this);
	}
	catch (...)
	{
	    if (exceptionHandler)
		exceptionHandler->onException(*this);
	}

    if (timer)
	try
	{
	    timer->onLoopQuit(*this);
	}
	catch (...)
	{
	    if (exceptionHandler)
		exceptionHandler->onException(*this);
	}

    if (iterationHandler)
	try
	{
	    iterationHandler->onLoopQuit(*this);
	}
	catch (...)
	{
	    if (exceptionHandler)
		exceptionHandler->onException(*this);
	}

    if (exceptionHandler)
	try
	{
	    exceptionHandler->onLoopQuit(*this);
	}
	catch (...)
	{
	    exceptionHandler->onException(*this);
	}

    doDisposeFreeList();

    handlers.clear();
    events.clear();
    timer = nullptr;
    iterationHandler = nullptr;
    exceptionHandler = nullptr;
}
