#include <iostream>
#include <cmath>

/*
Based on whether the index and tag bits use physical address or virtual address, cache can be categorized as:

Physically Indexed, Physically Tagged Cache / PIPT Cache
Physically Indexed, Virtually Tagged Cache / PIVT Cache
Virtually Indexed, Physically Tagged Cache / VIPT Cache
Virtually Indexed, Virtually Tagged Cache / VIVT Cache
*/

class PIPT
{
public:
	
	explicit PIPT(unsigned long lineSize, unsigned long numSets, unsigned long associativity)
		: lineSize(lineSize > 0 ? lineSize : throw std::invalid_argument("The line size must be positive."))
		, numSets(numSets > 0 ? numSets: throw std::invalid_argument("The number of sets must be positive."))
		, associativity(associativity > 0 ? associativity : throw std::invalid_argument("The number of cache lines per set must be positive."))
		, indexBits(static_cast<unsigned char>(std::ceil(std::log2(numSets))))
		, offsetBits(static_cast<unsigned char>(std::ceil(std::log2(lineSize))))
	{

	}

	PIPT(const PIPT& other) = default;

	PIPT(PIPT&& other) = default;

private:
	unsigned long lineSize;
	unsigned long numSets;
	unsigned long associativity;
	unsigned char indexBits, offsetBits;
};


template <class CacheAccessing, class CacheReplacementPolicy>
class CPUCache;

/*
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
*/

template <class CacheReplacementPolicy>
class CPUCache<PIPT, CacheReplacementPolicy>
{
public:
	template <class PIPTParams, class CacheReplacementParams>
	CPUCache(PIPTParams&& accessParams, CacheReplacementParams&& replacementParams = CacheReplacementParams())
		: pipt(std::forward<PIPTParams>(accessParams))
		, replacer(std::forward<CacheReplacementPolicy>(replacementParams))
	{
	}

private:
	PIPT pipt;
	CacheReplacementPolicy replacer;
};

class LRU
{

};

int main(int argc, char* argv[])
{	
	CPUCache<PIPT, LRU> cpuCache(PIPT(8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /*lines per set (associativity)*/), LRU());
	//CPUCache<PIPT, LRU> cpuCache(8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /* how many lines per set (associativity) */);
	//CPUCache<PIPT, LRU> cpuCache(12 /*address length*/, 1 /*index bits*/, 4 /* offset bits */, 2 /*associativity*/);

	//bool hit = cpuCache.read(address);
	//cpuCache.printStatus();

	return 0;
}