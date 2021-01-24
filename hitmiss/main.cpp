#include <iostream>
#include <vector>
#include <cmath>

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
	explicit PIPT(unsigned long lineSize, unsigned long numSets, unsigned long associativity)
		: lineSize(lineSize > 0 ? lineSize : throw std::invalid_argument("The line size must be positive."))
		, numSets(numSets > 0 ? numSets: throw std::invalid_argument("The number of sets must be positive."))
		, associativity(associativity > 0 ? associativity : throw std::invalid_argument("The number of cache lines per set must be positive."))
		, indexBits(static_cast<unsigned char>(std::ceil(std::log2(numSets))))
		, offsetBits(static_cast<unsigned char>(std::ceil(std::log2(lineSize))))
	{

	}

	PIPT(const PIPT& other) = default;

	PIPT(PIPT&& other) = default;

	// TODO: define the copy/move assignment operators

	unsigned long getLineSize() const { return this->lineSize; }

	unsigned long getNumSets() const { return this->numSets; }

	unsigned long getAssociativity() const { return this->associativity; }

	unsigned char getIndexLength() const { return this->indexBits; }

	unsigned char getOffsetLength() const { return this->offsetBits; }

private:
	unsigned long lineSize;
	unsigned long numSets;
	unsigned long associativity;
	unsigned char indexBits, offsetBits;
};


template <class CacheAccessing, class CacheReplacementPolicy>
class CPUCache;


/*
class CacheLine
{
public:
	CacheLine() {}

private:
	bool valid = false;
	bool dirty = false;
	unsigned long long tag = 0;
};

class CacheSet
{
public:
	CacheSet(unsigned long associativity)
		: lines(associativity)
	{

	}

private:
	std::vector<CacheLine> lines;
};
*/

template <class CacheAccessParams, class CacheReplacementPolicy>
class CacheLine
{
public:
	CacheLine(const CacheAccessParams& accessParams, const CacheReplacementPolicy& replacementPolicy)
		: accessParams(accessParams)
		, replacementPolicy(replacementPolicy)
		, b(1 + 1 + accessParams.getTagLength() + accessParams.getIndexLength() + accessParams.getOffsetLength() + accessParams.getReplacementBitLength(), false)
	{

	}

	BitArray getTag() const
	{
		BitArray tag(this->accessParams.getTagLength());
		std::copy(this->b.begin()+tagOffset, this->b.begin()+tagOffset+this->accessParams.getTagLength(), tag.begin());
		return tag;
	}

	void setTag(const BitArray& tag)
	{
		if (tag.getLength() != this->accessParams.getTagLength())
			throw std::runtime_error("Invalid tag length: " + tag.toString());

		std::copy(tag.begin(), tag.end(), this->b.begin() + tagOffset);
	}



private:

	static constexpr int tagOffset = 1 + 1;

	/*bool valid;
	bool dirty;
	unsigned long long tag;
	unsigned long lruCounter;*/
	const CacheAccessParams& accessParams;
	const CacheReplacementPolicy& replacementPolicy;
	//std::vector<bool> b;
	BitArray b;
};


template <class CacheAccessParams, class CacheReplacementPolicy>
class CacheSet
{
	using CacheLineT = CacheLine<CacheAccessParams, CacheReplacementPolicy>;

public:
	CacheSet(const CacheAccessParams &accessParams, const CacheReplacementPolicy &replacementPolicy)
		: lines(accessParams, replacementPolicy)
	{}

	bool read(const Address& tag, bool &hit, bool &writeBack)
	{
		//CacheLineT::IndexType invalidIdx = -1;
		//for (CacheLineT::Index)

		hit = false;
		writeBack = false;

		auto lineIt = lines.end();
		for (auto it = lines.begin(); it != lines.end(); ++it)
		{
			if (it->isValid())
			{
				if (it->getTag() == tag)
				{
					//replacementPolicy.updateEntries(it, lines.begin(), lines.end());
					//it->read(tag);
					//return true;
					hit = true;
					writeBack = false;
					lineIt = it;
					break;
				}	// tags match
			}	// valid
			else lineIt = it;
		}

		if (lineIt == lines.end)		// no free line
		{
			lineIt = replacementPolicy.evict(lines.begin(), lines.end());
			//lineIt->read(tag, hit, writeBack);
			writeBack = lineIt->isDirty();
			lineIt->setTag(tag);
		}
		else	// free line
		{
			//invalidIt->read(tag);
			replacementPolicy.updateEntries(lineIt, lines.begin(), lines.end());
			lineIt->setValid(true);
			lineIt->setDirty(false);
			lineIt->setTag(tag);
		}	// free line

		return hit;
	}

private:
	std::vector<CacheLineT> lines;
};

//template <class CacheReplacementPolicy>
//class CacheSet<PIPT, CacheReplacementPolicy>
//{
//public:
//	CacheSet(const PIPT& accessParams, const CacheReplacementPolicy& replacementParams)
//		: lines(accessParams.getAssociativity(), CacheLine(accessParams, replacementParams))
//	{
//
//	}
//
//private:
//	std::vector<CacheLine<PIPT, CacheReplacementPolicy>> lines;
//};

template <class CacheReplacementPolicy>
class CPUCache<PIPT, CacheReplacementPolicy>
{
	using CacheSetT = CacheSet<PIPT, CacheReplacementPolicy>;

public:
	template <class PIPTParams, class CacheReplacementParams>
	CPUCache(PIPTParams&& accessParams, CacheReplacementParams&& replacementParams)
		: pipt(std::forward<PIPTParams>(accessParams))
		, replacer(std::forward<CacheReplacementPolicy>(replacementParams))
		, sets(pipt.getNumSets(), pipt.getAssociativity())
	{
	}

	bool read(const Address& address)
	{
		auto indexPart = address.extract(this->pipt.getOffsetLength(), this->pipt.getIndexLength());
		auto tagPart = address.extract(this->pipt.getOffsetLength() + this->pipt.getIndexLength(), -1);
		
		auto index = indexPart.toNumber();	// TODO: check warnings on x86 and x64
		if (index >= sets.size())	// beware type truncation
			throw std::runtime_error("The indexed entry exceeds the number of sets.");

		auto& cacheSet = sets[static_cast<CacheSetT::IndexType>(index)];
		
		return cacheSet.read(tagPart);
	}

	
private:
	PIPT pipt;
	CacheReplacementPolicy replacer;
	std::vector<CacheSetT> sets;
};



class LRU
{

};

int main(int argc, char* argv[])
{	
	CPUCache<PIPT, LRU> cpuCache(PIPT(8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /*lines per set (associativity)*/), LRU());
	//CPUCache<PIPT, LRU> cpuCache(8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /* how many lines per set (associativity) */);
	//CPUCache<PIPT, LRU> cpuCache(12 /*address length*/, 1 /*index bits*/, 4 /* offset bits */, 2 /*associativity*/);

	//bool hit = cpuCache.read(address);
	//cpuCache.printStatus();

	return 0;
}