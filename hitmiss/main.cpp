#include <iostream>
#include <cmath>

/*
Based on whether the index and tag bits use physical address or virtual address, cache can be categorized as:

Physically Indexed, Physically Tagged Cache / PIPT Cache
Physically Indexed, Virtually Tagged Cache / PIVT Cache
Virtually Indexed, Physically Tagged Cache / VIPT Cache
Virtually Indexed, Virtually Tagged Cache / VIVT Cache
*/
enum class CacheAccessing
{
	PIPT, PIVT, VIPT, VIVT
};



template <CacheAccessing accessing, class CacheReplacementPolicy>
class CPUCache;


template <class CacheReplacementPolicy>
class CPUCache<CacheAccessing::PIPT, CacheReplacementPolicy>
{
public:
	//CPUCache(unsigned char addressLen, unsigned char indexBits, unsigned char offsetBits, unsigned char associativity);

	//CPUCache(unsigned long long cacheSize, unsigned long numSets, unsigned long associativity);

	template <typename CacheReplacementParams>
	CPUCache(unsigned long lineSize, unsigned long numSets, unsigned long associativity,
		const CacheReplacementPolicy::Params& replacementParams = CacheReplacementPolicy::Params())
		: associativity(associativity > 0 ? associativity : throw std::invalid_argument("The number of cache lines per set must be positive."))
		, indexBits(numSets > 0 ? static_cast<unsigned long>(std::ceil(std::log2(numSets))) : throw std::invalid_argument("The number of sets must be positive."))
		, offsetBits(lineSize > 0 ? static_cast<unsigned long>(std::ceil(std::log2(lineSize))) : throw std::invalid_argument("The line size must be positive."))
	{

	}

private:
	CacheReplacementPolicy replacer;
};

int main(int argc, char* argv[])
{
	CPUCache<PIPT, LRU> cpuCache(8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /* how many lines per set (associativity) */);
	//CPUCache<PIPT, LRU> cpuCache(12 /*address length*/, 1 /*index bits*/, 4 /* offset bits */, 2 /*associativity*/);

	bool hit = cpuCache.read(address);
	cpuCache.printStatus();

	return 0;
}