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

		//for (; first != last; ++first)
		for (std::size_t i = 0; i < cacheSet.getSize(); ++i)
		{
			auto& cacheLine = cacheSet[i];

			if (!cacheLine.isValid())
				continue;

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

	//template <typename Iterator>
	//Iterator evict(Iterator first, Iterator last) const
	//{
	//	assert(first < last);
	//	auto n = last - first;
	//	
	//	Iterator evicted;
	//	for (;first != last; ++first)
	//	{
	//		if (!first->isValid())
	//			continue;

	//		auto bits = first->getReplacementBits();
	//		auto rank = bits.toNumber<decltype(n)>();

	//		if (rank == 0)
	//		{
	//			evicted = first;
	//			bits.setNumber(n-1);
	//		}
	//		else
	//		{
	//			--rank;
	//			assert(rank >= 0);
	//			bits.setNumber(rank);
	//			//bits.decrement();
	//		}
	//		
	//		first->setReplacementBits(bits);
	//	}	// for
	//			
	//	assert(evicted != last);
	//	return evicted;
	//}

	//template <typename Iterator>
	//void update(Iterator updatedIt, Iterator first, Iterator last) const
	//{
	//	assert(first < last);

	//	auto n = last - first;

	//	auto ubits = updatedIt->getReplacementBits();
	//	auto urank = ubits.toNumber<decltype(n)>();
	//	//if (rank == n-1)
	//	for (; first != last; ++first)
	//	{
	//		// It looks like we have to update LRU counters even for invalid entries:
	//		// https://classroom.udacity.com/courses/ud007/lessons/632538649/concepts/9391386340923
	//		//if (!first->isValid() || first == updatedIt)
	//		if (first == updatedIt)
	//			continue;

	//		auto bits = first->getReplacementBits();
	//		auto rank = bits.toNumber<decltype(n)>();
	//		if (rank > urank)
	//		//if (bits > ubits)
	//		{
	//			--rank;
	//			assert(rank >= 0);
	//			bits.setNumber(rank);
	//			//bits.decrement();
	//			first->setReplacementBits(bits);
	//		}
	//	}	// while

	//	ubits.setNumber(n-1);
	//	updatedIt->setReplacementBits(ubits);			
	//}	// update

	template <class CacheAccessParams>
	void update(CacheSet<CacheAccessParams, LRU> &cacheSet, std::size_t updatedIndex) const
	{
		auto n = cacheSet.getSize();
		auto ubits = cacheSet[updatedIndex].getReplacementBits();
		auto urank = ubits.toNumber<decltype(n)>();
		//if (rank == n-1)
		//for (; first != last; ++first)
		for (std::size_t i = 0; i < cacheSet.getSize(); ++i)
		{
			// It looks like we have to update LRU counters even for invalid entries:
			// https://classroom.udacity.com/courses/ud007/lessons/632538649/concepts/9391386340923
			//if (!first->isValid() || first == updatedIt)
			//if (first == updatedIt)
			if (i == updatedIndex)
				continue;

			auto bits = cacheSet[i].getReplacementBits();
			auto rank = bits.toNumber<decltype(n)>();
			if (rank > urank)
			{
				--rank;
				assert(rank >= 0);
				bits.setNumber(rank);
				//bits.decrement();
				cacheSet[i].setReplacementBits(bits);
			}
		}	// while

		ubits.setNumber(n - 1);
		cacheSet[updatedIndex].setReplacementBits(ubits);
	}	// update

};	// LRU



#endif	// LRU_H