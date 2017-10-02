#include <iostream>
#include <string>
#include <variant>
#include <vector>

struct SOutput
{
	void operator()(const int& i)
	{
		std::cout << i << std::endl;
	}

	void operator()(const double& d)
	{
		std::cout << d << std::endl;
	}

	void operator()(const std::string& s)
	{
		std::cout << s << std::endl;
	}
};

struct STOutput
{
	template<typename TYPE>
	void operator()(const TYPE& v)
	{
		std::cout << v << std::endl;
	}
};

struct STwice
{
	template<typename TYPE>
	void operator()(TYPE& v)
	{
		v *= 2;
	}

	template<>
	void operator()(std::string& s)
	{
		s += s;
	}
};


int main()
{
	std::variant<int, double, std::string> x, y;

	// assign value
	x = 1;
	y = "1.0";

	// overwrite value
	x = 2.0;

	// check index
	std::cout << "x - " << x.index() << std::endl;
	std::cout << "y - " << y.index() << std::endl;

	// read value
	double d = std::get<double>(x);
	std::string s = std::get<2>(y);

	// error type
	try
	{
		int i = std::get<int>(x);
	}
	catch (std::bad_variant_access e)
	{
		std::cerr << e.what() << std::endl;
	}

	// use get_if
	int* i = std::get_if<int>(&x);
	if (i == nullptr)
	{
		std::cout << "wrong type" << std::endl;
	}
	else
	{
		std::cout << "value is " << *i << std::endl;
	}

	// visit
	std::visit(SOutput(), y);
	std::visit(STwice(), x);
	std::visit(STwice(), y);
	std::visit(STOutput(), x);
	std::visit(
		[](const auto& v) {std::cout << v << std::endl; },
		y);

	// vector
	using var_t = std::variant<int, double, std::string>;
	std::vector<var_t> vData = { 1, 2.0, "hi" };
	for (var_t& v : vData)
	{
		std::visit(STwice(), v);
		std::visit(SOutput(), v);
	}
}
