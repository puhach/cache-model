#include "bitarray.h"


BitArray::BitArray(std::size_t length, bool value)
	: b(length, value)
{
}

BitArray::BitArray(const std::string& s, Notation notation)
{
	if (s.empty())
		throw std::invalid_argument("The specified input string is empty.");

	if (notation == Notation::Binary)
	{
		this->b.resize(s.size());
		std::transform(s.rbegin(), s.rend(), this->b.begin(), [](const auto& c)
			{
				return c == '1';
			});
	}	// binary
	else if (notation == Notation::Hexadecimal)
	{
		this->b.reserve(s.size() * 4);
		//for (auto c : s)
		for (auto it = s.rbegin(); it != s.rend(); ++it)
		{
			char c = std::toupper(*it);

			std::size_t hexval;
			if (c >= 'A' && c <= 'F')
				hexval = 10U + c - 'A';
			else if (c >= '0' && c <= '9')
				hexval = c - '0';
			else throw std::invalid_argument("Wrong hex character: " + c);

			for (auto i = 0; i < 4; ++i)
			{
				this->b.push_back(hexval & 1);
				hexval >>= 1;
			}
		}	// for each char

		assert(this->b.size() == s.size() * 4);
	}	// hexadecimal
	else throw std::invalid_argument("The address type is not supported.");
}

//BitArray::BitArray(const std::vector<bool>& v)
//	: b(v)
//{
//}

BitArray::BitArray(std::vector<bool>&& v)
	: b(std::move(v))
{
}

BitArray::BitArray(std::vector<bool>::const_iterator first, std::vector<bool>::const_iterator last)
{
	if (first >= last)
		throw std::range_error("Invalid input range.");

	this->b.resize(last-first);
	std::copy(first, last, this->b.begin());
}

BitArray BitArray::getBits(std::size_t pos, std::size_t count) const
{
	if (pos + count > this->b.size())
		throw std::range_error("Attempting to extract bits outside the array.");

	std::vector<bool> bits(count);
	std::copy(this->b.begin()+pos, this->b.begin()+pos+count, bits.begin());
	return BitArray(std::move(bits));
}

void BitArray::setBits(std::size_t pos, const BitArray& bits)
{
	if (pos + bits.getLength() > this->b.size())
		throw std::range_error("Attempting to set bits outside the array");

	std::copy(bits.b.begin(), bits.b.end(), this->b.begin()+pos);
}

int BitArray::compare(std::size_t pos, std::size_t count, const BitArray& other) const
{
	if (pos + count > this->b.size())
		throw std::range_error("Invalid range for comparison.");

	if (count < other.getLength())
		return -1;	// shorter one is less
	else if (count > other.getLength())
		return +1;	// longer one is greater
	//else for (auto it = this->b.cbegin() + pos; it != this->b.cend(); ++it)
	else for (std::size_t i = 0; i < count; ++i)
	{
		if (other.b[i] != this->b[pos + i])
			return other.b[i] ? -1 : +1;
	}

	return 0;
}

