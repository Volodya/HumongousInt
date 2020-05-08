#ifndef __HUMONGOUS_INT_FUNCTIONS

#include "HumongousInt.hpp"

#include <iomanip>
#include <sstream>
#include <cmath>
#include <cassert>
#include <algorithm>

// *** CONSTRUCTORS ***
HumongousInt::HumongousInt(const Block& d)
{
	data.push_back(d);
}
HumongousInt::HumongousInt(Block&& d)
{
	data.push_back(d);
}


// *** CONVERTERS ***
HumongousInt::operator bool() const
{
	for(auto e : data)
	{
		if(e) return true;
	}
	return false;
}

std::string to_string(const HumongousInt& hint)
{
	std::string result;
	
	std::stringstream ss;
	ss << hint;
	ss >> result;
	
	return result;
}

HumongousInt stohi(const std::string &str)
{
	HumongousInt result;
	
	std::stringstream ss(str);
	ss >> result;
	
	return result;
}

// *** COMPARISON ***
bool HumongousInt::ltData(const BlockList &lhs, const BlockList &rhs, const size_t blocksToConsider)
{
	std::cerr << "Inside ltData" << std::endl;
	if(lhs.size() > blocksToConsider)
	{
		auto it=lhs.begin();
		std::advance(it, blocksToConsider);
		if(std::any_of(std::move(it), lhs.end(), [](const Block &b) { return static_cast<bool>(b); }))
		{
			return false;
		}
	}
	else if(lhs.size() < blocksToConsider)
	{
		auto it=rhs.begin();
		std::advance(it, rhs.size() - blocksToConsider + lhs.size());
		if(std::any_of(std::move(it), rhs.end(), [](const Block &b) { return static_cast<bool>(b); }))
		{
			return true;
		}
	}
	std::cerr << "Still Inside ltData" << std::endl;
	// data.size() is equal or higher blocks are 0
	auto lIt=lhs.rbegin();
	auto rIt=lhs.rbegin();
	if(lhs.size() > blocksToConsider)
	{
		std::advance(lIt, lhs.size() - blocksToConsider);
	}
	else if(lhs.size() < blocksToConsider)
	{
		std::advance(rIt, blocksToConsider - lhs.size());
	}
	while(lIt!=lhs.rend())
	{
		std::cerr << "In while loop of ltData" << std::endl;
		assert(rIt!=rhs.rend());
		
		if(*lIt < *rIt)
		{
			return true;
		}
		if(*lIt > *rIt)
		{
			return false;
		}
		
		++lIt;
		++rIt;
	}
	return false; // this is equality
}
bool HumongousInt::eqData(const BlockList &lhs, const BlockList &rhs)
{
	if(lhs.size() > rhs.size())
	{
		auto it=lhs.begin();
		std::advance(it, rhs.size());
		if(std::any_of(std::move(it), lhs.end(), [](const Block &b) { return static_cast<bool>(b); }))
		{
			return false;
		}
	}
	else if(lhs.size() < rhs.size())
	{
		auto it=rhs.begin();
		std::advance(it, rhs.size());
		if(std::any_of(std::move(it), rhs.end(), [](const Block &b) { return static_cast<bool>(b); }))
		{
			return false;
		}
	}
	// data.size() is equal or higher blocks are 0
	auto lIt=lhs.rbegin();
	auto rIt=lhs.rbegin();
	if(lhs.size() > rhs.size())
	{
		std::advance(lIt, lhs.size() - rhs.size());
	}
	else if(lhs.size() < rhs.size())
	{
		std::advance(rIt, rhs.size() - lhs.size());
	}
	while(lIt!=lhs.rend())
	{
		assert(rIt!=rhs.rend());
		
		if(*lIt != *rIt)
		{
			return false;
		}
	}
	return true;
}
bool HumongousInt::operator<(const HumongousInt& that) const
{
	if(isNaN() || that.isNaN())
	{
		return false;
	}
	if(isNegative() && that.isPositive())
	{
		if(isZero() && that.isZero())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	if(isPositive() && that.isNegative())
	{
		return false;
	}
	// if both are negative, we reverse what data we consider
	if(isNegative() && that.isNegative())
	{
		return ltData(that.data, this->data);
	}
	else
	{
		return ltData(this->data, that.data);
	}
}

bool HumongousInt::operator!() const
{
	return isNaN() || isZero();
}

// *** BIT OPERATIONS ***
HumongousInt& HumongousInt::operator<<=(size_t n)
{
	while(n>=BITS_IN_BLOCK)
	{
		data.push_front(0);
		n -= BITS_IN_BLOCK;
	}
	if(n) // n!=0
	{
		Block overflow = 0;
		uint64_t val;

		for(auto it = data.begin(); it!=data.end(); ++it)
		{
			val = (((uint64_t)(*it)) << n) | overflow;
			overflow = (Block)(val >> BITS_IN_BLOCK);

			(*it) = (Block)(val & MAX_BLOCK);
		}
		if(overflow)
		{
			data.push_back(overflow);
		}
	}
	return *this;
}
HumongousInt& HumongousInt::operator>>=(size_t n)
{
	size_t overflowShift = BITS_IN_BLOCK - n;
	Block prevOverflow;
	Block nextOverflow = 0;
	for(auto it = data.rbegin(); it!=data.rend(); ++it)
	{
		prevOverflow = nextOverflow;
		nextOverflow = (*it) << overflowShift;
		(*it) >>= n;
		(*it) |= prevOverflow;
	}
	if(!data.back() && data.size()>1)
	{
		data.pop_back();
	}
	return *this;
}

HumongousInt& HumongousInt::operator&=(const HumongousInt & that)
{
	auto itThat = that.data.begin();
	auto itThis = this->data.begin();
	while(itThat!=that.data.end() && itThis!=this->data.end())
	{
		(*itThis) &= (*itThat);
		++itThat;
		++itThis;
	}
	data.erase(itThis, this->data.end());
	return *this;
}
HumongousInt& HumongousInt::operator|=(const HumongousInt & that)
{
	auto itThat = that.data.cbegin();
	auto itThis = this->data.begin();
	while(itThat!=that.data.cend() && itThis!=this->data.end())
	{
		(*itThis) |= (*itThat);
		++itThat;
		++itThis;
	}
	std::copy(itThat, that.data.cend(), itThis);
	return *this;
}

// *** MATHEMATICAL OPERATIONS ***
void HumongousInt::addDataWithShift(BlockList &sink, const BlockList &d, size_t n)
{
	auto itSink = sink.begin();
	auto itThat = d.begin();

	while(n>=BITS_IN_BLOCK)
	{
		++itSink;
		n -= BITS_IN_BLOCK;
	}

	uint64_t val;
	Block overflow{};
	
	while(itThat!=d.end() && itSink!=sink.end())
	{
		val = static_cast<uint64_t>(*itSink) + (static_cast<uint64_t>(*itThat) << n) + static_cast<uint64_t>(overflow);
		overflow = static_cast<Block>(val >> BITS_IN_BLOCK);
		
		(*itSink) = val;
		
		++itSink;
		++itThat;
	}
	while(overflow && itSink!=sink.end())
	{
		val = static_cast<uint64_t>(*itSink) + static_cast<uint64_t>(overflow);
		overflow = static_cast<Block>(val >> BITS_IN_BLOCK);

		++itSink;
	}
	if(overflow)
	{
		sink.push_back(overflow);
	}
}	

HumongousInt& HumongousInt::operator*=(const HumongousInt & that)
{
	if(isNaN())
	{
		return *this;
	}
	if(that.isNaN())
	{
		setNaN();
		return *this;
	}
	if(isZero())
	{
		if(that.isNegative()) this->negate();
		
		return *this;
	}
	if(isInfinity())
	{
		if(that.isNegative()) this->negate();
		
		return *this;
	}
	
	const BlockList cloneData{this->data};
	std::for_each(data.rbegin(), data.rend(), [](Block &b){b=0;}); // std::execution::parallel_unsequenced_policy

	const BlockList & smallData = cloneData.size() < that.data.size() ? cloneData : that.data;
	const BlockList & largeData = cloneData.size() < that.data.size() ? that.data : cloneData;

	size_t pos = 0;
	for(auto smallDatum : smallData)
	{
		for(size_t i=0; i<BITS_IN_BLOCK; ++i)
		{
			if(smallDatum & 1)
			{
				addDataWithShift(this->data, largeData, pos);
			}
			++pos;
			smallDatum >>= 1;
		}
	}
	return *this;
}


void HumongousInt::divData(BlockList &lhs, const BlockList &rhs, const size_t blocksToConsider)
{
	auto itL = lhs.rbegin();
	
	BlockList remainder;
	
	for(size_t i=0; i<blocksToConsider; ++i)
	{
		assert(itL!=lhs.rend());
		
		remainder.push_front(*itL);
		(*itL) = 0;
		++itL;
	}
	do
	{
		std::cerr << "Inside DO." << std::endl;
		while(!ltData(remainder, rhs, blocksToConsider))
		{
			std::cerr << "Inside WHILE" << std::endl;
			++(*itL);
			subData(remainder, rhs, blocksToConsider);
		}
		++itL;
		remainder.push_front(*itL);
		(*itL) = 0;
	} while(itL != lhs.rend());
}
HumongousInt& HumongousInt::operator/=(const HumongousInt & that)
{
	if(this->isNaN() || that.isNaN() || that.isZero())
	{
		setNaN();
		return *this;
	}
	if(this->isInfinity())
	{
		if(that.isInfinity())
		{
			setNaN();
		}
		else if((this->isNegative() && that.isPositive()) || (this->isPositive() || that.isNegative()))
		{
			negate();
		}
		return *this;
	}
	if(that.isInfinity())
	{
		if(that.isNegative() || this->isPositive())
		{
			this->setZero();
		}
		else
		{
			this->setZero();
			this->setNegative();
		}
		return *this;
	}
	
	// remove trailing zero blocks
	size_t blocksToIgnore = 0;
	std::find_if(that.data.begin(), that.data.end(), [&blocksToIgnore, this](const Block& b) {
		if(b!=0) return true;
		this->data.pop_front();
		++blocksToIgnore;
		return false;
	});
	
	size_t blocksToConsider = that.data.size() - blocksToIgnore;
	
	divData(this->data, that.data, blocksToConsider);
	
	return *this;
}
void HumongousInt::addData(BlockList &lhs, const BlockList &rhs)
{
	Block overflow{};
	uint64_t val;
	
	auto itThat = rhs.begin();
	auto itThis = lhs.begin();
	while(itThat!=rhs.end() && itThis!=lhs.end())
	{
		val = (uint64_t)(*itThis) + (uint64_t)(*itThat) + (uint64_t)overflow;
		if(val > MAX_BLOCK)
		{
			overflow = 1;
		}
		else
		{
			overflow = 0;
		}
		(*itThis) = (Block)(val & MAX_BLOCK);
		
		++itThis;
		++itThat;
	}
	while(overflow && itThis!=lhs.end())
	{
		val = (uint64_t)(*itThis) + (uint64_t)overflow;
		if(val > MAX_BLOCK)
		{
			overflow = 1;
		}
		else
		{
			overflow = 0;
		}
		++itThis;
	}
	if(overflow)
	{
		lhs.push_back(1);
	}
}
bool HumongousInt::subData(BlockList &lhs, const BlockList &rhs, size_t blocksToConsider)
{
	bool negate = ltData(lhs, rhs, blocksToConsider);	
	
	if(!negate)
	{
		auto lIt = lhs.begin();
		auto rIt = rhs.begin();
		std::advance(rIt, rhs.size() - blocksToConsider);
		
		bool borrow = false;
		while(rIt != rhs.end()) // we know that left is larger
		{
			if(borrow)
			{
				if(*lIt==0)
				{
					*lIt=MAX_BLOCK;
					borrow=true;
				}
				else
				{
					--(*lIt);
					borrow=false;
				}
			}
			borrow = borrow || (*lIt < *rIt);
			*lIt -= *rIt;
			
			++lIt;
			++rIt;
		}
		while(borrow)
		{
			assert(lIt!=lhs.end());
			
			if(*lIt==0)
			{
				*lIt=MAX_BLOCK;
				borrow=true;
			}
			else
			{
				--(*lIt);
				borrow=false;
			}
			
			++lIt;
		}
	}
	else // subtracting larger from smaller
	{
		auto lIt = lhs.begin();
		auto rIt = rhs.begin();
		std::advance(rIt, rhs.size() - blocksToConsider);
		
		bool borrow = false;
		while(lIt != lhs.end()) // we know that right is larger
		{
			Block from = *rIt;
			if(borrow)
			{
				if(from==0)
				{
					from=MAX_BLOCK;
					borrow=true;
				}
				else
				{
					--from;
					borrow=false;
				}
			}
			borrow = borrow || (from < *lIt);
			*lIt = from - *lIt;
			
			++lIt;
			++rIt;
		}
		while(borrow)
		{
			assert(rIt!=rhs.end());
			
			Block from = *rIt;

			if(from==0)
			{
				from=MAX_BLOCK;
				borrow=true;
			}
			else
			{
				--from;
				borrow=false;
			}
			
			lhs.push_back(from);

			++rIt;
		}
		while(rIt!=rhs.end())
		{
			lhs.push_back(*rIt);
			++rIt;
		}
	}
		
	return negate;
}

HumongousInt& HumongousInt::operator+=(const HumongousInt & that)
{
	if(this->isNaN())
	{
		return *this;
	}
	if(that.isNaN())
	{
		setNaN();
		return *this;
	}
	if(this->isInfinity())
	{
		if((this->isPositiveInfinity() && that.isNegativeInfinity()) || (this->isNegativeInfinity() && that.isPositiveInfinity()))
		{
			setNaN();
		}
		return *this;
	}

	if((this->isPositive() && that.isPositive()) || (this->isNegative() && that.isNegative()))
	{
		addData(this->data, that.data);
	}
	else
	{
		bool res = subData(this->data, that.data);
		if(res) negate();
	}
	return *this;
}

HumongousInt& HumongousInt::operator-=(const HumongousInt & that)
{
	if(this->isNaN())
	{
		return *this;
	}
	if(that.isNaN())
	{
		setNaN();
		return *this;
	}
	if(this->isInfinity())
	{
		if((this->isPositiveInfinity() && that.isPositiveInfinity()) || (this->isNegativeInfinity() && that.isNegativeInfinity()))
		{
			setNaN();
		}
		return *this;
	}
	
	if((this->isPositive() && that.isPositive()) || (this->isNegative() && that.isNegative()))
	{
		bool res = subData(this->data, that.data);
		if(res) negate();
	}
	else
	{
		addData(this->data, that.data);
	}
	return *this;
}
void HumongousInt::incData(BlockList &d)
{
	auto it = d.begin();

	while(it!=d.end())
	{
		if(*it < MAX_BLOCK)
		{
			++(*it);
			return;
		}
		else
		{
			++it;
		}
	}

	d.push_back(1);
}
bool HumongousInt::decData(BlockList &d)
{
	auto it = d.begin();
	
	while(it!=d.end())
	{
		if(*it > 0)
		{
			--(*it);
			return false;
		}
		else
		{
			++it;
		}
	}
	
	// if we are here, we are trying to borrow from 0
	d.clear();
	d.push_back(1);
	return true;
}
HumongousInt& HumongousInt::operator++()
{
	if(this->isNaN() || this->isInfinity())
	{
		return *this;
	}
	
	if(this->isPositive() || this->isZero())
	{
		this->setPositive();
		incData(this->data);
	}
	else
	{
		decData(this->data);
	}
	return *this;
}
HumongousInt& HumongousInt::operator--()
{
	if(this->isNaN() || this->isInfinity())
	{
		return *this;
	}
	
	if(this->isNegative() || this->isZero())
	{
		this->setNegative();
		incData(this->data);
	}
	else
	{
		decData(this->data);
	}
	return *this;
}

// *** INPUT / OUTPUT ***
std::ostream& operator<<(std::ostream& out, const HumongousInt& hint)
{
	if(hint.optionBits.test(0)) // negative
	{
		out << '-';
	}
	if(hint.optionBits.test(1)) // infinite
	{
		out << "INF";
	}
	else
	{
		auto it=hint.data.rbegin();
		while(it!=hint.data.rend() && !(*it)) // skipping 0 bytes on the left
		{
			++it;
		}
		if(it!=hint.data.rend())
		{
			auto oldFlags = out.flags();
			out << "0x";
			out << std::hex << std::noshowbase;
			out << (*it);
			++it;
			while(it!=hint.data.rend())
			{
				out << std::setw(HumongousInt::BITS_IN_BLOCK/4) << std::setfill('0') << (*it);
				
				++it;
			}
			out.flags( oldFlags );
		}
		else
		{
			out << '0';
		}
	}
	return out;
}

std::istream& operator>>(std::istream& in, HumongousInt& hint)
{
	bool done = false;
	
	char c;
		
	hint.data.erase(hint.data.begin(), hint.data.end());
	hint.data.push_front(0);
	
	in >> c;
	if(c=='-')
	{
		hint.setNegative();
		in >> c;
	}
	if(c=='0')
	{
		c=in.get();
		if(isspace(c))
		{
			hint.setZero();
		}
		if(c!='x')
		{
			in.putback(c);
		}
	}
	else
	{
		in.putback(c);
	}
	
	HumongousInt::Block val;
	size_t bitcount = 0;
	while( !done && (c=in.get()) )
	{
		switch(c)
		{
			case '0':
				val <<= 4;
				//val|=0x0;
				break;
			case '1':
				val <<= 4;
				val|=0x1;
				break;
			case '2':
				val <<= 4;
				val|=0x2;
				break;
			case '3':
				val <<= 4;
				val|=0x3;
				break;
			case '4':
				val <<= 4;
				val|=0x4;
				break;
			case '5':
				val <<= 4;
				val|=0x5;
				break;
			case '6':
				val <<= 4;
				val|=0x6;
				break;
			case '7':
				val <<= 4;
				val|=0x7;
				break;
			case '8':
				val <<= 4;
				val|=0x8;
				break;
			case '9':
				val <<= 4;
				val|=0x9;
				break;
			case 'a': case 'A':
				val <<= 4;
				val|=0xA;
				break;
			case 'b': case 'B':
				val <<= 4;
				val|=0xB;
				break;
			case 'c': case 'C':
				val <<= 4;
				val|=0xC;
				break;
			case 'd': case 'D':
				val <<= 4;
				val|=0xD;
				break;
			case 'e': case 'E':
				val <<= 4;
				val|=0xE;
				break;
			case 'f': case 'F':
				val <<= 4;
				val|=0xF;
				break;
			
			case ' ': case '\t':
			case '\r': case '\n':
			case '\0': case ~'\0':
				done=true;
				break;
			default:
				return in;
				break;
		}
		bitcount +=4;
		
		if(bitcount == HumongousInt::BITS_IN_BLOCK)
		{
			hint.data.push_front(val);
			bitcount = 0;
		}
	}
	
	if(bitcount)
	{
		hint <<= bitcount;
		hint |= val;
	}
	return in;
}

// *** DEBUG STUFF ***
#ifdef __HUMONGOUS_INT_DEBUG
#include <iostream>
#include <iomanip>
void HumongousInt::debugPrint() const
{
	std::cout << "size: " << data.size() << ' ';
	for(auto it=data.rbegin(); it!=data.rend(); ++it)
	{
		std::cout << std::setfill('0') << std::setw(16) << std::hex << (*it) << ' ';
	}
	std::cout << std::endl;
}
#endif

#else // template functions

#include <cmath>

// *** CONVERTERS ***
template<typename T>
HumongousInt::HumongousInt(const T& d)
{
	if constexpr(std::is_integral_v<T>)
	{
		using datatypeUnsigned = std::make_unsigned<std::remove_reference_t<std::remove_cv_t<T>>>;
		
		unsigned long long datum;
		
		if constexpr(std::is_signed_v<T>)
		{
			if(d<0)
			{
				optionBits.set(0);
				datum = -d;
			}
			else
			{
				datum = d;
			}
		}
		else
		{
			datum = d;
		}

		if constexpr(sizeof(Block) >= sizeof(datatypeUnsigned)) // Block is equal or larger
		{
			data.push_back(datum);
		}
		else
		{
			do
			{
				data.push_back(datum);
				datum >>= BITS_IN_BLOCK;
			} while(datum);
		}
	}
	else if constexpr(std::is_floating_point_v<T>)
	{
		if constexpr(std::numeric_limits<T>::has_quiet_NaN)
		{
			if(std::isnan(d))
			{
				optionBits.set(2);
			}
		}
		else if constexpr(std::numeric_limits<T>::has_infinity)
		{
			if(d==std::numeric_limits<T>::infinity)
			{
				optionBits.set(1);
			}
		}
		if(!optionBits.test(2)) // not NaN
		{
			optionBits.set(0, d<=-0.0);
		}
		if(!optionBits.test(1) && !optionBits.test(2)) // not NaN and not INF
		{
			T whole;
			modf(d, &whole);
			Block b=0;
			while(whole!=0)
			{
				b = static_cast<Block>(whole);
				data.push_back(b);
				whole /= (1ull<<BITS_IN_BLOCK);
				modf(whole, &whole);
			}
			b = static_cast<Block>(whole);
			data.push_back(b);
		}
	}
}

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
HumongousInt::operator T() const
{
	typedef std::make_unsigned<std::remove_reference_t<std::remove_cv_t<T>>> datatype;

	if constexpr(std::is_signed_v<T>)
	{
		datatype res{};
		if constexpr(sizeof(Block) >= sizeof(datatype)) // Block is equal or larger
		{
			res = data.front();
		}
		else // need to combine several blocks for a value
		{
			auto it=data.begin();
			res |= *it;
			++it;
			for(size_t shift=1; shift<sizeof(T); ++shift)
			{
				std::remove_const_t<T> val = *it;
				val <<= shift*8;
				res |= val;
				++it;
			}
		}
		res <<= 1;
		res >>= 1;
		return (T)res;
	}
	else
	{
		if constexpr(sizeof(Block) >= sizeof(datatype)) // Block is equal or larger
		{
			return (T)data.front();
		}
		else // need to combine several blocks for a value
		{
			std::remove_const<T> res{};
			
			auto it=data.begin();
			res |= *it;
			++it;
			for(size_t shift=1; shift<sizeof(T); ++shift)
			{
				std::remove_const<T> val = *it;
				val <<= shift*8;
				res |= val;
				++it;
			}
		}
	}
}

// *** BIT OPERATIONS ***

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
HumongousInt& HumongousInt::operator&=(T d)
{
	if constexpr(std::is_signed_v<T>)
	{
		testNonNegative(d);
	}

	typedef std::make_unsigned<std::remove_reference_t<std::remove_cv_t<T>>> datatype;
	auto it=data.begin();
	if constexpr(sizeof(Block) >= sizeof(datatype)) // Block is equal or larger
	{
		(*it) &= d;
		++it;
	}
	else
	{
		do
		{
			(*it) &= (Block)d;
			d >>= BITS_IN_BLOCK;
			++it;
		} while(d);
	}
	data.erase(it, data.end());
	return *this;
}
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
HumongousInt& HumongousInt::operator|=(T d)
{
	if constexpr(std::is_signed_v<T>)
	{
		testNonNegative(d);
	}

	typedef std::make_unsigned<std::remove_reference_t<std::remove_cv_t<T>>> datatype;
	auto it=data.begin();
	if constexpr(sizeof(Block) >= sizeof(datatype)) // Block is equal or larger
	{
		(*it) |= d;
		++it;
	}
	else
	{
		do
		{
			(*it) |= (Block)d;
			d >>= BITS_IN_BLOCK;
			++it;
		} while(d);
	}
	return *this;
}
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
HumongousInt& HumongousInt::operator^=(T d)
{
	if constexpr(std::is_signed_v<T>)
	{
		testNonNegative(d);
	}

	typedef std::make_unsigned<std::remove_reference_t<std::remove_cv_t<T>>> datatype;
	auto it=data.begin();
	if constexpr(sizeof(Block) >= sizeof(datatype)) // Block is equal or larger
	{
		(*it) ^= d;
		++it;
	}
	else
	{
		do
		{
			(*it) ^= (Block)d;
			d >>= BITS_IN_BLOCK;
			++it;
		} while(d);
	}
	return *this;
}

#endif // template guard
