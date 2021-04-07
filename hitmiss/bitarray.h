#ifndef BITARRAY_H
#define BITARRAY_H

#include <vector>
#include <limits>
#include <string>
#include <cassert>
#include <stdexcept>
#include <cmath>

/*
* The class BitArray represents an arbitrary length sequence of bits. It provides access methods for manipulating individual bits
* and can be converted to and from integers and strings (in a binary or hexadecimal notation).
*/

class BitArray
{
public:
	
	using IndexType = std::vector<bool>::size_type;

	enum class Notation
	{
		Binary,
		Hexadecimal
	};

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	explicit BitArray(Integer value) //: BitArray(static_cast<std::size_t>(value > 0 ? std::ceil(std::log2(value)) : 1), false)
	{
		//setNumber(value);
		do
		{
			this->b.push_back(value & 1);
		} while (value >>= 1);
	}

	explicit BitArray(std::size_t length, bool value);

	explicit BitArray(const std::string& s, Notation notation);

	//BitArray(const std::vector<bool>& v);

	explicit BitArray(std::vector<bool>&& v);

	explicit BitArray(std::vector<bool>::const_iterator first, std::vector<bool>::const_iterator last);

	BitArray(const BitArray& other) = default;
	
	BitArray(BitArray&& other) = default;

	BitArray& operator = (const BitArray&) = default;

	BitArray& operator = (BitArray&&) = default;


	BitArray::IndexType getLength() const { return this->b.size(); }

	bool operator [](std::size_t pos) const { return getBit(pos); }

	// std::vector<bool> is special, it does not return a reference but a proxy object
	// https://stackoverflow.com/questions/63476664/what-does-the-error-return-cannot-convert-from-std-vb-referencestd-wrap
	// https://en.cppreference.com/w/cpp/container/vector_bool
	auto operator [](std::size_t pos) {	return this->b[pos]; }

	bool getBit(std::size_t pos) const { return this->b[pos]; }

	void setBit(std::size_t pos, bool value) { this->b[pos] = value; }

	BitArray getBits(std::size_t pos, std::size_t count) const;

	void setBits(std::size_t pos, const BitArray& other);


	// out-of-line and in-class definitions
	// https://stackoverflow.com/questions/40381680/out-of-line-definition-of-template-function-vs-in-class

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	void setNumber(Integer number)	{	setNumber(0, this->b.size(), number);	}

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

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	Integer toNumber() const
	{
		return toNumber<Integer>(0, this->b.size());
	}

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	Integer toNumber(std::size_t pos, std::size_t count) const
	{
		if (pos + count > this->b.size())
			throw std::range_error("Attempting to access bits outside the array.");

		if (std::numeric_limits<Integer>::digits < count)
			throw std::overflow_error("The specified address part can't fit the destination numeric type.");

		Integer num = 0;
		for (std::size_t i = pos; i < pos + count; ++i)
		{
			num |= static_cast<Integer>(this->b[i]) << (i-pos);
		}

		return num;
	}

	std::string toString(Notation notation) const	{ return toString(notation, 0, b.size());	}

	std::string toString(Notation notation, std::size_t pos, std::size_t count) const;
	
	// TODO: define comparison operators
	int compare(std::size_t pos, std::size_t count, const BitArray& other) const;

	explicit operator std::string() const { return toString(Notation::Hexadecimal); }

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	explicit operator Integer() const {	return toNumber<Integer>(); }

private:
	//BitArray_ b;
	std::vector<bool> b;
};	// BitArray


#endif	// BITARRAY_H