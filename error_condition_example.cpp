#include <iostream>
#include <system_error>

namespace Heresy
{
	enum class ErrorCondition
	{
		FileError = 1,
		NetworkError
	};

	// define error_category
	class ErrorCategory : public std::error_category
	{
	public:
		std::string message(int c) const override
		{
			switch (static_cast<ErrorCondition>(c))
			{
			case ErrorCondition::FileError:
				return "File-related error";

			case ErrorCondition::NetworkError:
				return "network related error";

			default:
				return "Undefined";
			}
		}

		const char* name() const noexcept override
		{
			return "Error condition category by Heresy";
		}

		bool equivalent(const std::error_code& ec, int iCond) const noexcept override
		{
			switch (static_cast<ErrorCondition>(iCond))
			{
			case ErrorCondition::FileError:
				return (ec == std::errc::file_exists ||
						ec == std::errc::file_too_large ||
						ec == std::errc::no_such_file_or_directory ||
						ec == std::errc::read_only_file_system);

			case ErrorCondition::NetworkError:
				return (ec == std::errc::network_down ||
						ec == std::errc::network_reset ||
						ec == std::errc::network_unreachable);
			}

			return false;
		}

	public:
		static const std::error_category& get()
		{
			const static ErrorCategory sCategory;
			return sCategory;
		}
	};

	std::error_condition make_error_condition(ErrorCondition ec)
	{
		return std::error_condition(static_cast<int>(ec), ErrorCategory::get());
	}
}

namespace std
{
	// let compiler know that Heresy::ErrorCode is compatible with std::error_condition
	template <>
	struct is_error_condition_enum<Heresy::ErrorCondition> : true_type {};
}

int main()
{
	std::error_code ec = std::make_error_code(std::errc::file_exists);
	if (ec == Heresy::ErrorCondition::FileError)
		std::cout << ec.message() << " is a kind of file error";
	else
		std::cout << ec.message() << " is not a kind of file error";

	return 0;
}
