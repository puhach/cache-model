#ifndef LRU_H
#define LRU_H

#include "bitarray.h"

#include <cassert>
#include <cmath>

class LRU
{
public:
	// TODO: perhaps, we could simply pass in the associativity?
	template <class CacheAccessParams>
	std::size_t getReplacementBitLength(const CacheAccessParams& accessParams) const
	{
		return static_cast<std::size_t>(std::ceil(std::log2(accessParams.getAssociativity())));
	}

	template <typename Iterator>
	Iterator evict(Iterator first, Iterator last) const
	{
		assert(first < last);
		auto n = last - first;
		
		Iterator evicted;
		for (;first != last; ++first)
		{
			if (!first->isValid())
				continue;

			auto bits = first->getReplacementBits();
			auto rank = bits.toNumber<decltype(n)>();

			if (rank == 0)
			{
				evicted = first;
				bits.setNumber(n-1);
			}
			else
			{
				--rank;
				assert(rank >= 0);
				bits.setNumber(rank);
				//bits.decrement();
			}
			
			first->setReplacementBits(bits);
			++first;
		}

		/*
		Iterator evicted;
		while (first != last)
		{
			first->updateReplacementBits([n, &first, &evicted](auto p, auto q) 
			{
				auto rank = BitArray::getNumber<decltype(n)>(p, q);

				if (rank == 0)
				{
					evicted = first;
					BitArray::setNumber(n - 1, p, q);
				}
				else
				{
					--rank;
					assert(rank >= 0);
					//bits.setNumber(rank);
					BitArray::setNumber(rank, p, q);
				}
			});	// updateReplacementBits

			
			//first->setReplacementBits(bits);
			++first;
		}
		*/

		assert(evicted != last);
		return evicted;
	}

	template <typename Iterator>
	void update(Iterator updatedIt, Iterator first, Iterator last) const
	{
		assert(first < last);

		auto n = last - first;

		auto ubits = updatedIt->getReplacementBits();
		auto urank = ubits.toNumber<decltype(n)>();
		//if (rank == n-1)
		for (; first != last; ++first)
		{
			// It looks like we have to update LRU counters even for invalid entries:
			// https://classroom.udacity.com/courses/ud007/lessons/632538649/concepts/9391386340923
			//if (!first->isValid() || first == updatedIt)
			if (first == updatedIt)
				continue;

			auto bits = first->getReplacementBits();
			auto rank = bits.toNumber<decltype(n)>();
			if (rank > urank)
			//if (bits > ubits)
			{
				--rank;
				assert(rank >= 0);
				bits.setNumber(rank);
				//bits.decrement();
				first->setReplacementBits(bits);
			}
		}	// while

		ubits.setNumber(n-1);
		updatedIt->setReplacementBits(ubits);
		

		/*
		updatedIt->updateReplacementBits([n, &first, &last](auto p, auto q)
			{
				auto urank = BitArray::getNumber<decltype(n)>(p, q);
				
				while (first != last)
				{
					first->updateReplacementBits([urank](auto r, auto t) 
						{
							if (rank > urank)
							{
								--rank;
								assert(rank >= 0);
								BitArray::setNumber(rank, r, t);
							}
						});	// updateReplacementBits inner

					BitArray::setNumber(n-1, p, q);
				}
			});	// updateReplacementBits outer
			*/
	}	// update
};



#endif	// LRU_H