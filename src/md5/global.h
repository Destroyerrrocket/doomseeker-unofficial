// derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm
//-----------------------------------------------------------------------------

/* GLOBAL.H - RSAREF types and constants
 */

/* PROTOTYPES should be set to one if and only if the compiler supports
  function argument prototyping.
The following makes PROTOTYPES default to 0 if it has not already
  been defined with C compiler flags.
 */
#ifndef __MD5_GLOBAL_H__
#define __MD5_GLOBAL_H__

#ifndef PROTOTYPES
#define PROTOTYPES 1
#endif

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

#if defined(_MSC_VER)
/* UINT2 defines a two byte word */
typedef unsigned __int16 UINT2;

/* UINT4 defines a four byte word */
typedef unsigned __int32 UINT4;
#elif defined(__GNUC__)
#include <stdint.h>
/* UINT2 defines a two byte word */
typedef uint16_t UINT2;

/* UINT4 defines a four byte word */
typedef uint32_t UINT4;
#else
/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned int UINT4;
#endif

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
  returns an empty list.
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif

#endif
