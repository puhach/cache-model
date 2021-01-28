#ifndef CPUCACHE_H
#define CPUCACHE_H

#include "cacheset.h"
#include "pipt.h"

#include <vector>
#include <stdexcept>


template <class CacheAccess, class CacheReplacementPolicy>
class CPUCache;


template <class CacheReplacementPolicy>
class CPUCache<PIPT, CacheReplacementPolicy>
{
	//using CacheSetT = CacheSet<PIPT, CacheReplacementPolicy>;

public:
	template <class PIPTParams, class CacheReplacementParams>
	CPUCache(PIPTParams&& accessParams, CacheReplacementParams&& replacementParams)
		: pipt(std::forward<PIPTParams>(accessParams))
		, replacer(std::forward<CacheReplacementPolicy>(replacementParams))
		, sets(pipt.getNumSets(), CacheSet(pipt, replacer))
	{
	}


	bool read(const BitArray& address);
	
	
private:
	PIPT pipt;
	CacheReplacementPolicy replacer;
	std::vector<CacheSet<PIPT, CacheReplacementPolicy>> sets;
};	// CPUCache

template <class ReplacementPolicy>
bool CPUCache<PIPT, ReplacementPolicy>::read(const BitArray& address)
{
	//auto indexPart = address.extract(this->pipt.getOffsetLength(), this->pipt.getIndexLength());
	//auto tagPart = address.extract(this->pipt.getOffsetLength() + this->pipt.getIndexLength(), -1);

	//auto index = indexPart.toNumber();	// TODO: check warnings on x86 and x64

	const auto& tag = address.getBits(this->pipt.getOffsetLength() + this->pipt.getIndexLength(), this->pipt.getTagLength());
	auto index = address.toNumber<std::size_t>(this->pipt.getOffsetLength(), this->pipt.getIndexLength());

	if (index >= sets.size())
		throw std::runtime_error("The indexed entry exceeds the number of sets.");

	auto& cacheSet = this->sets[static_cast<decltype(this->sets)::size_type>(index)];

	bool hit, writeBack;
	return cacheSet.read(tag, hit, writeBack);
}


/*
template <class CacheAccess, class CacheReplacementPolicy>
class CPUCache
{
	//using CacheSetT = ;

public:
	template <class CacheAccessParams, class CacheReplacementParams>
	CPUCache(CacheAccessParams&& accessParams, CacheReplacementParams&& replacementParams)
		: accessParams(std::forward<PIPTParams>(accessParams))
		, replacer(std::forward<CacheReplacementPolicy>(replacementParams))
		, sets(pipt.getNumSets(), CacheSet(pipt, replacer))
	{
	}


	bool read(const BitArray& address);
	

private:
	CacheAccess accessParams;
	CacheReplacementPolicy replacer;
	std::vector<CacheSet<CacheAccess, CacheReplacementPolicy>> sets;
};	// CPUCache

template <class CacheReplacementPolicy>
bool CPUCache<PIPT, CacheReplacementPolicy>::read(const BitArray& address)
{
	//auto indexPart = address.extract(this->pipt.getOffsetLength(), this->pipt.getIndexLength());
		//auto tagPart = address.extract(this->pipt.getOffsetLength() + this->pipt.getIndexLength(), -1);

		//auto index = indexPart.toNumber();	// TODO: check warnings on x86 and x64

	const auto& tag = address.getBits(this->pipt.getOffsetLength() + this->pipt.getIndexLength(), this->pipt.getTagLength());
	auto index = address.toNumber<std::size_t>(this->pipt.getOffsetLength(), this->pipt.getIndexLength());

	if (index >= sets.size())
		throw std::runtime_error("The indexed entry exceeds the number of sets.");

	auto& cacheSet = this->sets[static_cast<decltype(this->sets)::size_type>(index)];

	bool hit, writeBack;
	return cacheSet.read(tag, hit, writeBack);
}*/

#endif	// CPUCACHE_H