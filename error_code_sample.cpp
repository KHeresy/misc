#include <iostream>
#include <system_error>

namespace Heresy
{
	// define error code
	enum class ErrorCode
	{
		Success = 0,
		ErrorType1,
		ErrorType2
	};

	// define error_category
	class ErrorCategory : public std::error_category
	{
	public:
		ErrorCategory() = default;

		// map ErrorCode to detail message text
		std::string message(int c) const override
		{
			switch (static_cast<ErrorCode>(c))
			{
			case ErrorCode::Success:
				return "Success";

			case ErrorCode::ErrorType1:
				return "Error Type 1";

			case ErrorCode::ErrorType2:
				return "Error Type 2";
			}
		}

		// the name of this error_category
		const char* name() const noexcept override
		{
			return "Error Category by Heresy";
		}

	public:
		// get referenceof shared ErrorCategory
		static const std::error_category& get()
		{
			const static ErrorCategory sCategory;
			return sCategory;
		}
	};

	// convert ErrorCode to std::error_code
	std::error_code make_error_code(ErrorCode ec)
	{
		return std::error_code(static_cast<int>(ec), ErrorCategory::get());
	}

	void MyFunction(bool bError);
	void MyFunction(bool bError, std::error_code& ec);
	std::error_code MyFunction2(bool bError);
}

namespace std
{
	// let compiler know that Heresy::ErrorCode is compatible with std::error_code
	template <>
	struct is_error_code_enum<Heresy::ErrorCode> : true_type {};
}

int main()
{
	// try-catch
	{
		try
		{
			Heresy::MyFunction(true);
		}
		catch (std::error_code & e)
		{
			std::cout << e.message() << std::endl;
		}
	}

	// error_code as argument
	{
		std::error_code ec;
		Heresy::MyFunction(true,ec);

		if (ec)
		{
			std::cout << "Error : " << ec.message() << std::endl;
		}
		else
		{
			std::cout << "Work fine" << std::endl;
		}
	}

	// error_code as retun value
	{
		std::error_code ec = Heresy::MyFunction2(true);
		if (ec == Heresy::ErrorCode::ErrorType1)
		{
			// do something
			std::cout << ec << std::endl;
		}
		else if (ec == Heresy::ErrorCode::ErrorType2)
		{
			// do something
		}
	}

	return 0;
}

namespace Heresy
{
	void MyFunction(bool bError)
	{
		if (bError)
			throw ErrorCode::ErrorType1;
	}

	void MyFunction(bool bError, std::error_code& ec)
	{
		if (bError)
			ec = ErrorCode::ErrorType2;
		else
			ec = ErrorCode::Success;
	}

	std::error_code MyFunction2(bool bError)
	{
		if (bError)
			return ErrorCode::ErrorType1;
		else
			return ErrorCode::Success;
	}
}
