#ifndef CPUCACHE_H
#define CPUCACHE_H

#include "cacheset.h"
#include "pipt.h"
#include "lru.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <functional>


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

	/// TODO: add a function to invalidate a cache line
	std::pair<bool, bool> invalidate(const BitArray& address) { return update(address, std::mem_fn(&CacheSet<PIPT, CacheReplacementPolicy>::invalidate)); }

	/// TODO: we need both hit and writeBack
	//std::pair<bool, bool> read(const BitArray& address) { return update<false>(address); }
	std::pair<bool, bool> read(const BitArray& address) { return update(address, std::mem_fn(&CacheSet<PIPT, CacheReplacementPolicy>::read)); }
	
	//std::pair<bool, bool> write(const BitArray& address) { return update<true>(address); }
	std::pair<bool, bool> write(const BitArray& address) { return update(address, std::mem_fn(&CacheSet<PIPT, CacheReplacementPolicy>::write)); }
	
	void printStatus();

private:

	//template <bool isWrite>
	//std::pair<bool, bool> update(const BitArray& address);
	///bool update(const BitArray& address);
	template <class F>
	decltype(auto) update(const BitArray& address, F op);
	//auto update(const BitArray& address, F op) -> decltype(op(address));
	//decltype(F(address)) update(const BitArray& address, F op);

	PIPT pipt;
	CacheReplacementPolicy replacer;
	std::vector<CacheSet<PIPT, CacheReplacementPolicy>> sets;
};	// CPUCache


template <class ReplacementPolicy>
template <class F>
//bool CPUCache<PIPT, ReplacementPolicy>::update(const BitArray& address)
//auto CPUCache<PIPT, ReplacementPolicy>::update(const BitArray& address, F op) -> decltype(op(address))
decltype(auto) CPUCache<PIPT, ReplacementPolicy>::update(const BitArray& address, F op)
{
	if (address.getLength() != this->pipt.getAddressLength())
		throw std::invalid_argument("The specified address length does not match the system address length.");

	const auto& tag = address.getBits(this->pipt.getOffsetLength() + this->pipt.getIndexLength(), this->pipt.getTagLength());
	auto index = address.toNumber<std::size_t>(this->pipt.getOffsetLength(), this->pipt.getIndexLength());

	if (index >= sets.size())
		throw std::runtime_error("The indexed entry exceeds the number of sets.");

	auto& cacheSet = this->sets[static_cast<decltype(this->sets)::size_type>(index)];

	//bool hit, writeBack;
	return op(cacheSet, tag);	
}	// update

//template <class ReplacementPolicy>
//template <bool isWrite>
////bool CPUCache<PIPT, ReplacementPolicy>::update(const BitArray& address)
//std::pair<bool, bool> CPUCache<PIPT, ReplacementPolicy>::update(const BitArray& address)
//{
//	if (address.getLength() != this->pipt.getAddressLength())
//		throw std::invalid_argument("The specified address length does not match the system address length.");
//
//	const auto& tag = address.getBits(this->pipt.getOffsetLength() + this->pipt.getIndexLength(), this->pipt.getTagLength());
//	auto index = address.toNumber<std::size_t>(this->pipt.getOffsetLength(), this->pipt.getIndexLength());
//
//	if (index >= sets.size())
//		throw std::runtime_error("The indexed entry exceeds the number of sets.");
//
//	auto& cacheSet = this->sets[static_cast<decltype(this->sets)::size_type>(index)];
//
//	//bool hit, writeBack;
//	if constexpr (isWrite)	// TODO: instead of if-checking, we can pass a pointer to CacheSet::read, CacheSet::write, or CacheSet::invalidate
//		return cacheSet.write(tag);
//	else
//		return cacheSet.read(tag);
//}	// update



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