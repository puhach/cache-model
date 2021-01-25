#ifndef PIPT_H
#define PIPT_H

#include <cmath>
#include <stdexcept>

/*
Based on whether the index and tag bits use physical address or virtual address, cache can be categorized as:

Physically Indexed, Physically Tagged Cache / PIPT Cache
Physically Indexed, Virtually Tagged Cache / PIVT Cache
Virtually Indexed, Physically Tagged Cache / VIPT Cache
Virtually Indexed, Virtually Tagged Cache / VIVT Cache
*/

class PIPT
{
public:
	
	// TODO: consider making it constexpr
	//explicit PIPT(unsigned long lineSize, unsigned long numSets, unsigned long associativity)
	explicit PIPT(std::size_t addressLength, std::size_t lineSize, std::size_t numSets, std::size_t associativity)
		: lineSize(lineSize > 0 ? lineSize : throw std::invalid_argument("The line size must be positive."))
		, numSets(numSets > 0 ? numSets: throw std::invalid_argument("The number of sets must be positive."))
		, associativity(associativity > 0 ? associativity : throw std::invalid_argument("The number of cache lines per set must be positive."))
		, indexLength(static_cast<unsigned char>(std::ceil(std::log2(numSets))))
		, offsetLength(static_cast<unsigned char>(std::ceil(std::log2(lineSize))))
		, tagLength(indexLength+offsetLength <= addressLength ? addressLength-indexLength-offsetLength : 0)
	{

	}

	PIPT(const PIPT& other) = default;

	PIPT(PIPT&& other) = default;

	// TODO: define the copy/move assignment operators

	//unsigned long getLineSize() const { return this->lineSize; }
	std::size_t getLineSize() const { return this->lineSize; }

	//unsigned long getNumSets() const { return this->numSets; }
	std::size_t getNumSets() const { return this->numSets; }

	//unsigned long getAssociativity() const { return this->associativity; }
	std::size_t getAssociativity() const { return this->associativity; }

	std::size_t getTagLength() const { return this->tagLength; }

	//unsigned char getIndexLength() const { return this->indexBits; }
	std::size_t getIndexLength() const { return this->indexLength; }

	//unsigned char getOffsetLength() const { return this->offsetBits; }
	std::size_t getOffsetLength() const { return this->offsetLength; }

private:
	std::size_t lineSize; //unsigned long lineSize;
	std::size_t numSets;//unsigned long numSets;
	std::size_t associativity;//unsigned long associativity;
	std::size_t indexLength, offsetLength, tagLength;//unsigned char indexBits, offsetBits;
};


#endif	// PIPT_H