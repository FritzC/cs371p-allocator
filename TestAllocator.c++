// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

#include "gtest/gtest.h"

#include "Allocator.h"

// --------------
// TestAllocator1
// --------------

template <typename A>
struct TestAllocator1 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator1, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type  x;
    const size_type       s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

// --------------
// TestAllocator2
// --------------

TEST(TestAllocator2, const_index) {
    const Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);}

TEST(TestAllocator2, index) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

// --------------
// TestAllocator3
// --------------

template <typename A>
struct TestAllocator3 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator3, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 10;
    const value_type     v = 2;
    const pointer        b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

//multiple allocations
TEST(TestAllocator4, index) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
    x.allocate((size_t)1);
    x.allocate((size_t)1);
    x.allocate((size_t)2);
    x.allocate((size_t)3);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);
    ASSERT_EQ(x[24], -8);
    ASSERT_EQ(x[36], -8);
    ASSERT_EQ(x[40], -12);
    ASSERT_EQ(x[56], -12);
}

// multiple allocs with double
TEST(TestAllocator5, index) {
    Allocator<double, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
    x.allocate((size_t)1);
    x.allocate((size_t)1);
    x.allocate((size_t)2);
    x.allocate((size_t)3);
    ASSERT_EQ(x[0], -8);
}

// allocating entire heap
TEST(TestAllocator6, index) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
    x.allocate((size_t)23);
    ASSERT_EQ(x[0], -92);
    ASSERT_EQ(x[96], -92);
}

//allocate something too big
TEST(TestAllocator7, index) {
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
    try{
        x.allocate((size_t)24);
        assert(false);
    }
    catch(std::bad_alloc& e){
        ASSERT_EQ(x[0], 92);
        ASSERT_EQ(x[96], 92);
    }
}

// deallocate: coalescing to the left
TEST(TestAllocator8, index) {
    Allocator<int, 100> x;
    int* b = x.allocate((size_t)1);
    int* a = x.allocate((size_t)2);
    x.allocate((size_t)4);
    x.deallocate(a, sizeof(int));
    x.deallocate(b, sizeof(int));
    ASSERT_EQ(x[0], 20);
    ASSERT_EQ(x[24], 20);
}

//deallocate: coalescing from both sides 
TEST(TestAllocator9, index) {
    Allocator<int, 100> x;
    int* b = x.allocate((size_t)1);
    int* a = x.allocate((size_t)2);
    int* c = x.allocate((size_t)4);
    x.deallocate(b, sizeof(int));
    x.deallocate(c, sizeof(int));
    x.deallocate(a, sizeof(int));
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

// creating heap smaller than data type
TEST(TestAllocator10, index) {
    try{
        Allocator<double, 10> x;
        assert(false);
    }
    catch(std::bad_alloc& e){
    }
}

// valid: sentinel larger than heap
TEST(TestAllocator11, index) {
    Allocator<int, 100> x;
    x[0] = 100;
    ASSERT_EQ(x.valid(), false);   
}

//valid: mismatched sentinels
TEST(TestAllocator12, index) {
    Allocator<int, 100> x;
    x[0] = 4;
    x[8] = 10;
    ASSERT_EQ(x.valid(), false);   
}

//deallocate: invalid_argument
TEST(TestAllocator13, index) {
    Allocator<int, 100> x;
    int* b = x.allocate((size_t)1);
    try{
        x.deallocate(b-1, sizeof(int));
        assert(false);
    }
    catch(std::invalid_argument& e){
    }
}

//zero sentinel
TEST(TestAllocator14, index) {
    Allocator<int, 100> x;
    x[0] = 0;
    ASSERT_EQ(x.valid(), false);   
}

//tests if there is enough room for valid block after allocation
TEST(TestAllocator15, index) {
    Allocator<double, 20> x;
    x.allocate((size_t)1);
    ASSERT_EQ(x[0], -12);
    ASSERT_EQ(x[16], -12);
}

