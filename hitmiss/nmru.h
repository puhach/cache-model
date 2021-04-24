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

	// TODO: perhaps, we could simply pass in the associativity?
	template <class CacheAccessParams>
	std::size_t getReplacementBitLengthPerSet(const CacheAccessParams& accessParams) const
	{
		return static_cast<std::size_t>(std::ceil(std::log2(accessParams.getAssociativity())));
	}

	template <class CacheAccessParams>
	std::size_t getReplacementBitLengthPerLine(const CacheAccessParams& accessParams) const
	{
		return 0;
	}

	template <class CacheAccessParams>
	std::size_t evict(CacheSet<CacheAccessParams, NMRU> &cacheSet) const
	{
		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_int_distribution<std::size_t> dist(0, cacheSet.getSize()-1);	// closed interval
		std::size_t evicted;

		do
		{
			evicted = dist(engine);
		} while (evicted == cacheSet.getReplacementBits().toNumber<std::size_t>());
		
		// We don't need to evict anything if there are free lines
		assert(cacheSet[evicted].isValid());
		cacheSet.setReplacementBits(BitArray(evicted));
		return evicted;
	}	// evict

	template <class CacheAccessParams>
	void update(CacheSet<CacheAccessParams, NMRU> &cacheSet, std::size_t updatedIndex) const
	{
		cacheSet.setReplacementBits(BitArray(updatedIndex));
	}	// update

};	// NMRU



#endif	// NMRU_H