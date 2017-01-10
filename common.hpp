#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <cassert>
#include <stdexcept>
#include <cstdlib>
#include "net/global_zcontext.hpp"
#include <iostream>

#if __x86_64__ || \
	_M_X64 || \
	_WIN64 || \
	__powerpc64__ || \
	__ppc64__ || \
	__PPC64__ || \
	__ARCH_PPC64 || \
	__aarch64__ || \
	__sparc64__
#	define AF_64BIT
#else
#	define AF_32BIT
#endif

/**
 * \brief Branch prediction hint for compiler.
 * 
 * These are non-trivially defined for gcc and clang.
 */
#if defined __GNUC__ || defined __clang__
#	define LIKELY(x)	__builtin_expect(x, 1)
#	define UNLIKELY(x)	__builtin_expect(x, 0)
#else
#	define LIKELY(x)	(x)
#	define UNLIKELY(x)	(x)
#endif

/**
 * \brief Used to supress compiler warnings about unused variables.
 */
#define UNUSED(x) (void)(x)

/**
 * \brief Loop unroll hint for the compiler
 * 
 * This has to be put before a function declaration, whichs means all loops within the
 * function will be considered.
 */
#ifdef __GNUC__
#	define UNROLL_LOOP __attribute__((optimize("unroll-loops")))
#elif defined __clang__
#	define UNROLL_LOOP #pragma unroll
#else
#	define UNROLL_LOOP
#endif

#if defined(__SSE4_2__)
/**
 * \brief Enable SSE4.2 optimization for rapidjson.
 */
#	define RAPIDJSON_SSE42
#elif defined(__SSE2__)
/**
 * \brief Enable SSE2 optimization for rapidjson.
 */
#	define RAPIDJSON_SSE2
#endif

/**
 * \brief Whether to enable more descriptive exception checking.
 *
 */
#define THROW

#ifdef AF_64BIT
#	define AF_UINTN uint64_t
#	define AF_HUINTN uint32_t
#	define AF_QUINTN uint16_t
#else
#	define AF_UINTN uint32_t
#	define AF_HUINTN uint16_t
#	define AF_QUINTN uint8_t
#endif

/**
 * \brief Error message strings.
 */
namespace err_msg {
	const char _nllpntr[] = "null ptr";
	const char _arybnds[] = "array bounds exceeded";
	const char _stcimpl[] = "static unimplemented in child";
	const char _tpntfnd[] = "type not found by name";
	const char _rgstrfl[] = "failed to register child";
	const char _undhcse[] = "unhandled case";
	const char _zrlngth[] = "zero length";
	const char _ntwrkdn[] = "network down";
	const char _unrchcd[] = "unreachable code reached";
	
	
	const char _malinpt[] = "malformed input";
}

#include <boost/predef/detail/endian_compat.h>

#ifdef BOOST_LITTLE_ENDIAN
#	include <boost/endian/conversion.hpp>
#	define HTN_BYTE_ORD(x) boost::endian::endian_reverse(x)
#	define NTH_BYTE_ORD(x) boost::endian::endian_reverse(x)
#else
#	define HTN_BYTE_ORD(x) x
#	define NTH_BYTE_ORD(x) x
#endif

#define ARRAY_COUNT(x) sizeof(x)/sizeof(x[0])


#endif
