#ifndef CACHESET_H
#define CACHESET_H

#include "cacheline.h"

/*
* A cache set consists of one or more cache lines. The number of cache lines is defined by cache associativity.
*/

template <class CacheAccessParams, class CacheReplacementPolicy>
class CacheSet
{
public:
	CacheSet(const CacheAccessParams &accessParams, const CacheReplacementPolicy &replacementPolicy)
		: replacementPolicy(replacementPolicy)
		, replacementBits(replacementPolicy.getReplacementBitLengthPerSet(accessParams))
		, lines(accessParams.getAssociativity(), CacheLine(accessParams, replacementPolicy))		
	{}

	std::size_t getSize() const { return this->lines.size(); }

	const CacheLine<CacheAccessParams, CacheReplacementPolicy>& getCacheLine(std::size_t index) const { return this->lines.at(index); }

	CacheLine<CacheAccessParams, CacheReplacementPolicy>& getCacheLine(std::size_t index) { return this->lines.at(index); }

	const CacheLine<CacheAccessParams, CacheReplacementPolicy>& operator [] (std::size_t index) const { return this->lines[index]; }

	CacheLine<CacheAccessParams, CacheReplacementPolicy>& operator [] (std::size_t index) { return this->lines[index]; }


	BitArray getReplacementBits() const { return this->replacementBits; }

	void setReplacementBits(const BitArray& replacementBits)
	{
		if (replacementBits.getLength() != this->replacementBits.getLength())
			throw std::runtime_error("Attempting to set the replacement bits of invalid length.");

		this->replacementBits = replacementBits;
	}

	// TODO: consider adding an rvalue-reference overload


	std::pair<bool, bool> read(const BitArray& tag)	{	return update<false>(tag);	}	

	std::pair<bool, bool> write(const BitArray& tag) {	return update<true>(tag);	}	

	std::pair<bool, bool> invalidate(const BitArray& tag);

private:

	template <bool isWrite>
	std::pair<bool, bool> update(const BitArray& tag)
	{
		bool hit = false;
		bool writeBack = false;

		auto lineIt = lines.end();
		for (auto it = lines.begin(); it != lines.end(); ++it)
		{
			if (it->isValid())
			{
				if (it->compareTag(tag))
				{
					hit = true;
					writeBack = false;
					lineIt = it;
					break;
				}	// tags match
			}	// valid
			else lineIt = it;
		}

		if (lineIt == lines.end())		// no free line
		{
			std::size_t evictedIdx = this->replacementPolicy.evict(*this);
			lineIt = lines.begin() + evictedIdx;
			writeBack = lineIt->isDirty();
			lineIt->setDirty(isWrite);
			lineIt->setTag(tag);
		}
		else	// free line
		{
			this->replacementPolicy.update(*this, lineIt - lines.begin());
			lineIt->setValid(true);
			lineIt->setDirty(isWrite || hit && lineIt->isDirty());		// in case of a hit we have to check whether it was dirty
			lineIt->setTag(tag);
		}	// free line

		//return hit;
		return std::make_pair(hit, writeBack);
	}	// update

	const CacheReplacementPolicy& replacementPolicy;
	BitArray replacementBits;
	std::vector<CacheLine<CacheAccessParams, CacheReplacementPolicy>> lines;
};	// CacheSet

template <class CacheAccessParams, class CacheReplacementPolicy>
std::pair<bool, bool> CacheSet<CacheAccessParams, CacheReplacementPolicy>::invalidate(const BitArray& tag)
{
	for (auto it = lines.begin(); it != lines.end(); ++it)
	{
		if (it->isValid())
		{
			if (it->compareTag(tag))
			{
				bool writeBack = it->isDirty();
				it->setValid(false);
				return std::make_pair(true, writeBack);
			}	// tags match
		}	// valid
	}	// for

	return std::make_pair(false, false);
}	// invalidate

#endif	// CACHESET_H