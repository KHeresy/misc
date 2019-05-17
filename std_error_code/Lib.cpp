#include "Lib.h"

namespace Heresy
{
	bool testFunction(int i, std::error_code& ec)
	{
		if (i < 4)
			ec = error::TooSmall;
		else if (i > 128)
			ec = error::TooLarge;
		else if( i % 2)
			ec = error::Invaild;
		else
		{
			ec = error::Success;
			return true;
		}

		return false;
	}

	class ErrorCategory : public std::error_category
	{
	public:
		std::string message(int c) const override
		{
			switch (static_cast<error>(c))
			{
			case error::Success:
				return "Success";

			case error::TooLarge:
				return "Value too large";

			case error::TooSmall:
				return "Value too small";

			case error::Invaild:
				return "Not a multiple of 2";

			default:
				return "Undefined";
			}
		}

		const char* name() const noexcept override
		{
			return "Error code category";
		}

	public:
		static const std::error_category& get()
		{
			const static ErrorCategory sCategory;
			return sCategory;
		}
	};

	class ErrcCategory : public std::error_category
	{
	public:
		std::string message(int c) const override
		{
			return "API error";
		}

		const char* name() const noexcept override
		{
			return "Error condition category";
		}

		bool equivalent(const std::error_code& ec, int c) const noexcept override
		{
			switch (static_cast<errc>(c))
			{
			case errc::OutOfRange:
				return (ec == error::TooLarge || ec == error::TooSmall);

			case errc::InvaildInput:
				return (ec == error::Invaild);
			}

			return false;
		}

	public:
		static const std::error_category& get()
		{
			const static ErrcCategory sCategory;
			return sCategory;
		}
	};

	std::error_code make_error_code(error ec)
	{
		return std::error_code(static_cast<int>(ec), ErrorCategory::get());
	}

	std::error_condition make_error_condition(error ec)
	{
		return std::error_condition(static_cast<int>(ec), ErrorCategory::get());
	}

	std::error_code make_error_code(errc ec)
	{
		return std::error_code(static_cast<int>(ec), ErrcCategory::get());
	}

	std::error_condition make_error_condition(errc ec)
	{
		return std::error_condition(static_cast<int>(ec), ErrcCategory::get());
	}
}
