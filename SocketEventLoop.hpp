#if !defined(WINSOCK2_CXX_SOCKET_EVENT_LOOP)
#define WINSOCK2_CXX_SOCKET_EVENT_LOOP

#include <WinSock2.h>
#include <cstdint>
#include <utility>
#include <string>
#include <stdexcept>
#include <vector>
#include <map>

#include "SocketError.hpp"
#include "SocketLibrary.hpp"
#include "SocketEventHandle.hpp"

namespace netsocket
{
    class EventLoop
    {
    public:
	class BaseHandler;
	class EventHandler;
	class TimerHandler;
	class ExceptionHandler;
	class IterationHandler;

	void  doTimerEvent();
	DWORD doProcessElapsedTime();
	void  doSocketEvent(unsigned idx);
	void  doQuitEvent();
	void  doWaitForSocketEvent();
	void  doIterationStartEvent();
	void  doDisposeFreeList();

    protected:
	bool loopRunning = false;
	ULONGLONG timerTargetTickCountMs = 0U;
	std::vector<WSAEVENT> events;
	std::map<WSAEVENT, EventHandler *> handlers;
	BaseHandler *freeList = nullptr;
	TimerHandler *timer = nullptr;
	ExceptionHandler *exceptionHandler = nullptr;
	IterationHandler *iterationHandler = nullptr;

    public:
	class BaseHandler
	{
	    friend class EventLoop;

	protected:
	    BaseHandler *next = nullptr;

	    virtual void onLoopQuit(EventLoop &eventLoop);

	public:
	    virtual ~BaseHandler();
	};

	class EventHandler: public BaseHandler
	{
	    friend class EventLoop;

	protected:
	    virtual EventHandle eventHandle() = 0;
	    virtual bool        onEventTrigger(EventLoop &eventLoop) = 0;
	};

	class TimerHandler: public BaseHandler
	{
	    friend class EventLoop;

	protected:
	    virtual std::uint_least32_t timerIntervalMs() = 0;
	    virtual bool onTimerTrigger(EventLoop &eventLoop) = 0;
	};

	class ExceptionHandler: public BaseHandler
	{
	    friend class EventLoop;

	protected:
	    virtual void onException(EventLoop &eventLoop) noexcept = 0;
	};

	class IterationHandler: public BaseHandler
	{
	    friend class EventLoop;

	protected:
	    virtual void onNextIteration(EventLoop &eventLoop) = 0;
	};

	enum class HandlerType
	{
	    Event,
	    Timer,
	    Exception,
	    Iteration
	};

	static BaseException::Tag const
	    multipleHandlersTag, eventCountExceededTag, invalidApiReturnValueTag;

	class EventLoopException: BaseException, public std::runtime_error
	{
	protected:
	    std::uintptr_t errorCode;

	public:
	    EventLoopException(BaseException::Tag const &tag, std::uintptr_t errorCode, char const *errorMessage);
	    EventLoopException(BaseException::Tag const &tag, std::uintptr_t errorCode, std::string const &errorMessage);
	    EventLoopException(BaseException::Tag const &tag, std::uintptr_t errorCode, std::string &&errorMessage);

	    virtual char const *what() const noexcept override;
	    virtual std::uintptr_t baseErrorCode() const noexcept override;
	};

	class EventCountExceeded: public EventLoopException
	{
	public:
	    EventCountExceeded();
	};

	class MultipleHandlers: public EventLoopException
	{
	protected:
	    WSAEVENT wsaEvent;
	    static std::string handlerTypeString(HandlerType handlerType);

	public:
	    MultipleHandlers(EventHandle eventHandle);
	    MultipleHandlers(HandlerType handlerType);

	    EventHandle event();
	};

	class InvalidAPIFunctionReturn: public EventLoopException
	{
	public:
	    InvalidAPIFunctionReturn(std::uintptr_t, char const *apiFunctionName);
	};

	static BaseException::Tag const quitLoopTag;

	class QuitLoop: public EventLoopException
	{
	public:
	    QuitLoop();
	};

	~EventLoop();

	unsigned long capacity() const noexcept;
	unsigned long available() const noexcept;
	bool	      full()      const noexcept;

	void addEventHandler(EventHandler &handler);
	void addTimerHandler(TimerHandler &handler);
	void addExceptionHandler(ExceptionHandler &handler);
	void addIterationHandler(IterationHandler &handler);

	template <typename HandlerT>
	    HandlerT *allocateHandler();
	void deallocateHandler(BaseHandler *handler);
	void enqueueDisposeHandler(BaseHandler *handler);

	void postQuitRequest();
	bool quitRequestPending();
	void triggerNextEvent();
	void runLoop(ExceptionHandler *handler = nullptr);

	EventLoop(Library &);
    };
}

inline netsocket::EventLoop::BaseHandler::~BaseHandler()
{
}

inline void netsocket::EventLoop::BaseHandler::onLoopQuit(EventLoop &)
{
}

inline void netsocket::EventLoop::postQuitRequest()
{
    loopRunning = false;
}

inline bool netsocket::EventLoop::quitRequestPending()
{
    return !loopRunning;
}

inline netsocket::EventLoop::EventLoopException::EventLoopException(BaseException::Tag const& tag, std::uintptr_t errorCode, char const* errorMessage)
    : BaseException(tag), runtime_error(errorMessage), errorCode(errorCode)
{
}

inline netsocket::EventLoop::EventLoopException::EventLoopException(BaseException::Tag const& tag, std::uintptr_t errorCode, std::string const &errorMessage)
    : BaseException(tag), runtime_error(errorMessage), errorCode(errorCode)
{
}

inline netsocket::EventLoop::EventLoopException::EventLoopException(BaseException::Tag const& tag, std::uintptr_t errorCode, std::string &&errorMessage)
    : BaseException(tag), runtime_error(errorMessage), errorCode(errorCode)
{
}

inline char const* netsocket::EventLoop::EventLoopException::what() const noexcept
{
    return this->runtime_error::what();
}

inline std::uintptr_t netsocket::EventLoop::EventLoopException::baseErrorCode() const noexcept
{
    return errorCode;
}

inline netsocket::EventLoop::EventCountExceeded::EventCountExceeded()
    : EventLoopException(eventCountExceededTag, 0U, "Maximum outstanding socket event count for the event loop was exceeded")
{
}

inline netsocket::EventLoop::MultipleHandlers::MultipleHandlers(EventHandle wsaEvent)
    : EventLoopException(multipleHandlersTag, static_cast<std::uintptr_t>(HandlerType::Event), "Multiple handlers registered in event loop for WSA event"),
	wsaEvent(wsaEvent)
{
}

inline netsocket::EventLoop::MultipleHandlers::MultipleHandlers(HandlerType handlerType)
    : EventLoopException
	(
	    multipleHandlersTag,
	    static_cast<std::uintptr_t>(handlerType),
	    "Multiple " + handlerTypeString(handlerType) + " handlers registered for socket event loop."
	)
{
}

inline netsocket::EventHandle netsocket::EventLoop::MultipleHandlers::event()
{
    return std::move(EventHandle(wsaEvent));
}

inline netsocket::EventLoop::InvalidAPIFunctionReturn::InvalidAPIFunctionReturn(std::uintptr_t returnValue, char const *apiFunctionName)
    : EventLoopException(invalidApiReturnValueTag, returnValue, "Invalid return value " + std::to_string(returnValue) + " from WinSock2 API function " + apiFunctionName)
{
}


inline netsocket::EventLoop::QuitLoop::QuitLoop()
    : EventLoopException(quitLoopTag, 0U, "Event loop quit")
{
}

inline netsocket::EventLoop::~EventLoop()
{
    doQuitEvent();
}

inline unsigned long netsocket::EventLoop::capacity() const noexcept
{
    return WSA_MAXIMUM_WAIT_EVENTS;
}

inline unsigned long netsocket::EventLoop::available() const noexcept
{
    return WSA_MAXIMUM_WAIT_EVENTS - events.size();
}

inline bool netsocket::EventLoop::full() const noexcept
{
    return !available();
}

inline void netsocket::EventLoop::addEventHandler(EventHandler &handler)
{
    WSAEVENT wsaEvent = handler.eventHandle();
    EventHandler *&handlerPtr = handlers[wsaEvent];

    if (handlerPtr)
	throw MultipleHandlers(wsaEvent);

    if (events.size() >= WSA_MAXIMUM_WAIT_EVENTS)
	throw EventCountExceeded();

    events.push_back(wsaEvent);
    handlerPtr = &handler;
}

inline void netsocket::EventLoop::addTimerHandler(TimerHandler &handler)
{
    if (timer)
	throw MultipleHandlers(HandlerType::Timer);

    timer = &handler;
    timerTargetTickCountMs = ::GetTickCount64() + timer->timerIntervalMs();
}

inline void netsocket::EventLoop::addExceptionHandler(ExceptionHandler &handler)
{
    if (exceptionHandler)
	throw MultipleHandlers(HandlerType::Exception);

    exceptionHandler = &handler;
}

inline void netsocket::EventLoop::addIterationHandler(IterationHandler &handler)
{
    if (iterationHandler)
	throw MultipleHandlers(HandlerType::Iteration);

    iterationHandler = &handler;
}

template <typename HandlerT>
    inline HandlerT *netsocket::EventLoop::allocateHandler()
{
    return new HandlerT();
}

inline void netsocket::EventLoop::deallocateHandler(BaseHandler *handler)
{
    delete handler;
}

inline void netsocket::EventLoop::enqueueDisposeHandler(BaseHandler *handler)
{
    handler->next = freeList;
    freeList = handler;
}

inline void netsocket::EventLoop::runLoop(ExceptionHandler *handler)
{
    if (handler)
	addExceptionHandler(*handler);

    loopRunning = true;

    while (loopRunning)
	triggerNextEvent();

    doQuitEvent();
}

inline netsocket::EventLoop::EventLoop(Library &)
{
}

#endif // !defined(WINSOCK2_CXX_SOCKET_EVENT_LOOP)
