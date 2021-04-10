#ifndef CPUCACHE_H
#define CPUCACHE_H

#include "cacheset.h"
#include "pipt.h"
#include "lru.h"
#include "nmru.h"
#include "plru.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <functional>


template <class CacheAccess, class CacheReplacementPolicy>
class CPUCache;

/*
* A Physically Indexed, Physically Tagged (PIPT) cache is one that is accessed using a physical address.
* CPU cache comprises multiple cache sets (except for a fully associative cache where there is only one cache
* containing all cache lines). A particular cache set is selected using index bits of the address (there are no
* index bits in case of a fully associative cache).
*/

template <class CacheReplacementPolicy>
class CPUCache<PIPT, CacheReplacementPolicy>
{
public:
	template <class PIPTParams, class CacheReplacementParams>
	CPUCache(PIPTParams&& accessParams, CacheReplacementParams&& replacementParams)
		: pipt(std::forward<PIPTParams>(accessParams))
		, replacer(std::forward<CacheReplacementPolicy>(replacementParams))
		, sets(pipt.getNumSets(), CacheSet(pipt, replacer))
	{
	}

	std::pair<bool, bool> invalidate(const BitArray& address) { return update(address, std::mem_fn(&CacheSet<PIPT, CacheReplacementPolicy>::invalidate)); }

	std::pair<bool, bool> read(const BitArray& address) { return update(address, std::mem_fn(&CacheSet<PIPT, CacheReplacementPolicy>::read)); }
	
	std::pair<bool, bool> write(const BitArray& address) { return update(address, std::mem_fn(&CacheSet<PIPT, CacheReplacementPolicy>::write)); }
	
	void printStatus();

private:

	// Perfectly forward a return type without knowing whether we are dealing with a reference or a value
	// https://stackoverflow.com/questions/24109737/what-are-some-uses-of-decltypeauto
	template <class F>
	decltype(auto) update(const BitArray& address, F op);

	PIPT pipt;
	CacheReplacementPolicy replacer;
	std::vector<CacheSet<PIPT, CacheReplacementPolicy>> sets;
};	// CPUCache


template <class ReplacementPolicy>
template <class F>
decltype(auto) CPUCache<PIPT, ReplacementPolicy>::update(const BitArray& address, F op)
{
	if (address.getLength() != this->pipt.getAddressLength())
		throw std::invalid_argument("The specified address length does not match the system address length.");

	const auto& tag = address.getBits(this->pipt.getOffsetLength() + this->pipt.getIndexLength(), this->pipt.getTagLength());
	auto index = address.toNumber<std::size_t>(this->pipt.getOffsetLength(), this->pipt.getIndexLength());

	if (index >= sets.size())
		throw std::runtime_error("The indexed entry exceeds the number of sets.");

	auto& cacheSet = this->sets[static_cast<decltype(this->sets)::size_type>(index)];

	return op(cacheSet, tag);	
}	// update


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
}	// printStatus


template <>
inline void CPUCache<PIPT, PLRU>::printStatus()
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
				<< "\tPLRU: " << (std::string)this->sets[i][j].getReplacementBits()
				<< std::endl << std::endl;
		}	// j

		std::cout << std::endl;
	}	// for i
}	// printStatus

template <>
inline void CPUCache<PIPT, NMRU>::printStatus()
{
	for (std::size_t i = 0; i < this->sets.size(); ++i)
	{
		std::cout << "Set " << i << std::endl << "NMRU: " << (std::string)this->sets[i].getReplacementBits() << std::endl << std::endl;

		for (std::size_t j = 0; j < this->pipt.getAssociativity(); ++j)
		{
			std::cout << "Line " << j << std::endl;
			std::cout << "Valid: " << this->sets[i][j].isValid()
				<< "\tDirty: " << this->sets[i][j].isDirty()
				<< "\tTag: " << (std::string)this->sets[i][j].getTag()
				<< std::endl << std::endl;
		}	// j

		std::cout << std::endl;
	}	// for i
}	// printStatus


#endif	// CPUCACHE_H