#include <iostream>

#include <boost/process.hpp>

int main(int argc, char** argv)
{
	std::cout << boost::process::system("ping 127.0.0.1") << std::endl;
}
