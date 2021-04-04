#ifndef NMRU_H
#define NMRU_H

#include "bitarray.h"
#include "cacheset.h"

#include <cassert>
#include <cmath>
#include <random>

class NMRU
{
public:

	/*NMRU(std::size_t associativity)
		: replacementBits(static_cast<std::size_t>(std::ceil(std::log2(
			associativity > 0 ? associativity : throw std::invalid_argument("The associativity must be positive.")))),
			false)
	{}*/

	//NMRU() : replacementBits(0, false) {}

	// TODO: define copy/move semantics

	// TODO: perhaps, we could simply pass in the associativity?
	template <class CacheAccessParams>
	std::size_t getReplacementBitLengthPerSet(const CacheAccessParams& accessParams) const
	{
		return static_cast<std::size_t>(std::ceil(std::log2(accessParams.getAssociativity())));
		//return 0;
	}

	template <class CacheAccessParams>
	std::size_t getReplacementBitLengthPerLine(const CacheAccessParams& accessParams) const
	{
		//return static_cast<std::size_t>(std::ceil(std::log2(accessParams.getAssociativity())));
		return 0;
	}


	/*BitArray getReplacementBits() const
	{
		return this->replacementBits;
	}*/

	//template <typename Iterator>
	//Iterator evict(Iterator first, Iterator last) const
	template <class CacheAccessParams>
	std::size_t evict(CacheSet<CacheAccessParams, NMRU> &cacheSet) const
	{
		//assert(first < last);

		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_int_distribution<std::size_t> dist(0, cacheSet.getSize()-1);	// closed interval
		//Iterator evicted;
		std::size_t evicted;

		do
		{
			evicted = dist(engine);
		} while (evicted == cacheSet.getReplacementBits().toNumber<std::size_t>());
		//} while (evicted == this->replacementBits.toNumber<std::size_t>());

		
		// we don't need to evict anything if there are free lines
		assert(cacheSet[evicted].isValid());
		//assert(evicted->isValid());		
		//this->replacementBits.setNumber(idx);
		cacheSet.setReplacementBits(BitArray(evicted));
		return evicted;
	}	// evict

	//template <typename Iterator>
	//void update(Iterator updatedIt, Iterator first, Iterator last) 
	template <class CacheAccessParams>
	void update(CacheSet<CacheAccessParams, NMRU> &cacheSet, std::size_t updatedIndex) const
	{
		//assert(first < last);
		//assert(updatedIndex >= first && updatedIndex < last);
		//this->replacementBits.setNumber(updatedIt - first);
		cacheSet.setReplacementBits(BitArray(updatedIndex));
	}	// update

private:
	//BitArray replacementBits;
};	// NMRU



#endif	// NMRU_H