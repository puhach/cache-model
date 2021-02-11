#ifndef CACHESET_H
#define CACHESET_H

#include "cacheline.h"


template <class CacheAccessParams, class CacheReplacementPolicy>
class CacheSet
{
public:
	CacheSet(const CacheAccessParams &accessParams, const CacheReplacementPolicy &replacementPolicy)
		: replacementPolicy(replacementPolicy)
		, lines(accessParams.getAssociativity(), CacheLine(accessParams, replacementPolicy))		
	{}

	std::size_t getSize() const { return this->lines.size(); }

	const CacheLine<CacheAccessParams, CacheReplacementPolicy>& getCacheLine(std::size_t index) const { return this->lines.at(index); }

	CacheLine<CacheAccessParams, CacheReplacementPolicy>& getCacheLine(std::size_t index) { return this->lines.at(index); }

	const CacheLine<CacheAccessParams, CacheReplacementPolicy>& operator [] (std::size_t index) const { return this->lines[index]; }

	CacheLine<CacheAccessParams, CacheReplacementPolicy>& operator [] (std::size_t index) { return this->lines[index]; }

	// TODO: perhaps, pass hit and writeBack by pointers and make them optional
	std::pair<bool, bool> read(const BitArray& tag)	{	return update<false>(tag);	}	

	std::pair<bool, bool> write(const BitArray& tag) {	return update<true>(tag);	}	

private:

	template <bool isWrite>
	//bool update(const BitArray& tag, bool* hit, bool* writeBack)
	std::pair<bool, bool> update(const BitArray& tag)
	{
		//*hit = false;
		//*writeBack = false;
		bool hit = false;
		bool writeBack = false;

		auto lineIt = lines.end();
		for (auto it = lines.begin(); it != lines.end(); ++it)
		{
			if (it->isValid())
			{
				//if (it->getTag() == tag)
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
			lineIt = this->replacementPolicy.evict(lines.begin(), lines.end());
			writeBack = lineIt->isDirty();
			lineIt->setDirty(isWrite);
			lineIt->setTag(tag);
		}
		else	// free line
		{
			this->replacementPolicy.update(lineIt, lines.begin(), lines.end());
			lineIt->setValid(true);
			lineIt->setDirty(isWrite || hit && lineIt->isDirty());		// in case of a hit we have to check whether it was dirty
			lineIt->setTag(tag);
		}	// free line

		//return hit;
		return std::make_pair(hit, writeBack);
	}	// update

	const CacheReplacementPolicy& replacementPolicy;
	std::vector<CacheLine<CacheAccessParams, CacheReplacementPolicy>> lines;
};	// CacheSet



#endif	// CACHESET_H