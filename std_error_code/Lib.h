#pragma once

#include <system_error>

namespace Heresy
{
	// Error code
	enum class error
	{
		Success = 0,
		TooLarge,
		TooSmall,
		Invaild
	};

	// Error condition
	enum class errc
	{
		OutOfRange = 1,
		InvaildInput
	};

	#pragma region Functions of library
	bool testFunction(int i, std::error_code& ec);
	#pragma endregion

	#pragma region Functions to make error_code and error_condition
	std::error_code make_error_code(error ec);
	std::error_condition make_error_condition(error ec);

	std::error_code make_error_code(errc ec);
	std::error_condition make_error_condition(errc ec);
	#pragma endregion
}

namespace std
{
	template <>
	struct is_error_code_enum<Heresy::error> : true_type {};

	template <>
	struct is_error_condition_enum<Heresy::errc> : true_type {};
}
