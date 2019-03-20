#include <iostream>

#include <gsl/gsl_util>

class CData
{
public:
	CData()
	{
		std::cout << " > CData created" << std::endl;
	}

	~CData()
	{
		std::cout << " > CData released" << std::endl;
	}
};


int leak( int iInput)
{
	CData* pData = new CData();

	if (iInput > 0)
		return 1;

	delete pData;
	return -1;
}

int noleak(int iInput)
{
	CData* pData = new CData();
	auto _ = gsl::finally([pData] {
		delete pData;
	});

	if (iInput > 0)
		return 1;

	return -1;
}

int main()
{
	std::cout << "May leak" << std::endl;
	leak(1);

	std::cout << "No leak with gsl::finally" << std::endl;
	noleak(1);
}
