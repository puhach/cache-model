#ifndef LRU_H
#define LRU_H

#include "bitarray.h"
#include "cacheset.h"

#include <cassert>
#include <cmath>

class LRU
{
public:
	// TODO: perhaps, we could simply pass in the associativity?
	template <class CacheAccessParams>
	std::size_t getReplacementBitLengthPerLine(const CacheAccessParams& accessParams) const
	{
		return static_cast<std::size_t>(std::ceil(std::log2(accessParams.getAssociativity())));
	}

	template <class CacheAccessParams>
	std::size_t getReplacementBitLengthPerSet(const CacheAccessParams& accessParams) const
	{
		return 0;
	}


	template <class CacheAccessParams>
	std::size_t evict(CacheSet<CacheAccessParams, LRU>& cacheSet) const
	{
		auto n = cacheSet.getSize();
		std::size_t evicted = n;

		for (std::size_t i = 0; i < cacheSet.getSize(); ++i)
		{
			auto& cacheLine = cacheSet[i];

			// Why would we evict something if there are free lines?
			assert(cacheLine.isValid());
			//if (!cacheLine.isValid())
			//	continue;

			auto bits = cacheLine.getReplacementBits();
			auto rank = bits.toNumber<decltype(n)>();

			if (rank == 0)
			{
				evicted = i;
				bits.setNumber(n-1);
			}
			else
			{
				--rank;
				assert(rank >= 0);
				bits.setNumber(rank);
			}
					
			cacheLine.setReplacementBits(bits);
		}	// for
						
		assert(evicted < cacheSet.getSize());
		return evicted;
	}	// evict


	template <class CacheAccessParams>
	void update(CacheSet<CacheAccessParams, LRU> &cacheSet, std::size_t updatedIndex) const
	{
		auto n = cacheSet.getSize();
		auto ubits = cacheSet[updatedIndex].getReplacementBits();
		auto urank = ubits.toNumber<decltype(n)>();

		for (std::size_t i = 0; i < cacheSet.getSize(); ++i)
		{
			// It looks like we have to update LRU counters even for invalid entries:
			// https://classroom.udacity.com/courses/ud007/lessons/632538649/concepts/9391386340923
			if (i == updatedIndex)
				continue;

			auto bits = cacheSet[i].getReplacementBits();
			auto rank = bits.toNumber<decltype(n)>();
			if (rank > urank)
			{
				--rank;
				assert(rank >= 0);
				bits.setNumber(rank);
				cacheSet[i].setReplacementBits(bits);
			}
		}	// while

		ubits.setNumber(n - 1);
		cacheSet[updatedIndex].setReplacementBits(ubits);
	}	// update

};	// LRU



#endif	// LRU_H