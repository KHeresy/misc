#include <iostream>

#include <boost/process.hpp>

int main(int argc, char** argv)
{
	boost::process::spawn("notepad");
}
