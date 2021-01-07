#define NOMINMAX
#include <WS2tcpip.h>

#include <limits>
#include <string>
#include <system_error>
#include <utility>

#include "SocketError.hpp"
#include "AddressInfoError.hpp"

using std::move;
using std::numeric_limits;
using std::string;
using std::error_condition;
using std::error_category;
using std::make_error_condition;

namespace netsocket
{
    class AddressInfoErrorCategory: public error_category
    {
    public:
	using error_category::error_category;

	virtual char const* name() const noexcept override
	{
	    return "AddressInfo";
	}

	virtual string message(int ev) const override;
    };

    class WinSock2AddressInfoErrorCategory: public ErrorCategory
    {
    public:
	using ErrorCategory::ErrorCategory;

	virtual char const* name() const noexcept override
	{
	    return "WinSock2AddressInfo";
	}

	virtual error_condition default_error_condition(int ev) const noexcept override;
    };
}

netsocket::BaseException::Tag const netsocket::addressExceptionTag { "GetAddressInfo API error" };

string netsocket::AddressInfoErrorCategory::message(int ev) const
{
    return move(string(gai_strerrorA(ev)));
}

error_condition netsocket::WinSock2AddressInfoErrorCategory::default_error_condition(int ev) const noexcept
{
    if (ev)
    {
	AddressInfoError gai_error = AddressInfoError::Ok;

	switch (ev)
	{
	case +Error::TryAgain:
	    gai_error = AddressInfoError::Again;
	    break;
	case +Error::EInval:
	    gai_error = AddressInfoError::BadFlags;
	    break;
	case +Error::NoRecovery:
	    gai_error = AddressInfoError::Fail;
	    break;
	case +Error::EAFNoSupport:
	    gai_error = AddressInfoError::Family;
	    break;
	case +Error::NotEnoughMemory:
	    gai_error = AddressInfoError::Memory;
	    break;
	case +Error::HostNotFound:
	    gai_error = AddressInfoError::NoName;
	    break;
	case +Error::TypeNotFound:
	    gai_error = AddressInfoError::Service;
	    break;
	case +Error::ESockTNoSupprot:
	    gai_error = AddressInfoError::SockType;
	    break;
	}

	if (+gai_error)
	    return move(make_error_condition(gai_error));

	return move(this->ErrorCategory::default_error_condition(ev));
    }

    return move(make_error_condition(AddressInfoError::Ok));
}

error_category const &netsocket::generic_address_info_category()
{
    static AddressInfoErrorCategory addressInfoCategory;

    return addressInfoCategory;
}

error_category const &netsocket::system_address_info_category()
{
    static WinSock2AddressInfoErrorCategory winsock2AddressInfoCategory;

    return winsock2AddressInfoCategory;
}
