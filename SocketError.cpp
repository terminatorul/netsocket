#include <WinSock2.h>
#include <Windows.h>

#include <utility>
#include <string>
#include <system_error>

#include "SocketError.hpp"

using std::move;
using std::string;
using std::to_string;
using std::errc;
using std::error_category;
using std::error_condition;
using std::generic_category;

netsocket::BaseException::Tag const netsocket::socketExceptionTag { "WinSock2 API error" };

error_condition netsocket::ErrorCategory::default_error_condition(int ev) const noexcept
{
    errc code = errc::is_a_directory;

    switch (ev)
    {
    case +Error::InvalidHandle:
	code = errc::invalid_argument;	    // errc::bad_file_descriptor
	break;
    case +Error::NotEnoughMemory:
	code = errc::not_enough_memory;
	break;
    case +Error::InvalidParameter:
	code = errc::invalid_argument;
	break;
    case +Error::OperationAborted:
	code = errc::operation_canceled;
	break;
    case +Error::IOIncomplete:
	code = errc::resource_unavailable_try_again;
	break;
    case +Error::IOPending:
	code = errc::operation_in_progress;
	break;
    case +Error::EIntr:
	code = errc::interrupted;
	break;
    case +Error::BadF:
	code = errc::bad_file_descriptor;
	break;
    case +Error::EAccess:
	code = errc::permission_denied;
	break;
    case +Error::EFault:
	code = errc::bad_address;
	break;
    case +Error::EInval:
	code = errc::invalid_argument;
	break;
    case +Error::EMFile:
	code = errc::too_many_files_open;
	break;
    case +Error::EWouldBlock:
	code = errc::operation_would_block;
	break;
    case +Error::EInProgress:
	code = errc::operation_in_progress;
	break;
    case +Error::EAlready:
	code = errc::connection_already_in_progress;
	break;
    case +Error::ENotSock:
	code = errc::not_a_socket;
	break;
    case +Error::EDestAddrReq:
	code = errc::destination_address_required;
	break;
    case +Error::EMsgSize:
	code = errc::message_size;
	break;
    case +Error::EProtoType:
	code = errc::wrong_protocol_type;
	break;
    case +Error::EProtoOpt:
	code = errc::no_protocol_option;
	break;
    case +Error::EProtoNoSupport:
	code = errc::protocol_not_supported;
	break;
    case +Error::ESockTNoSupprot:
	code = errc::not_supported;
	break;
    case +Error::EOpNotSupp:
	code = errc::operation_not_supported;
	break;
    case +Error::EPFNoSupport:
	code = errc::address_family_not_supported;
	break;
    case +Error::EAFNoSupport:
	code = errc::address_family_not_supported;
	break;
    case +Error::EAddrInUse:
	code = errc::address_in_use;
	break;
    case +Error::EAddrNotAvail:
	code = errc::address_not_available;
	break;
    case +Error::ENetDown:
	code = errc::network_down;
	break;
    case +Error::ENetUnreach:
	code = errc::network_unreachable;
	break;
    case +Error::ENetReset:
	code = errc::network_reset;
	break;
    case +Error::EConnAborted:
	code = errc::connection_aborted;
	break;
    case +Error::EConnReset:
	code = errc::connection_reset;
	break;
    case +Error::ENoBufS:
	code = errc::no_buffer_space;
	break;
    case +Error::EIsConn:
	code = errc::already_connected;
	break;
    case +Error::ENotConn:
	code = errc::not_connected;
	break;
    case +Error::EShutDown:
	code = errc::broken_pipe;
	break;
    case +Error::ETooManyRefs:
	code = errc::too_many_links;
	break;
    case +Error::ETimedOut:
	code = errc::timed_out;
	break;
    case +Error::EConnRefused:
	code = errc::connection_refused;
	break;
    case +Error::ELoop:
	code = errc::too_many_symbolic_link_levels;
	break;
    case +Error::ENameTooLong:
	code = errc::filename_too_long;
	break;
    case +Error::EHostDown:
	code = errc::host_unreachable;
	break;
    case +Error::EHostUnreach:
	code = errc::host_unreachable;
	break;
    case +Error::ENotEmpty:
	code = errc::directory_not_empty;
	break;
    case +Error::EDQuot:
	code = errc::no_space_on_device;
	break;
    case +Error::ENoMore:
	code = errc::no_such_file_or_directory;
	break;
    case +Error::ECancelled:
	code = errc::operation_canceled;
	break;
    case +Error::EDiscon:
	code = errc::not_connected;
	break;
    case +Error::E_No_More:
	code = errc::no_such_file_or_directory;
	break;
    case +Error::E_Cancelled:
	code = errc::operation_canceled;
	break;
    case +Error::ERefused:
	code = errc::connection_refused;
	break;
    case +Error::HostNotFound:
	code = errc::no_such_device_or_address;
	break;
    case +Error::TryAgain:
	code = errc::resource_unavailable_try_again;
	break;
    case +Error::NoRecovery:
	code = errc::state_not_recoverable;
	break;
    case +Error::NoData:
	code = errc::no_message_available;
	break;
    }

    if (code == errc::is_a_directory)
	return move(error_condition(ev, system_category()));

    return move(make_error_condition(code));
}

string netsocket::ErrorCategory::message(int ev) const
{
    class LocalMemory
    {
    protected:
	void *memory;
    public:
	void *&getPtr()
	{
	    return memory;
	}

	~LocalMemory()
	{
	    if (memory)
	    {
		LocalFree(memory);
		memory = nullptr;
	    }
	}
    }
	localMemory;

    DWORD dwMsgLength = ::FormatMessageA
	(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	    nullptr,
	    ev,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	    static_cast<LPSTR>(static_cast<void *>(&localMemory.getPtr())),
	    0,
	    nullptr
	);

    if (dwMsgLength)
	return move(string(static_cast<char *>(localMemory.getPtr()), static_cast<char*>(localMemory.getPtr()) + dwMsgLength));

    return move(string("Windows Sockets error: " + to_string(ev)));
}

error_category &netsocket::system_category()
{
    static ErrorCategory socketErrorCategory;

    return socketErrorCategory;
}