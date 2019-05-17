#include <iostream>

#include "Lib.h"

int main()
{
	std::error_code ec;
	Heresy::testFunction(7, ec);
	if (ec)
	{
		if (ec == Heresy::errc::OutOfRange)
			std::cerr << "Out of range, require 4 - 128" << std::endl;
		else
			std::cerr << ec.message() << std::endl;
	}
	else
	{
		std::cout << "It works" << std::endl;
	}

	return 0;
}
