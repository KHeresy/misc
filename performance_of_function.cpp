#include <chrono>
#include <functional>
#include <iostream>
#include <string>

class CAdd
{
public:
	double operator()(double d1, double d2)
	{
		return d1 + d2;
	}
};

double fadd(double d1, double d2)
{
	return d1 + d2;
}

template<class _TFunc>
void RunTest(size_t uTimes, _TFunc funcTest, std::string sText)
{
	auto tpNow = std::chrono::high_resolution_clock::now();
	double dR;
	for (size_t t = 0; t < uTimes; ++t)
	{
		dR = funcTest();
	}
	std::cout << sText << "\tAnswer: " << dR << ", time " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tpNow).count() << "ms" << std::endl;
	
}

template<class _FUNC>
double Templeat_Func(size_t uSize, double* pD1, double* pD2,
					_FUNC func)
{
	double dRes = 0;
	for (size_t i = 0; i < uSize; ++i)
	{
		dRes += func(pD1[i], pD2[i]);
	}
	return dRes;
}

double STL_Func( size_t uSize, double* pD1, double* pD2, 
				std::function<double(double,double)> func )
{
	double dRes = 0;
	for (size_t i = 0; i < uSize; ++i)
	{
		dRes += func( pD1[i], pD2[i] );
	}
	return dRes;
}

double Pointer_Func(size_t uSize, double* pD1, double* pD2,
					double(*func)(double, double))
{
	double dRes = 0;
	for (size_t i = 0; i < uSize; ++i)
	{
		dRes += func(pD1[i], pD2[i]);
	}
	return dRes;
}

int main(int argc, char** argv)
{
	// allocate data
	size_t uTimes = 1000;
	size_t uSize = 1000000;
	double	*pD1 = new double[uSize],
			*pD2 = new double[uSize];
	
	// generate test data
	for (size_t i = 0; i < uSize; ++i)
	{
		pD1[i] = i;
		pD2[i] = -1.0 * i;
	}

	RunTest(uTimes, [&]()
	{
		return Templeat_Func(uSize, pD1, pD2, [](double d1, double d2){ return d1 + d2; });
	}, "template + lambda");

	RunTest(uTimes, [&]()
	{
		return Templeat_Func(uSize, pD1, pD2, CAdd());
	}, "template + class");

	RunTest(uTimes, [&]()
	{
		return Templeat_Func(uSize, pD1, pD2, fadd);
	}, "template + function");

	std::cout << std::endl;
	RunTest(uTimes, [&]()
	{
		return STL_Func(uSize, pD1, pD2, [](double d1, double d2){ return d1 + d2; });
	}, "std::function + lambda");

	RunTest(uTimes, [&]()
	{
		return STL_Func(uSize, pD1, pD2, CAdd());
	}, "std::function + class");

	RunTest(uTimes, [&]()
	{
		return STL_Func(uSize, pD1, pD2, fadd);
	}, "std::function + func");


	std::cout << std::endl;
	RunTest(uTimes, [&]()
	{
		return Pointer_Func(uSize, pD1, pD2, [](double d1, double d2){ return d1 + d2; });
	}, "function ptr + lambda");

	RunTest(uTimes, [&]()
	{
		return Pointer_Func(uSize, pD1, pD2, fadd);
	}, "function ptr + func");

}
