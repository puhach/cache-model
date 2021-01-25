#ifndef BITARRAY_H
#define BITARRAY_H

#include <vector>
#include <limits>
#include <string>
#include <algorithm>
#include <cassert>
#include <stdexcept>


class BitArray
{
	//using BitArray_ = std::vector<bool>;

public:
	
	//using Iterator = BitArray_::iterator;
	//using ConstIterator = BitArray_::const_iterator;

	//using IndexType = BitArray_::size_type;
	using IndexType = std::vector<bool>::size_type;

	enum class Notation
	{
		Binary,
		Hexadecimal
	};

	explicit BitArray(std::size_t length, bool value);

	explicit BitArray(const std::string& s, Notation notation);

	//BitArray(const std::vector<bool>& v);

	explicit BitArray(std::vector<bool>&& v);

	explicit BitArray(std::vector<bool>::const_iterator first, std::vector<bool>::const_iterator last);

	// TODO: add copy/move constructors and copy/move assignment operators

	BitArray::IndexType getLength() const { return this->b.size(); }

	// TODO: define the subscript operator

	bool getBit(std::size_t pos) const { return this->b[pos]; }

	void setBit(std::size_t pos, bool value) { this->b[pos] = value; }

	BitArray getBits(std::size_t pos, std::size_t count) const;

	void setBits(std::size_t pos, const BitArray& other);

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	void setNumber(Integer number)
	{
		setNumber(0, this->b.size(), number);
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	void setNumber(std::size_t pos, std::size_t count, Integer number)
	{
		if (pos + count > this->b.size())
			throw std::range_error("Attempting to access bits outside the array.");

		for (std::size_t i = pos; i < pos + count; ++i)
		{
			this->b[i] = number & 1;
			number >>= 1;
		}
	}

	//unsigned long long extractNumber(std::size_t pos, std::size_t count) const;

	// TODO: define the type cast operator
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	Integer toNumber() const
	{
		return toNumber<Integer>(0, this->b.size());
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	Integer toNumber(std::size_t pos, std::size_t count) const
	{
		if (pos + count > this->b.size())
			throw std::range_error("Attempting to access bits outside the address.");

		if (std::numeric_limits<Integer>::digits < count)
			throw std::overflow_error("The specified address part can't fit the destination numeric type.");

		Integer num = 0;
		for (std::size_t i = pos; i < pos + count; ++i)
		{
			num |= static_cast<Integer>(this->b[i]);
			num <<= 1;
		}

		return num;

		//return BitArray::getNumber<Integer>(this->b.begin()+pos, this->b.begin()+pos+count);
	}

	/*
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	static Integer getNumber(BitArray::ConstIterator first, BitArray::ConstIterator last)
	//static Integer getNumber(std::vector<bool>::const_iterator first, std::vector<bool>::const_iterator last)
	{
		if (first >= last)
			throw std::range_error("Invalid range for conversion.");

		if (std::numeric_limits<Integer>::digits() < last-first)
			throw std::overflow_error("The specified address part can't fit the destination numeric type.");

		Integer num = 0;
		while (first != last)
		{
			num |= *first;
			num <<= 1;
			++first;
		}

		return num;
	}*/

	std::string toString() const
	{
		// TODO: not implemented
		return std::string();
	}

	std::string toString(std::size_t pos, std::size_t count) const
	{
		// TODO: not implemented
	}

	// TODO: define comparison operators
	int compare(std::size_t pos, std::size_t count, const BitArray& other) const;

private:
	//BitArray_ b;
	std::vector<bool> b;
};


#endif	// BITARRAY_H