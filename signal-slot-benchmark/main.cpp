#include <iostream>
#include <chrono>
#include <functional>
#include <vector>
#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include "QtClass.h"

class BasicReciver
{
public:
	void impl(int iVal)
	{
		m_iAnswer = 0;
		for (unsigned int i = 0; i < iVal; ++i)
			m_iAnswer += i;
	}

public:
	int m_iAnswer;
};

class BaseReciver
{
public:
	virtual void impl(int iVal)
	{
		m_iAnswer = 0;
		for (unsigned int i = 0; i < iVal; ++i)
			m_iAnswer += i;
	}

public:
	int m_iAnswer;
};

class ExtReciver : public BaseReciver
{
public:
	void impl(int iVal) override
	{
		m_iAnswer = 0;
		for (unsigned int i = 0; i < iVal; ++i)
			m_iAnswer += i;
	}
};

class CfuncCaller
{
public:
	void run(int iVal)
	{
		for( auto& rFunc: m_vReciver)
			rFunc(iVal);
	}

	std::vector<std::function<void(int)>> m_vReciver;
};

class BSigCaller
{
public:
	void run(int iVal)
	{
		invoke(iVal);
	}

	boost::signals2::signal<void(int)> invoke;
};

class MemCaller
{
public:
	void run(int iVal)
	{
		for (auto pRec : m_vReciver)
			pRec->impl(iVal);
	}

	std::vector<BasicReciver*> m_vReciver;
};

class SptrCaller
{
public:
	void run(int iVal)
	{
		for (auto pRec : m_vReciver)
			pRec->impl(iVal);
	}

	std::vector<BasicReciver*> m_vReciver;
};

class VirCaller
{
public:
	void run(int iVal)
	{
		for (auto pRec : m_vReciver)
			pRec->impl(iVal);
	}

	std::vector<BaseReciver*> m_vReciver;
};

template<class TCaller>
void RunBenchmark(std::string sName, TCaller& caller )
{
	unsigned int iTestTime = 100000000;
	auto tpNow = std::chrono::high_resolution_clock::now();
	for (unsigned int i = 0; i < iTestTime; ++i)
		caller.run(10);
	std::cout << sName << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tpNow).count() << std::endl;
}

int main(int argc, char *argv[])
{
	unsigned int iSlotNum = 1;
	{
		QCaller mCaller;

		std::vector<QReciver> vReciver(iSlotNum);
		for(QReciver& rReciver: vReciver)
			QObject::connect(&mCaller, &QCaller::invoke, &rReciver, &QReciver::impl);

		RunBenchmark("Qt Signal / Slot", mCaller);
		std::cout << "  Answer: " << vReciver[0].m_iAnswer << std::endl;
	}

	{
		QCaller mCaller;

		std::vector<BasicReciver> vReciver(iSlotNum);
		for (auto& rReciver : vReciver)
		{
			QObject::connect(&mCaller, &QCaller::invoke, [&rReciver](int iVal) {
				rReciver.impl(iVal);
			});
		}

		RunBenchmark("Qt Signal / Slot (general calss)", mCaller);
		std::cout << "  Answer: " << vReciver[0].m_iAnswer << std::endl;
	}

	{
		BSigCaller mCaller;

		std::vector<BasicReciver> vReciver(iSlotNum);
		for (auto& rReciver : vReciver)
		{
			mCaller.invoke.connect([&rReciver](int iVal) {
				rReciver.impl(iVal);
			});
		}

		RunBenchmark("Boost Signal2", mCaller);
		std::cout << "  Answer: " << vReciver[0].m_iAnswer << std::endl;
	}

	{
		CfuncCaller mCaller;

		std::vector<BasicReciver> vReciver(iSlotNum);
		for (auto& rReciver : vReciver)
		{
			mCaller.m_vReciver.push_back( [&rReciver](int iVal) {
				rReciver.impl(iVal);
			});
		}

		RunBenchmark("std::function", mCaller);
		std::cout << "  Answer: " << vReciver[0].m_iAnswer << std::endl;
	}


	{
		MemCaller mCaller;

		std::vector<BasicReciver> vReciver(iSlotNum);
		for (auto& rReciver : vReciver)
			mCaller.m_vReciver.push_back(&rReciver);

		RunBenchmark("Member pointer", mCaller);
		std::cout << "  Answer: " << vReciver[0].m_iAnswer << std::endl;
	}

	{
		VirCaller mCaller;

		std::vector<ExtReciver> vReciver(iSlotNum);
		for (auto& rReciver : vReciver)
			mCaller.m_vReciver.push_back(&rReciver);

		RunBenchmark("Member pointer with virtual", mCaller);
		std::cout << "  Answer: " << vReciver[0].m_iAnswer << std::endl;
	}
}
