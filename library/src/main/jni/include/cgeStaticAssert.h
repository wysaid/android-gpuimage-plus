/*
@Author: wysaid
@Blog: blog.wysaid.org
@Date: 2013-10-31
*/

#ifndef _CGE_STATICASSERT_H_
#define _CGE_STATICASSERT_H_

#ifndef _CGE_STATIC_ASSERT_

#define cgeStaticAssert(value) static_assert(value, "Invalid Parameters!")

#else

#if defined(DEBUG) || defined(_DEBUG)

template<bool K>
struct _CGEStaticAssert ;

template<>
struct _CGEStaticAssert<true> { int dummy; };

template<int n>
struct __CGEStaticAssert {};

#define cgeStaticAssert(value) do \
{\
	typedef __CGEStaticAssert<\
	sizeof(_CGEStaticAssert<(bool)(value)>)\
	> ___CGEStaticAssert;\
} while (0)

#else

#define cgeStaticAssert(...) 

#endif

#endif

#endif //_CGE_STATICASSERT_H_
