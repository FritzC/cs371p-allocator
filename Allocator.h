// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument
#include <stdlib.h> 

// ---------
// Allocator
// ---------

template <typename T, std::size_t N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        typedef       value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef       value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}                                              // this is correct

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}

    private:
        // ----
        // data
        // ----

        char a[N];
        const int debug = 0; 
        const int sent_size = 2 * sizeof(int);

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * Ensures that the allocator only contains valid blocks 
         */
        bool valid () const {
            int i = 0;
            while (i < N-sent_size/2) {             
                int sent = (*this)[i];
                if(!sent) return false;
                if(debug) std::cout << i << "\t" << sent << std::endl;                  //prints value of array index and sentinel value
                i += abs(sent) + sent_size/2;                                           //finds index of matching sentinel from the value at first sentinel
                if(debug) std::cout << i << "\t" << (*this)[i] << std::endl;            //prints value of array index and sentinel value
                if (i > N-sent_size/2 || (*this)[i] != sent) return false;              //checks if index is out of bounds or values mismatch 
                i += sent_size/2;                                                          
            }
            return true;
        }

        /**
         * O(1) in space
         * O(1) in time
         * https://code.google.com/p/googletest/wiki/AdvancedGuide#Private_Class_Members
         */
        FRIEND_TEST(TestAllocator2, index);
        FRIEND_TEST(TestAllocator4, index);
        FRIEND_TEST(TestAllocator5, index);
        FRIEND_TEST(TestAllocator6, index);
        FRIEND_TEST(TestAllocator7, index);
        FRIEND_TEST(TestAllocator8, index);
        FRIEND_TEST(TestAllocator9, index);
        FRIEND_TEST(TestAllocator10, index);
        FRIEND_TEST(TestAllocator11, index);
        FRIEND_TEST(TestAllocator12, index);
        FRIEND_TEST(TestAllocator13, index);
        FRIEND_TEST(TestAllocator14, index);
        FRIEND_TEST(TestAllocator15, index);

        int& operator [] (int i) {
            return *reinterpret_cast<int*>(&a[i]);}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
         * Sets values at the first and last value of the array as sentinels with values  
         * representing the amount of free space between them 
         */
        Allocator () {
            if (N < sizeof(T) + sent_size) {
                throw std::bad_alloc();
            }
            (*this)[0] = N - sent_size;
            (*this)[N - sent_size/2] = N - sent_size;
            assert(valid());
        }

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         * throw a bad_alloc exception, if n is invalid
         */
        pointer allocate (size_type n) {
            pointer ptr;
            if (n <= 0) throw std::bad_alloc();
            int sv = n * sizeof(T); 
            int i = 0;
            bool found = false;
            while (i < N-sent_size/2 && !found) {
                int sent = (*this)[i];
                // checks if there is enough space for a valid block after allocation
                if ((sent > sv) && ((sent - sv) < (sizeof(T) + sent_size))) sv = sent;
                // if the block available is exactly the size needed, simply flip sentinels
                if (sent == sv){
                    (*this)[i] *= -1;
                    (*this)[i + sent + sent_size/2] *= -1;
                    ptr = reinterpret_cast<T*>(&(*this)[i + sent_size/2]);
                    found = true;
                }
                // if there is enough room after allocation for a valid block, create a new block
                else if (sent > sv) { 
                    (*this)[i] = -sv;
                    (*this)[i + sv + sent_size/2] = -sv;
                    (*this)[i + sv + sent_size] = sent - sv - sent_size;
                    (*this)[i + sent + sent_size/2] = sent - sv - sent_size;
                    ptr = reinterpret_cast<T*>(&(*this)[i + sent_size/2]);
                    found = true;
                }
                //finds index of next block 
                i += abs(sent) + sent_size;
            }
            if(!found) throw std::bad_alloc();
            assert(valid());
            return ptr;
        }

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                               // this is correct and exempt
            assert(valid());}                           // from the prohibition of new

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * after deallocation adjacent free blocks must be coalesced
         * throw an invalid_argument exception, if p is invalid
         * frees previously allocated memory  
         */
        void deallocate (pointer p, size_type) {
            int* curr = reinterpret_cast<int*>(p);
            if(curr <= &(*this)[0] || curr >= &(*this)[N-sent_size/2] ){
                throw std::invalid_argument("pointer p invalid");
            }
            --curr;
            *curr *= -1;
            int* next = curr + 2 + *curr/sizeof(int);  
            int* last = curr - 1;
            int nv = *next/sizeof(int);
            int lv = *last/sizeof(int);
            *(next-1) = *curr;

            //checks if block to the left must be coalesced
            if(curr != &(*this)[0]){
                if(*last > 0){
                    *(last - 1 - lv) = (*last + sent_size + *curr);
                    *(next-1) = (*last + sent_size + *curr);
                    curr = last - 1 - lv;
                }
            }
            //checks if block to the right must be coalesced
            if((next-1) != &(*this)[N-sent_size/2]){
                if(*next > 0){
                    *curr = (sent_size + *curr + *next);
                    *(next + nv + 1) = *curr;
                }

            }
            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        const int& operator [] (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};

#endif // Allocator_h
