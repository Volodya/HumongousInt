#ifndef __HUMONGOUS_INT
#define __HUMONGOUS_INT

//#define __HUMONGOUS_INT_DEBUG

#include <list>
#include <bitset>

#include <cstdint>
#include <type_traits>
#include <limits>

#include <algorithm>

#include <iostream>
#include <string>

class HumongousInt
{
public:
	using Block = uint32_t;
	using BlockList = std::list<Block>;
	static constexpr size_t BITS_IN_BLOCK = 32;
	static constexpr Block MAX_BLOCK = std::numeric_limits<Block>::max();

private:
	BlockList data;
	std::bitset<3> optionBits; // Bit0 - 0=positive, 1=negative; Bit1 - 0=finite, 1=infinite; Bit2 - 0=number, 1=NaN
	bool lastBit() { return data.front() & 1; }
	
	static bool ltData(const BlockList &lhs, const BlockList &rhs, const size_t blocksToConsider);
	static bool ltData(const BlockList &lhs, const BlockList &rhs) { return ltData(lhs, rhs, rhs.size()); }
	static bool leData(const BlockList &lhs, const BlockList &rhs) { return !gtData(rhs, lhs); }
	static bool gtData(const BlockList &lhs, const BlockList &rhs) { return ltData(rhs, lhs); }
	static bool geData(const BlockList &lhs, const BlockList &rhs) { return !ltData(rhs, lhs); }
	static bool eqData(const BlockList &lhs, const BlockList &rhs);
	static bool neData(const BlockList &lhs, const BlockList &rhs) { return !eqData(lhs, rhs); }
		
	static void addData(BlockList &lhs, const BlockList &rhs);
	static bool subData(BlockList &lhs, const BlockList &rhs) { return subData(lhs, rhs, rhs.size()); };
	static bool subData(BlockList &lhs, const BlockList &rhs, size_t blocksToConsider);
	static void incData(BlockList &d);
	static bool decData(BlockList &d);
	static void divData(BlockList &lhs, const BlockList &rhs, const size_t blocksToConsider);
	
	static void addDataWithShift(BlockList &sink, const BlockList &d, size_t n);
public:
	template<typename T> explicit HumongousInt(const T& d);

	explicit HumongousInt(const Block& d = 0);
	explicit HumongousInt(Block&& d);
	HumongousInt(const HumongousInt& that) : data(that.data), optionBits(that.optionBits) {};
	HumongousInt(HumongousInt&& that) : data(std::move(that.data)), optionBits(std::move(that.optionBits)) {};
	HumongousInt& operator=(const HumongousInt&) = default;
	
	void setNaN() { optionBits=4; data.clear(); }
	void setPositiveInfinity() { optionBits=2; data.clear(); }
	void setNegativeInfinity() { optionBits=3; data.clear(); }
	void setZero() { optionBits=0; data.clear(); }
	void setPositive() { optionBits.set(0,0); }
	void setNegative() { optionBits.set(0); }
	
	// comparison
	bool isNaN() const { return !optionBits.test(0) && optionBits.test(1); }
	bool isInfinity() const { return !isNaN() && optionBits.test(1); }
	bool isPositiveInfinity() const { return isPositive() && isInfinity(); }
	bool isNegativeInfinity() const { return isNegative() && isInfinity(); }
	bool isPositive() const { return !isNaN() && !optionBits.test(1); }
	bool isNegative() const { return !isNaN() && optionBits.test(1); }
	bool isZero() const { return !isNaN() && !isInfinity() && !std::any_of(data.begin(), data.end(), [](const Block &b){ return b; }); }
	
	bool operator<(const HumongousInt & that) const;
	bool operator>(const HumongousInt & that) const { return that<(*this); }
	bool operator<=(const HumongousInt & that) const { return !operator>(that); }
	bool operator>=(const HumongousInt & that) const { return !operator<(that); }
	bool operator==(const HumongousInt & that) const { return !operator<(that) && !operator>(that); } // redo this!
	bool operator!=(const HumongousInt & that) const { return !operator==(that); }

	bool operator<(const Block & that) const;
	bool operator>(const Block & that) const;
	bool operator<=(const Block & that) const { return !operator>(that); }
	bool operator>=(const Block & that) const { return !operator<(that); }
	bool operator==(const Block & that) const { return !operator<(that) && !operator>(that); } // redo this!
	bool operator!=(const Block & that) const { return !operator==(that); }

	// not
	bool operator!() const;

	// bitshifts
	HumongousInt& operator<<=(size_t n);
	HumongousInt& operator>>=(size_t n);
	HumongousInt operator<<(size_t n) const { HumongousInt r = *this; r <<= n; return r; }
	HumongousInt operator>>(size_t n) const { HumongousInt r = *this; r >>= n; return r; }

	// mathematical operators
	void negate() { optionBits.flip(0); }
	
	HumongousInt& addWithLeftShift(const HumongousInt & that, size_t n);
	
	HumongousInt& operator+=(const HumongousInt & that);
	HumongousInt& operator-=(const HumongousInt & that);
	HumongousInt& operator*=(const HumongousInt & that);
	HumongousInt& operator/=(const HumongousInt & that);
	HumongousInt operator+(const HumongousInt & that) const { HumongousInt r = *this; r += that; return r; }
	HumongousInt operator-(const HumongousInt & that) const { HumongousInt r = *this; r -= that; return r; }
	HumongousInt operator*(const HumongousInt & that) const { HumongousInt r = *this; r *= that; return r; }
	HumongousInt operator/(const HumongousInt & that) const { HumongousInt r = *this; r /= that; return r; }
	HumongousInt& operator++();
	HumongousInt operator++(int) { HumongousInt r; ++(*this); return r; }
	HumongousInt& operator--();
	HumongousInt operator--(int) { HumongousInt r; --(*this); return r; }
	
	template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>> HumongousInt& operator*=(T d) { return *this *= HumongousInt(d); }
	template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>> HumongousInt& operator+=(T d) { return *this += HumongousInt(d); }

	// bit operations
	HumongousInt& operator&=(const HumongousInt & that);
	HumongousInt& operator|=(const HumongousInt & that);
	HumongousInt operator&(const HumongousInt & that) const { HumongousInt r = *this; r &= that; return r; }
	HumongousInt operator|(const HumongousInt & that) const { HumongousInt r = *this; r |= that; return r; }
	
	HumongousInt operator&(const Block & that) const { return HumongousInt(data.front() & (Block)that); }
	friend HumongousInt operator&(const Block & lhs, const HumongousInt & rhs) { return rhs & lhs; }
	HumongousInt operator|(const Block & that) const { HumongousInt r = *this; r.data.front() |= that; return r; }
	friend HumongousInt operator|(const Block & lhs, const HumongousInt & rhs) { return rhs | lhs; }
	
	template<typename T, typename = std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>>> HumongousInt& operator&=(T d);
	template<typename T, typename = std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>>> HumongousInt& operator|=(T d);
	template<typename T, typename = std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>>> HumongousInt& operator^=(T d);
	
	// conversion
	template<typename T, typename = std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>>> HumongousInt& operator=(T d) { return *this = HumongousInt(d); }
	template<typename T, typename = std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>>> explicit operator T () const;
	explicit operator bool() const;
	
	friend HumongousInt stohi(const std::string &str);
	friend std::string to_string(const HumongousInt& hint);

	// input/output
	friend std::ostream& operator<<(std::ostream& out, const HumongousInt& hint);
	friend std::istream& operator>>(std::istream& in, HumongousInt& hint);
	
	#ifdef __HUMONGOUS_INT_DEBUG
	void debugPrint() const;
	#endif
};
// friend functions declaration
HumongousInt stohi(const std::string &str);
std::string to_string(const HumongousInt& hint);
std::ostream& operator<<(std::ostream& out, const HumongousInt& hint);
std::istream& operator>>(std::istream& in, HumongousInt& hint);

#ifndef __HUMONGOUS_INT_FUNCTIONS
#define __HUMONGOUS_INT_FUNCTIONS
#include "HumongousInt.cpp"
#endif

#endif
