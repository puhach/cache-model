#ifndef PLRU_H
#define PLRU_H

#include "bitarray.h"
#include "cacheset.h"

#include <cassert>

class PLRU
{
public:
	// TODO: perhaps, we could simply pass in the associativity?
	template <class CacheAccessParams>
	std::size_t getReplacementBitLengthPerLine(const CacheAccessParams& accessParams) const
	{
		//return static_cast<std::size_t>(std::ceil(std::log2(accessParams.getAssociativity())));
		return 1;
	}

	template <class CacheAccessParams>
	std::size_t getReplacementBitLengthPerSet(const CacheAccessParams& accessParams) const
	{
		return 0;
	}


	template <class CacheAccessParams>
	std::size_t evict(CacheSet<CacheAccessParams, PLRU>& cacheSet) const
	{
		auto n = cacheSet.getSize();
		std::size_t evicted = n;
		bool allOne = true;		// all replacement bits equal to 1

		// The Pseudo-LRU (PLRU) replacement policy works by keeping one bit per line in each set. All bits are set to 0, 
		// and they are then set to 1 whenever their block is accessed. On replacement, any of the blocks with a 0 bit is 
		// eligible for replacement. This essentially means that all of the recently accessed blocks will have a 1 bit.
		
		for (std::size_t i = 0; i < cacheSet.getSize(); ++i)
		{
			auto& cacheLine = cacheSet[i];

			// Why would we evict something if there are free lines?
			assert(cacheLine.isValid());
			//if (!cacheLine.isValid())
			//	continue;

			auto bits = cacheLine.getReplacementBits();
			assert(bits.getLength() == 1);
			//auto rank = bits.toNumber<decltype(n)>();
			bool b = bits[0];

			if (!b)
			{
				if (evicted == n)	// nothing evicted so far
				{
					evicted = i;
					//bits.setNumber(n - 1);
					bits[0] = true;
					cacheLine.setReplacementBits(bits);
				}	// evicted == n
				else	// we have already found a line to evict
				{
					allOne = false;		// not all replacement bits are equal to 1
					break;		// no point in further search since we have evicted a line and found a zero bit
				}	// evicted
			}	// b == 0				
		}	// for

		assert(evicted < cacheSet.getSize());

		// When the final 0 bit block is replaced and its bit is set to 1, all of the other block's bits (which should be 1, 
		// since this is the last 0 bit block) are set to 0.
		if (allOne)
			zeroize(cacheSet, evicted);

		return evicted;
	}	// evict



	template <class CacheAccessParams>
	void update(CacheSet<CacheAccessParams, PLRU>& cacheSet, std::size_t updatedIndex) const
	{
		auto n = cacheSet.getSize();
		auto ubits = cacheSet[updatedIndex].getReplacementBits();
		bool allOne = true;

		for (std::size_t i = 0; i < cacheSet.getSize(); ++i)
		{
			if (i == updatedIndex)	// we need to check whether all other lines have 1 bit
				continue;

			auto bits = cacheSet[i].getReplacementBits();
			assert(bits.getLength() == 1);
			bool b = bits[0];

			if (!b)
			{
				allOne = false;
				break;
			}	// !b
		}	// while

		assert(ubits.getLength() == 1);
		ubits[0] = true;
		cacheSet[updatedIndex].setReplacementBits(ubits);

		if (allOne)
			zeroize(cacheSet, updatedIndex);
	}	// update


private:
	template <class CacheAccessParams>
	void zeroize(CacheSet<CacheAccessParams, PLRU>& cacheSet, std::size_t retainedIndex) const
	{
		for (std::size_t i = 0; i < cacheSet.getSize(); ++i)
		{
			if (i == retainedIndex)
				continue;

			auto bits = cacheSet[i].getReplacementBits();
			assert(bits.getLength() == 1);
			assert(bits[0] == true);
			bits[0] = false;
			cacheSet[i].setReplacementBits(bits);
		}	// for
	}	// zeroize
};	// PLRU

#endif	// PLRU_H