// Project:  BitArray
// Name: Tyler Randolph
// Submitted: 12/2/2019
// I declare that the following source code was written by me, or provided
// by the instructor for this project. I understand that copying source
// code from any other source, providing source code to another student,
// or leaving my code on a public web site constitutes cheating.
// I acknowledge that  If I am found in violation of this policy this may result
// in a zero grade, a permanent record on file and possibly immediate failure of the class.

/* Reflection (1-2 paragraphs):  I really liked this assignment, but had a hard time getting test 8 to output correctly. It would convert the first 22 bits to 0 even though when I would test the input, it was inputting the correct numbers. I was unable to figure it out in the end. Other than that, the assignment went really well and was a tough one. */

#ifndef bitarray_hpp
#define bitarray_hpp

#include <vector>
#include <climits>
#include <stdexcept>
#include <string>
#include <math.h>
#include <iostream>
#include <utility>
#include <string.h>

using namespace std;

template<class IType = size_t>
class BitArray {
    
private:
    
    enum { BITS_PER_WORD = CHAR_BIT * sizeof(IType) };
    size_t _count;
    friend class bitproxy;
    vector<IType> Bit_Vector;
    
    void reset(size_t pos)
    {
        Bit_Vector[pos / BITS_PER_WORD] &= ~(1u << (pos % BITS_PER_WORD));
    }
    void set(size_t pos)
    {
        Bit_Vector[pos / BITS_PER_WORD] |= (1u << (pos % BITS_PER_WORD));
    }
    void copy(size_t pos, bool value)
    {
        if (value)
            set(pos);
        else
            reset(pos);
    }
    bool read_bit(size_t pos)const
    {
        size_t block = pos / BITS_PER_WORD;
        size_t offset = pos % BITS_PER_WORD;

        if (pos > _count)
            throw logic_error("Index given is out of range.");
        else
            return Bit_Vector[block] & (1u << offset);
    }
    void assign_bit(size_t pos, bool value)
    {
        size_t block = pos / BITS_PER_WORD;
        size_t offset = pos % BITS_PER_WORD;
        if (value)
            Bit_Vector[block] |= (1u << offset);
        else
            Bit_Vector[block] &= ~(1u << offset);
    }
    
    
    
public:
    // Object Management
    explicit BitArray(size_t bitCount = 0) : _count(bitCount) {
        if (bitCount)
            Bit_Vector.resize((bitCount - 1) / BITS_PER_WORD + 1);
    };
    
    explicit BitArray(const string& bit_string) : _count(bit_string.length()){
        if (_count)
            Bit_Vector.resize((_count - 1) / BITS_PER_WORD + 1);
        
        for (size_t i = 0; i < _count; ++i)
        {
            if (bit_string[i] == '1')
            {
                set(i);   
            }
            else if (bit_string[i] == '0')
            {
                reset(i);
            }
            else
                throw runtime_error("0 or 1 not found");
        }
    };
    
    BitArray(const BitArray& b)= default; // Copy constructor
    BitArray& operator=(const BitArray& b)= default; // Copy assignment
    
    explicit BitArray(BitArray&& b) noexcept : Bit_Vector(b.Bit_Vector)    // Move constructor
    {
        cout << "move constructor" << endl;
        _count = b._count;
        //Bit_Vector = b.Bit_Vector;
        b._count = 0;
        b.Bit_Vector.clear();
    };
    
    BitArray& operator=(BitArray&& b)noexcept { // Move assignment
        cout << "move assignment" << endl;
        swap(_count, b._count);
        swap(Bit_Vector, b.Bit_Vector);
        return *this;
    };
    
    size_t capacity()const { // # of bits the current allocation can hold
        return (BITS_PER_WORD * Bit_Vector.size());
    };
    
    // Mutators
    BitArray& operator+=(bool bit) { //Append a bit
        
        //2 options space/no space
        if (capacity() == _count)
            Bit_Vector.resize(Bit_Vector.size() + 1);
        _count++;
        //put bit in last spot
        copy(_count - 1, bit);
        return *this;
    };
    
    BitArray& operator+=(const BitArray& b) { // Append a BitArray
        
        size_t counter = b._count;
        
        // Add BitArray obj starting from the last spot of the current BitArray
        if (capacity() == _count)
            Bit_Vector.resize(Bit_Vector.size() + Bit_Vector.size());
        
        //Append BitArray obj to the end
        for (size_t i = 0; i < counter; i++)
        {
            copy(_count, b[i]);
            _count++;
        }
        
        return *this;
    };
    
    void erase(size_t pos, size_t nbits = 1) { // Remove “nbits” bits at a position
        
        if (nbits > _count)
            nbits = _count;
        
        for (size_t i = 0; i < nbits; i++) {
            for (size_t j = pos; j < _count; j++) {

                if (read_bit(j+1))
                    set(j);
                else
                    reset(j);
                
            }
            
            reset(_count - 1);
            _count--;
            
            if (_count == 1)
                break;
            
            shrink_to_fit();
        }
        
    };
    
    void insert(size_t pos, bool value) { // Insert a bit at a position (slide "right")
        if (capacity() == _count)
            Bit_Vector.resize(Bit_Vector.size() + 1);
        _count++;
        
        for (size_t i = _count; i > pos; i--) {

            if (read_bit(i-1))
                set(i);
            else
                reset(i);
            
        }
        
        assign_bit(pos, value);
    };
    
    void insert(size_t pos, const BitArray& bitArray) { // Insert an entire BitArray object
        if (capacity() == _count)
            Bit_Vector.resize(Bit_Vector.size() + Bit_Vector.size());
        _count += bitArray._count;
        
        for(size_t i = 0; i < bitArray._count; i++) {
            for (size_t j = _count; j > pos; j--) {
                if (read_bit(j-1))
                    set(j);
                else
                    reset(j);
            }
        }
        
        size_t j = 0;
        for (size_t i = pos; i < pos + bitArray._count; i++) {
            if (bitArray.read_bit(j))
                set(i);
            else
                reset(i);
            j++;
        }
        
        
        
    };
    
    void shrink_to_fit() { // Discard unused,trailing vector cells
        
        if (!Bit_Vector[Bit_Vector.size() - 1]) { //ask if it's a 0
            Bit_Vector.resize((_count - 1) / BITS_PER_WORD + 1);
        }
    };
    
    // Bitwise ops
    class BitProxy {
        size_t pos;
        BitArray& b;
    public:
        
        BitProxy(BitArray& bs, size_t p) : b{bs} {
            pos = p;
        };
        
        BitProxy& operator=(const BitProxy& bp) {
            
            if (bp.pos)
                b.Bit_Vector[pos / BITS_PER_WORD] |= (1u << (pos % BITS_PER_WORD));
            else
                b.Bit_Vector[pos / BITS_PER_WORD] &= ~(1u << (pos % BITS_PER_WORD));
            return *this;
        };
        
        BitProxy& operator=(bool bit) {
            // Set the bit in position pos to true or false, per "bit"
            if (bit)
                b.Bit_Vector[pos/BITS_PER_WORD] |= (1u << (pos%BITS_PER_WORD));
            else
                b.Bit_Vector[pos / BITS_PER_WORD] &= ~(1u << (pos%BITS_PER_WORD));
            return *this;
        };
        operator bool () const {
            // Return true or false per the bit in position pos
            return b.Bit_Vector[pos / BITS_PER_WORD] & (1u << (pos%BITS_PER_WORD));
        };
    };
    
    BitProxy operator[](size_t pos) {
        
        if (pos >= _count) {
            throw logic_error("Invalid Index.");
        }
        
        return BitProxy(*this, pos);
    };
    
    bool operator[](size_t pos) const {
        if (pos >= _count)
            throw logic_error("Invalid Index.");
        else
            return read_bit(pos);
    };
        
    void toggle(size_t pos) { // Toggles all bits
        
        if (pos >= _count) {
            throw logic_error("Invalid Index.");
        }
        
        if (read_bit(pos))
            reset(pos);
        else
            set(pos);
    };
    
    void toggle() {
        for (size_t i = 0; i < _count; ++i) {
            if (read_bit(i))
                reset(i);
            else
                set(i);
        }
    };
    
    BitArray operator~() const {
        BitArray bitArray(*this);
        bitArray.toggle();
        return bitArray;
    };
    
    // Shift operators...
    BitArray operator<<(unsigned int sLeft) const {
        BitArray<> bitArray(*this);
        return bitArray <<= sLeft;
    };
        
    BitArray operator>>(unsigned int sRight) const {
        BitArray<> bitArray(*this);
        return bitArray >>= sRight;
    };
        
    BitArray& operator<<=(unsigned int n) {
        BitArray<> temp1 = slice(n, _count - n);
        BitArray<> temp2(n);
        temp1 += temp2;
        *this = temp1;
        return *this;
    };
        
    BitArray& operator>>=(unsigned int n) {
        BitArray<> temp1(n);
        BitArray<> temp2 = slice(0, _count - n);
        temp1 += temp2;
        *this = temp1;
        return *this;
    };
    
    // Extraction ops
    BitArray slice(size_t pos, size_t count) const { // Extracts a new sub-array
        string bitstr = (*this).to_string();
        BitArray b{ bitstr.substr(pos, count) };
        return b;
    };

    // Comparison ops
    bool operator==(const BitArray& checkBits) const {
        return !((*this) < checkBits || checkBits > (*this));
    };
    
    bool operator!=(const BitArray& checkBits) const {
        return ((*this) < checkBits || checkBits > (*this));
    };
    
    bool operator<(const BitArray& checkBits) const {
        if (strcmp((*this).to_string().c_str(), checkBits.to_string().c_str()) < 0)
            return true;
        else
            return false;
    };
    
    bool operator<=(const BitArray& checkBits) const {
        if (strcmp((*this).to_string().c_str(), checkBits.to_string().c_str()) <= 0)
            return true;
        else
            return false;
    };
    
    bool operator>(const BitArray& checkBits) const {
        if (strcmp((*this).to_string().c_str(), checkBits.to_string().c_str()) > 0)
            return true;
        else
            return false;
    };
    
    bool operator>=(const BitArray& checkBits) const {
        if (strcmp((*this).to_string().c_str(), checkBits.to_string().c_str()) >= 0)
            return true;
        else
            return false;
    };
    
    // Counting ops
    size_t size() const { // Number of bits in use in the vector
        return _count;
    };
    size_t count()const { // The number of 1-bits present
        size_t one_bits = 0;
        for (size_t i = 0; i < _count; ++i)
        {
            if (read_bit(i))
                one_bits++;
        }
        return one_bits;
    };
    bool any() const { // Optimized version of count() > 0
        //for loop through vector
        for (size_t i = 0; i < Bit_Vector.size(); i++)
        {
            if (Bit_Vector[i])
                return true;
        }
        return false;
    };
    
    // Stream I/O (define these in situ)
    friend ostream& operator<<(ostream& os, const BitArray& _bitarray) {
        
        os << _bitarray.to_string();
        return os;
    };
    
    friend istream& operator>>(istream& is, BitArray& _bitarray) {
        string bitStr = "";
        char charChk;
        char charNext = 0;
        while (is >>charChk)
        {
            if (!isdigit(charChk))
            {
                charNext = -1;
                charNext = is.peek();
                is.unget();
                is.setstate(std::ios::failbit);
                break;
            }
            else
            {
                bitStr.push_back(charChk);
            }
        }
        if (charNext == -1)
            return is;
        is.clear();
        BitArray<> ba(bitStr);
        _bitarray = ba;
        return is;
    };
    
    // String conversion
    std::string to_string() const {
        string charStr = "";
        for (size_t i = 0; i < _count; i++)
        {
            if(read_bit(i))
                charStr += '1'; //adding a 1
            else
                charStr += '0';    //adding a 0
        }
        return charStr;
    };
    
};

#endif

