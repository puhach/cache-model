
#include "bitarray.h"
#include "cpucache.h"
#include "pipt.h"
#include "lru.h"

#include <iostream>


int main(int argc, char* argv[])
{	
	CPUCache<PIPT, LRU> cpuCache(PIPT(12 /*address length (bits)*/, 8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /*lines per set (associativity)*/), LRU());
	//CPUCache<PIPT, LRU> cpuCache(8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /* how many lines per set (associativity) */);
	//CPUCache<PIPT, LRU> cpuCache(12 /*address length*/, 1 /*index bits*/, 4 /* offset bits */, 2 /*associativity*/);

	BitArray address("F17", BitArray::Notation::Hexadecimal);
	bool hit = cpuCache.read(address);
	//cpuCache.printStatus();

	return 0;
}