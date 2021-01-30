#ifndef CACHELINE_H
#define CACHELINE_H

#include "bitarray.h"

#include <stdexcept>
#include <cassert>


template <class CacheAccessParams, class CacheReplacementPolicy>
class CacheLine
{
public:
	CacheLine(const CacheAccessParams& accessParams, const CacheReplacementPolicy& replacementPolicy)
		: accessParams(accessParams)
		, replacementPolicy(replacementPolicy)
		, b(1 + 1 + accessParams.getTagLength() + replacementPolicy.getReplacementBitLength(accessParams), false)
	{

	}

	bool isValid() const
	{
		//return this->b[0];
		return this->b.getBit(0);
	}

	void setValid(bool valid)
	{
		//this->b[0] = valid;
		this->b.setBit(0, valid);
	}

	bool isDirty() const
	{
		//return this->b[1];
		return this->b.getBit(1);
	}

	void setDirty(bool dirty)
	{
		//this->b[1] = dirty;
		this->b.setBit(1, dirty);
	}

	BitArray getTag() const
	{
		//BitArray tag(this->accessParams.getTagLength());
		//std::copy(this->b.begin()+tagOffset, this->b.begin()+tagOffset+this->accessParams.getTagLength(), tag.begin());
		//return tag;
		//return BitArray(this->b.begin() + tagOffset, this->b.begin() + tagOffset + this->accessParams.getTagLength());
		return this->b.getBits(tagOffset, this->accessParams.getTagLength());
	}


	void setTag(const BitArray& tag)
	{
		if (tag.getLength() != this->accessParams.getTagLength())
			//throw std::runtime_error("Invalid tag length: " + tag.toString());
			throw std::runtime_error("Invalid tag length: " + std::to_string(tag.getLength()));

		//std::copy(tag.begin(), tag.end(), this->b.begin() + tagOffset);
		this->b.setBits(tagOffset, tag);
	}


	BitArray getReplacementBits() const
	{
		//BitArray replacementBits(this->replacementPolicy.getReplacementBitLength(this->accessParams));
		//auto from = this->b.begin() + tagOffset + this->accessParams.getTagLength();
		//std::copy(from, from+this->accessParams.getReplacementBits(), replacementBits.begin());
		//return replacementBits;
		//return BitArray(from, from + this->accessParams.getReplacementBits());
		return this->b.getBits(tagOffset + this->accessParams.getTagLength(), this->replacementPolicy.getReplacementBitLength(this->accessParams));
	}

	void setReplacementBits(const BitArray& replacementBits)
	{
		if (replacementBits.getLength() != this->replacementPolicy.getReplacementBitLength(this->accessParams))
			throw std::runtime_error("Attempting to set the replacement bits of invalid length.");

		//std::copy(replacementBits.begin(), replacementBits.end(), this->b.begin()+tagOffset+this->accessParams.getTagLength());
		//return replacementBits;
		this->b.setBits(tagOffset + this->accessParams.getTagLength(), replacementBits);
	}

	bool compareTag(const BitArray& tag) const
	{
		if (tag.getLength() != this->accessParams.getTagLength())
			//throw std::runtime_error("Invalid tag length: " + tag.toString());
			throw std::runtime_error("Invalid tag length: " + std::to_string(tag.getLength()));

		//return std::equal(tag.begin(), tag.end(), this->b.begin()+tagOffset);
		//return this->b.equals(tagOffset, tag.getLength(), tag);
		return this->b.compare(tagOffset, tag.getLength(), tag) == 0;
	}

private:

	static constexpr BitArray::IndexType tagOffset = 1 + 1;

	/*bool valid;
	bool dirty;
	unsigned long long tag;
	unsigned long lruCounter;*/
	const CacheAccessParams& accessParams;
	const CacheReplacementPolicy& replacementPolicy;
	//std::vector<bool> b;
	BitArray b;
};




#endif	// CACHELINE_H