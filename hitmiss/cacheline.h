#ifndef CACHELINE_H
#define CACHELINE_H

#include "bitarray.h"

#include <stdexcept>
#include <cassert>

/*
* A cache line is a slot into which a cache block fits. The cache block is the actual data fetched from memory which is inserted 
* into a slot called a cache line.
*/

template <class CacheAccessParams, class CacheReplacementPolicy>
class CacheLine
{
public:
	CacheLine(const CacheAccessParams& accessParams, const CacheReplacementPolicy& replacementPolicy)
		: accessParams(accessParams)	// TODO: we, probably, don't need it as a member variable
		, replacementPolicy(replacementPolicy)
		, b(1 + 1 + accessParams.getTagLength() + replacementPolicy.getReplacementBitLengthPerLine(accessParams), false)
	{

	}

	bool isValid() const
	{
		return this->b.getBit(0);
	}

	void setValid(bool valid)
	{
		this->b.setBit(0, valid);
	}

	bool isDirty() const
	{
		return this->b.getBit(1);
	}

	void setDirty(bool dirty)
	{
		this->b.setBit(1, dirty);
	}

	BitArray getTag() const
	{
		return this->b.getBits(tagOffset, this->accessParams.getTagLength());
	}

	void setTag(const BitArray& tag)
	{
		if (tag.getLength() != this->accessParams.getTagLength())
			throw std::runtime_error("Invalid tag length: " + std::to_string(tag.getLength()));

		this->b.setBits(tagOffset, tag);
	}


	BitArray getReplacementBits() const
	{
		return this->b.getBits(tagOffset + this->accessParams.getTagLength(), 
			this->replacementPolicy.getReplacementBitLengthPerLine(this->accessParams));
	}

	void setReplacementBits(const BitArray& replacementBits)
	{
		if (replacementBits.getLength() != this->replacementPolicy.getReplacementBitLengthPerLine(this->accessParams))
			throw std::runtime_error("Attempting to set the replacement bits of invalid length.");

		this->b.setBits(tagOffset + this->accessParams.getTagLength(), replacementBits);
	}

	bool compareTag(const BitArray& tag) const
	{
		if (tag.getLength() != this->accessParams.getTagLength())
			throw std::runtime_error("Invalid tag length: " + std::to_string(tag.getLength()));

		return this->b.compare(tagOffset, tag.getLength(), tag) == 0;
	}

private:

	static constexpr BitArray::IndexType tagOffset = 1 + 1;

	const CacheAccessParams& accessParams;
	const CacheReplacementPolicy& replacementPolicy;	// TODO: do we really need a replacementPolicy member?
	BitArray b;
};	// CacheLine




#endif	// CACHELINE_H