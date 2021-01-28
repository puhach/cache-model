#ifndef CPUCACHE_H
#define CPUCACHE_H

#include "cacheset.h"
#include "pipt.h"
#include "lru.h"

#include <iostream>
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
	
	// TODO: implement a write
	
	void printStatus();

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


template <class ReplacementPolicy>
void CPUCache<PIPT, ReplacementPolicy>::printStatus()
{
	// In case of any issues check this:
	// https://stackoverflow.com/questions/14637356/static-assert-fails-compilation-even-though-template-function-is-called-nowhere
	static_assert(false, "Printing status is not implemented for this type of cache.");
}

template <>
inline void CPUCache<PIPT, LRU>::printStatus()
{
	for (std::size_t i = 0; i < this->sets.size(); ++i)
	{
		std::cout << "Set " << i << std::endl << std::endl;

		for (std::size_t j = 0; j < this->pipt.getAssociativity(); ++j)
		{
			std::cout << "Line " << j << std::endl;
			std::cout << "Valid: " << this->sets[i][j].isValid()
				<< "\tDirty: " << this->sets[i][j].isDirty()
				<< "\tTag: " << (std::string)this->sets[i][j].getTag()  
				<< "\tLRU: " << (std::string)this->sets[i][j].getReplacementBits() 
				<< std::endl << std::endl;
		}	// j

		std::cout << std::endl;
	}	// for i
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