/*
@Author: wysaid
@Blog: blog.wysaid.org
@Date: 2013-10-31
*/

#ifndef _CGE_STATICASSERT_H_
#define _CGE_STATICASSERT_H_

#include <cassert>

#ifndef _CGE_STATIC_ASSERT_

#define cgeStaticAssert(value) static_assert(value, "Invalid Parameters!")

#else

#if defined(DEBUG) || defined(_DEBUG)

template<bool K>
struct CGEStaticAssert_ ;

template<>
struct CGEStaticAssert_<true> { int dummy; };

template<int n>
struct CGEStaticAssert {};

#define cgeStaticAssert(value) do \
{\
	typedef CGEStaticAssert<\
	sizeof(CGEStaticAssert_<(bool)(value)>)\
	> CGEStaticAssert__;\
} while (0)

#else

#define cgeStaticAssert(...) 

#endif

#endif

#endif //_CGE_STATICASSERT_H_
