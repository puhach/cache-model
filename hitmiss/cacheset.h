#ifndef CACHESET_H
#define CACHESET_H

#include "cacheline.h"


template <class CacheAccessParams, class CacheReplacementPolicy>
class CacheSet
{
	//using CacheLineT = ;

public:
	CacheSet(const CacheAccessParams &accessParams, const CacheReplacementPolicy &replacementPolicy)
		: replacementPolicy(replacementPolicy)
		, lines(accessParams.getAssociativity(), CacheLine(accessParams, replacementPolicy))		
	{}

	// TODO: perhaps, pass hit and writeBack by pointers and make them optional
	bool read(const BitArray& tag, bool &hit, bool &writeBack)
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
				//if (it->getTag() == tag)
				if (it->compareTag(tag))
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

		if (lineIt == lines.end())		// no free line
		{
			lineIt = this->replacementPolicy.evict(lines.begin(), lines.end());
			//lineIt->read(tag, hit, writeBack);
			writeBack = lineIt->isDirty();
			lineIt->setTag(tag);
		}
		else	// free line
		{
			//invalidIt->read(tag);
			this->replacementPolicy.update(lineIt, lines.begin(), lines.end());
			lineIt->setValid(true);
			lineIt->setDirty(false);
			lineIt->setTag(tag);
		}	// free line

		return hit;
	}

private:
	const CacheReplacementPolicy& replacementPolicy;
	std::vector<CacheLine<CacheAccessParams, CacheReplacementPolicy>> lines;
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


#endif	// CACHESET_H