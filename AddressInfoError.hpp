#if !defined(WINSOCK2_CXX_ADDRESS_INFO_ERROR)
#define WINSOCK2_CXX_ADDRESS_INFO_ERROR

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <system_error>
#include "SocketError.hpp"

namespace netsocket
{
    enum class AddressInfoError: int
    {
	Ok		= 0,
	Again		= EAI_AGAIN,
	Memory		= EAI_MEMORY,
	BadFlags	= EAI_BADFLAGS,
	Fail		= EAI_FAIL,
	Family		= EAI_FAMILY,
	IPSecPolicy	= EAI_IPSECPOLICY,
	NoData		= EAI_NODATA,
	NoName		= EAI_NONAME,
	NoSecureName	= EAI_NOSECURENAME,
	Service		= EAI_SERVICE,
	SockType	= EAI_SOCKTYPE
    };

    constexpr inline bool operator !(AddressInfoError addressInfoError);
    constexpr inline int  operator +(AddressInfoError addressInfoError);

    std::error_category const &generic_address_info_category();
    std::error_category const &system_address_info_category();

    extern BaseException::Tag const addressExceptionTag;

    class AddressException: public BaseExceptionType<addressExceptionTag>, public std::system_error
    {
	AddressException(int wsaError);
	AddressException(int wsaError, std::string const& errorMessage);
	AddressException(int wsaError, char const* errorMessage);

	virtual char const* what() const noexcept override;
	virtual std::uintptr_t baseErrorCode() const noexcept override;

    };

    template <Error... expectedSocketError>
	Error checkAddressInfoError(int wsaLastError = ::WSAGetLastError());
}

namespace std
{
    error_condition make_error_condition(netsocket::AddressInfoError address_info_error) noexcept;

    template <>
	struct is_error_condition_enum<netsocket::AddressInfoError>: true_type
    {
    };
}

constexpr inline bool netsocket::operator !(AddressInfoError addressInfoError)
{
    return addressInfoError == AddressInfoError::Ok;
}

constexpr inline int netsocket::operator +(AddressInfoError addressInfoError)
{
    return static_cast<int>(addressInfoError);
}

inline netsocket::AddressException::AddressException(int wsaError)
    : system_error(wsaError, system_address_info_category())
{
}

inline netsocket::AddressException::AddressException(int wsaError, std::string const& errorMessage)
    : system_error(wsaError, system_address_info_category(), errorMessage)
{
}

inline netsocket::AddressException::AddressException(int wsaError, char const* errorMessage)
    : system_error(wsaError, system_address_info_category(), errorMessage)
{
}

inline char const* netsocket::AddressException::what() const noexcept
{
    return this->system_error::what();
}

inline std::uintptr_t netsocket::AddressException::baseErrorCode() const noexcept
{
    return this->system_error::code().value();
}

template <netsocket::Error... expectedSocketError>
    netsocket::Error netsocket::checkAddressInfoError(int wsaLastError)
{
    if ((... || (wsaLastError == static_cast<int>(expectedSocketError))))
	return static_cast<Error>(wsaLastError);

    if (wsaLastError)
	throw AddressException(wsaLastError);

    return static_cast<Error>(wsaLastError);
}

inline std::error_condition std::make_error_condition(netsocket::AddressInfoError address_info_error) noexcept
{
    return error_condition(static_cast<int>(address_info_error), netsocket::generic_address_info_category());
}

#endif // !defined(WINSOCK2_CXX_ADDRESS_INFO_ERROR)
