#include <iostream>

#include <boost/process.hpp>

int main(int argc, char** argv)
{
	boost::process::child c("ping 127.0.0.1");
	c.wait();

	std::cout << c.exit_code() << std::endl;
}
