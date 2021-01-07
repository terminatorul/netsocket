#if !defined(WINSOCK2_CXX_SOCKET_ERROR)
#define WINSOCK2_CXX_SOCKET_ERROR

#include <WinSock2.h>
#include <string>
#include <system_error>

namespace netsocket
{
    enum class Error: int
    {
	Ok			= 0,
	InvalidHandle		= WSA_INVALID_HANDLE,		// Specified event object handle is invalid.
	NotEnoughMemory		= WSA_NOT_ENOUGH_MEMORY,	// Insufficient memory available.
	InvalidParameter	= WSA_INVALID_PARAMETER,	// One or more parameters are invalid.
	OperationAborted	= WSA_OPERATION_ABORTED,	// Overlapped operation aborted
	IOIncomplete		= WSA_IO_INCOMPLETE,		// Overlapped I/O event object not in signaled state.
    	IOPending		= WSA_IO_PENDING,		// Overlapped operations will complete later.
	EIntr			= WSAEINTR,			// Interrupted function call.
	BadF			= WSAEBADF,			// File handle is not valid.
	EAccess			= WSAEACCES,			// Permission denied.
	EFault			= WSAEFAULT,			// Bad address.
	EInval			= WSAEINVAL,			// Invalid argument.
	EMFile			= WSAEMFILE,			// Too many open files.
	EWouldBlock		= WSAEWOULDBLOCK,		// Resource temporarily unavailable.
	EInProgress		= WSAEINPROGRESS,		// Operation now in progress.
	EAlready		= WSAEALREADY,			// Operation already in progress.
	ENotSock		= WSAENOTSOCK,			// Socket operation on nonsocket.
	EDestAddrReq		= WSAEDESTADDRREQ,		// Destination address required.
	EMsgSize		= WSAEMSGSIZE,			// Message too long.
	EProtoType		= WSAEPROTOTYPE,		// Protocol wrong type for socket.
	EProtoOpt		= WSAENOPROTOOPT,		// Bad protocol option.
	EProtoNoSupport		= WSAEPROTONOSUPPORT,		// Protocol not supported.
	ESockTNoSupprot		= WSAESOCKTNOSUPPORT,		// Socket type not supported.
	EOpNotSupp		= WSAEOPNOTSUPP,		// Operation not supported.
	EPFNoSupport		= WSAEPFNOSUPPORT,		// Protocol family not supported
	EAFNoSupport		= WSAEAFNOSUPPORT,		// Address family not supported by protocol family.
	EAddrInUse		= WSAEADDRINUSE,		// Address already in use.
	EAddrNotAvail		= WSAEADDRNOTAVAIL,		// Cannot assign requested address.
	ENetDown		= WSAENETDOWN,			// Network is down.
	ENetUnreach		= WSAENETUNREACH,		// Network is unreachable.
	ENetReset		= WSAENETRESET,			// Network dropped connection on reset.
	EConnAborted		= WSAECONNABORTED,		// Software caused connection abort.
	EConnReset		= WSAECONNRESET,		// Connection reset by peer.
	ENoBufS			= WSAENOBUFS,			// No buffer space available.
	EIsConn			= WSAEISCONN,			// Socket is already connected.
	ENotConn		= WSAENOTCONN,			// Socket is not connected.
	EShutDown		= WSAESHUTDOWN,			// Cannot send after socket shutdown.
	ETooManyRefs		= WSAETOOMANYREFS,		// Too many references.
	ETimedOut		= WSAETIMEDOUT,			// Connection timed out.
	EConnRefused		= WSAECONNREFUSED,		// Connection refused.
	ELoop			= WSAELOOP,			// Cannot translate name.
	ENameTooLong		= WSAENAMETOOLONG,		// Name too long.
	EHostDown		= WSAEHOSTDOWN,			// Host is down.
	EHostUnreach		= WSAEHOSTUNREACH,		// No route to host.
	ENotEmpty		= WSAENOTEMPTY,			// Directory not empty.
	EProcLim		= WSAEPROCLIM,			// Too many processes.
	EUsers			= WSAEUSERS,			// User quota exceeded.
	EDQuot			= WSAEDQUOT,			// Disk quota exceeded.
	EStale			= WSAESTALE,			// Stale file handle reference.
	ERemote			= WSAEREMOTE,			// Item is remote.
	SysNotReady		= WSASYSNOTREADY,		// Network subsystem is unavailable
	VerNotSupported		= WSAVERNOTSUPPORTED,		// Winsock.dll version out of range.
	NotInitialized		= WSANOTINITIALISED,		// Successful WSAStartup not yet performed.
	EDiscon			= WSAEDISCON,			// Graceful shutdown in progress.
	ENoMore			= WSAENOMORE,			// No more results.
	ECancelled		= WSAECANCELLED,		// Call has been canceled.
	EInvalidProcTable	= WSAEINVALIDPROCTABLE,		// Procedure call table is invalid.
	EInvalidProvider	= WSAEINVALIDPROVIDER,		// Service provider is invalid.
	EProviderFailedInit	= WSAEPROVIDERFAILEDINIT,	// Service provider failed to initialize.
	SysCallFailure		= WSASYSCALLFAILURE,		// System call failure.
	ServiceNotFound		= WSASERVICE_NOT_FOUND,		// Service not found.
	TypeNotFound		= WSATYPE_NOT_FOUND,		// Class type not found.
	E_No_More		= WSA_E_NO_MORE,		// No more results.
	E_Cancelled		= WSA_E_CANCELLED,		// Call was canceled.
	ERefused		= WSAEREFUSED,			// Database query was refused.
	HostNotFound		= WSAHOST_NOT_FOUND,		// Host not found.
	TryAgain		= WSATRY_AGAIN,			// Nonauthoritative host not found.
	NoRecovery		= WSANO_RECOVERY,		// This is a nonrecoverable error.
	NoData			= WSANO_DATA,			// Valid name, no data record of requested type.
	QoSReceivers		= WSA_QOS_RECEIVERS,		// QoS receivers.
	QoSSenders		= WSA_QOS_SENDERS,		// QoS senders.
	QoSNoSenders		= WSA_QOS_NO_SENDERS,		// No QoS senders.
	QoSNoReceivers		= WSA_QOS_NO_RECEIVERS,		// QoS no receivers.
	QoSRequestConfirmed	= WSA_QOS_REQUEST_CONFIRMED,	// QoS request confirmed.
	QoSAdmissionFailure	= WSA_QOS_ADMISSION_FAILURE,	// QoS admission error.
	QoSPolicyFailure	= WSA_QOS_POLICY_FAILURE,	// QoS policy failure.
	QoSBadStyle		= WSA_QOS_BAD_STYLE,		// QoS bad style.
	QoSBadObject		= WSA_QOS_BAD_OBJECT,		// QoS bad object.
	QoSTrafficCtrlError	= WSA_QOS_TRAFFIC_CTRL_ERROR,	// QoS traffic control error.
	QoSGenericError		= WSA_QOS_GENERIC_ERROR,	// QoS generic error.
	QoSEServiceType		= WSA_QOS_ESERVICETYPE,		// QoS service type error.
	QoSEFlowSpec		= WSA_QOS_EFLOWSPEC,		// QoS flowspec error.
	QoSEProvSpecBuf		= WSA_QOS_EPROVSPECBUF,		// Invalid QoS provider buffer.
	QoSEFilterStyle		= WSA_QOS_EFILTERSTYLE,		// Invalid QoS filter style.
	QoSEFilterType		= WSA_QOS_EFILTERTYPE,		// Invalid QoS filter type.
	QosSEFilterCount	= WSA_QOS_EFILTERCOUNT,		// Incorrect QoS filter count.
	QoSEObjLength		= WSA_QOS_EOBJLENGTH,		// Invalid QoS object length.
	QoSEFlowCount		= WSA_QOS_EFLOWCOUNT,		// Incorrect QoS flow count.
	QoSEUnknownPSObj	= WSA_QOS_EUNKOWNPSOBJ,		// Unrecognized QoS object.
	QoSEPolicyObj		= WSA_QOS_EPOLICYOBJ,		// Invalid QoS policy object.
	QoSEFlowDesc		= WSA_QOS_EFLOWDESC,		// Invalid QoS flow descriptor.
	QoSEPSFlowSpec		= WSA_QOS_EPSFLOWSPEC,		// Invalid QoS provider-specific flowspec.
	QoSEPSFilterSpec	= WSA_QOS_EPSFILTERSPEC,	// Invalid QoS provider-specific filterspec.
	QoSESDModeObj		= WSA_QOS_ESDMODEOBJ,		// Invalid QoS shape discard mode object.
	QoSEShapeRateObj	= WSA_QOS_ESHAPERATEOBJ,	// Invalid QoS shaping rate object.
	QoSReservedPEType	= WSA_QOS_RESERVED_PETYPE	// Reserved policy QoS element type.
    };

    constexpr bool operator !(Error socketError);
    constexpr int  operator +(Error socketError);

    class ErrorCategory: public std::error_category
    {
    public:
	using error_category::error_category;

	virtual char const *name() const noexcept override;
	virtual std::error_condition default_error_condition(int ev) const noexcept override;
	virtual std::string message(int ev) const override;
    };

    std::error_category &system_category();

    class BaseException
    {
    public:
	struct Tag
	{
	    char const *tagDisplayName;

	    bool operator ==(Tag const &other) const noexcept;
	    bool operator !=(Tag const &other) const noexcept;
	    bool operator ==(Tag &&other) const noexcept = delete;
	    bool operator !=(Tag &&other) const noexcept = delete;
	};

	BaseException(Tag const &tag);

	virtual char const *what() const noexcept = 0;
	virtual std::uintptr_t baseErrorCode() const noexcept = 0;

	Tag const &exceptionTag() const noexcept;

    protected:
	Tag const &tag;
    };

    template <BaseException::Tag const &tag>
	class BaseExceptionType: public BaseException
    {
    public:
	BaseExceptionType();
    };

    extern BaseException::Tag const socketExceptionTag;

    class Exception: public BaseExceptionType<socketExceptionTag>, public std::system_error
    {
    public:
	Exception(int wsaError);
	Exception(int wsaError, std::string const& errorMessage);
	Exception(int wsaError, char const* errorMessage);

	virtual char const *what() const noexcept override;
	virtual std::uintptr_t baseErrorCode() const noexcept override;
    };

    void raiseError(int wsaError);
    void raiseError(Error socketError);

    template <Error... expectedError>
	Error checkError(int wsaLastError = ::WSAGetLastError());
}

namespace std
{
    error_code make_error_code(netsocket::Error socket_err) noexcept;

    template <>
        struct is_error_code_enum<netsocket::Error> : true_type
    {
    };
}


constexpr inline bool netsocket::operator !(Error socketError)
{
    return socketError == Error::Ok;
}
constexpr inline int netsocket::operator +(Error socketError)
{
    return static_cast<int>(socketError);
}

inline char const *netsocket::ErrorCategory::name() const noexcept
{
    return "WinSock2";
}

inline std::error_code std::make_error_code(netsocket::Error socket_err) noexcept
{
    return error_code(static_cast<int>(socket_err), netsocket::system_category());
}

inline bool netsocket::BaseException::Tag::operator ==(Tag const &other) const noexcept
{
    return this == &other;
}

inline bool netsocket::BaseException::Tag::operator !=(Tag const& other) const noexcept
{
    return this != &other;
}

inline netsocket::BaseException::BaseException(Tag const &tag)
    : tag(tag)
{
}

inline netsocket::BaseException::Tag const &netsocket::BaseException::exceptionTag() const noexcept
{
    return tag;
}

template <netsocket::BaseException::Tag const &tag>
    inline netsocket::BaseExceptionType<tag>::BaseExceptionType()
	: BaseException(tag)
{
}

inline netsocket::Exception::Exception(int wsaError)
    : system_error(wsaError, system_category())
{
}

inline netsocket::Exception::Exception(int wsaError, std::string const &errorMessage)
    : system_error(wsaError, system_category(), errorMessage)
{
}

inline netsocket::Exception::Exception(int wsaError, char const *errorMessage)
    : system_error(wsaError, system_category(), errorMessage)
{
}

inline char const *netsocket::Exception::what() const noexcept
{
    return this->system_error::what();
}

inline std::uintptr_t netsocket::Exception::baseErrorCode() const noexcept
{
    return this->system_error::code().value();
}

inline void netsocket::raiseError(int wsaError = ::WSAGetLastError())
{
    throw Exception(wsaError);
}

inline void netsocket::raiseError(Error socketError)
{
    throw Exception(+socketError);
}

template <netsocket::Error... expectedError>
    netsocket::Error netsocket::checkError(int wsaLastError)
{
    if ((... || (wsaLastError == +expectedError)))
	return static_cast<Error>(wsaLastError);

    if (wsaLastError)
	raiseError(wsaLastError);

    return static_cast<Error>(wsaLastError);
}

#endif // !defined(WINSOCK2_CXX_SOCKET_ERROR)