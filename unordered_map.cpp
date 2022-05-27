#include <iostream>
#include <random>
#include <map>
#include <unordered_map>
#include <chrono>
#include <vector>

template<typename TFunc>
size_t Benchmark(TFunc func)
{
	using nsClock = std::chrono::high_resolution_clock;
	auto tpStart = nsClock::now();

	func();

	return std::chrono::duration_cast<std::chrono::milliseconds>(nsClock::now() - tpStart).count();
}

template<typename TData>
void test(const std::vector<size_t>& vData, const std::vector<size_t>& vSearch)
{
	TData m;
	std::cout << " Insert, use " << Benchmark([&]() {
		for (size_t i : vData)
			m[i] = 1;
		}) << " ms" << std::endl;
	std::cout << "  size " << m.size() << std::endl;

	size_t uCount = 0;
	std::cout << " Lookup, use " << Benchmark([&]() {
		for (size_t i : vSearch)
			uCount += m[i];
		}) << "ms " << std::endl;
	std::cout << " res: " << uCount << "\n";
}

int main()
{
	std::random_device ranDevice;
	std::vector<size_t> vIndex;
	for (size_t i = 0; i < 100'000; ++i)
		vIndex.emplace_back(ranDevice());

	std::vector<size_t> vSIndex;
	for (size_t i = 0; i < 100'000; ++i)
		vSIndex.emplace_back(ranDevice());

	std::cout << "Ordered map test\n";
	test<std::map<size_t, size_t>>(vIndex, vSIndex);

	std::cout << "\nUnordered map test\n";
	test<std::unordered_map<size_t, size_t>>(vIndex, vSIndex);
}
