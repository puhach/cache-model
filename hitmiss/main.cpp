
#include "bitarray.h"
#include "cpucache.h"
#include "pipt.h"
#include "lru.h"
#include "nmru.h"

#include <iostream>


int main(int argc, char* argv[])
{	
	static const std::string yn[] = { "No", "Yes" };

	try
	{
		CPUCache<PIPT, LRU> cpuCache(
			PIPT(12 /*address length (bits)*/, 8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /*lines per set (associativity)*/), 
			LRU());
		//CPUCache<PIPT, NMRU> cpuCache(PIPT(12 /*address length (bits)*/, 8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /*lines per set (associativity)*/), NMRU());
		//CPUCache<PIPT, PLRU> cpuCache(
		//	PIPT(12 /*address length (bits)*/, 8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /*lines per set (associativity)*/), 
		//	PLRU());
		//CPUCache<PIPT, LRU> cpuCache(8 /*line size (bytes)*/, 2 /*how many sets*/, 2 /* how many lines per set (associativity) */);
		//CPUCache<PIPT, LRU> cpuCache(12 /*address length*/, 1 /*index bits*/, 4 /* offset bits */, 2 /*associativity*/);

		auto [hit, writeBack] = cpuCache.write(BitArray("000", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F00", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("000", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F1F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("01F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("01F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("F1F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F17", BitArray::Notation::Hexadecimal));

		/*
		Expected output:

			Set 0:
			Tag F1 LRU=1 V=1 D=0
			Tag F0 LRU=0 V=1 D=0

			Set 1:
			Tag 01 LRU=1 V=0 D=1
			Tag F1 LRU=0 V=0 D=1

		Hit: No
		Write-back: No
		*/
		cpuCache.printStatus();
		std::cout << "Hit: " << yn[hit] << std::endl << "Write-back: " << yn[writeBack] << std::endl << std::endl;


		///BitArray address("F17", BitArray::Notation::Hexadecimal);
		//auto [hit, writeBack] = cpuCache.read(BitArray("F00", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("000", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F00", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("000", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F1F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("01F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("01F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("F1F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F18", BitArray::Notation::Hexadecimal));

		/*
		Expected output:

			Set 0: 
			Tag=00 V=0 D=1 LRU=0
			Tag=F0 V=1 D=0 LRU=1

			Set 1: 
			Tag=01 V=0 D=1 LRU=0
			Tag=F1 V=1 D=0 LRU=1

		Hit: No
		Write-back: No
		*/
		cpuCache.printStatus();
		std::cout << "Hit: " << yn[hit] << std::endl << "Write-back: " << yn[writeBack] << std::endl << std::endl;


		std::tie(hit, writeBack) = cpuCache.write(BitArray("000", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F00", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("000", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F1F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("01F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("01F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("F1F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F08", BitArray::Notation::Hexadecimal));

		/*
		Expected output:

			Set 0:
			Tag=00 V=0 D=1 LRU=0
			Tag=F0 V=1 D=0 LRU=1

			Set 1:
			Tag=01 V=0 D=1 LRU=0
			Tag=F0 V=1 D=0 LRU=1

		Hit: No
		Write-back: No
		*/
		cpuCache.printStatus();
		std::cout << "Hit: " << yn[hit] << std::endl << "Write-back: " << yn[writeBack] << std::endl << std::endl;


		std::tie(hit, writeBack) = cpuCache.write(BitArray("000", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F00", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("000", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F1F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("01F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("01F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.invalidate(BitArray("F1F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F07", BitArray::Notation::Hexadecimal));

		/*
		Expected out:

			Set 0:
			Tag=00 V=0 D=1 LRU=0 
			Tag=F0 V=1 D=0 LRU=1

			Set 1:
			Tag=01 V=0 D=1 LRU=1
			Tag=F1 V=0 D=1 LRU=0

		Hit: Yes
		Write-back: No
		*/
		cpuCache.printStatus();
		std::cout << "Hit: " << yn[hit] << std::endl << "Write-back: " << yn[writeBack] << std::endl << std::endl;


		std::tie(hit, writeBack) = cpuCache.write(BitArray("F10", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F00", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F3F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F2F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F1F", BitArray::Notation::Hexadecimal));

		/*
		Expected output:

			Set 0:
			Tag=F0 V=1 D=1 LRU=1
			Tag=F1 V=1 D=1 LRU=0

			Set 1:
			Tag=F2 V=1 D=1 LRU=0
			Tag=F1 V=1 D=0 LRU=1

		Hit: No
		Write-back: Yes
		*/
		cpuCache.printStatus();
		std::cout << "Hit: " << yn[hit] << std::endl << "Write-back: " << yn[writeBack] << std::endl << std::endl;


		std::tie(hit, writeBack) = cpuCache.write(BitArray("F10", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F00", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F3F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F2F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F20", BitArray::Notation::Hexadecimal));

		/*
		Expected output:

			Set 0:
			Tag=F0 V=1 D=1 LRU=0
			Tag=F2 V=1 D=0 LRU=1

			Set 1:
			Tag=F2 V=1 D=1 LRU=1
			Tag=F3 V=1 D=1 LRU=0

		Hit: No
		Write-back: Yes
		*/
		cpuCache.printStatus();
		std::cout << "Hit: " << yn[hit] << std::endl << "Write-back: " << yn[writeBack] << std::endl << std::endl;


		std::tie(hit, writeBack) = cpuCache.write(BitArray("AA0", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F10", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F00", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F3F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("AAF", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F3F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F2F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.read(BitArray("F3F", BitArray::Notation::Hexadecimal));
		std::tie(hit, writeBack) = cpuCache.write(BitArray("F00", BitArray::Notation::Hexadecimal));

		/*
		Expected out:

			Set 0:
			Tag=F0 V=1 D=1 LRU=1
			Tag=F1 V=1 D=1 LRU=0

			Set 1:
			Tag=F2 V=1 D=0 LRU=0
			Tag=F3 V=1 D=1 LRU=1

		Hit: Yes
		Write-back: No
		*/
		cpuCache.printStatus();
		std::cout << "Hit: " << yn[hit] << std::endl << "Write-back: " << yn[writeBack] << std::endl << std::endl;

		return 0;
	}	// try
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}
}