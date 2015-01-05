// Copyright (C) 1999 Andy Adler <adler@sce.carleton.ca>
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, see <http://www.gnu.org/licenses/>.

// Note that part of the code below is on the public domain. That part will
// marked as such.

#include <octave/oct.h>

/*
 * defines for SHA1.c code
 */

#define HW 5
#define LITTLE_ENDIAN_DEF 5

typedef struct {
    unsigned long state[5];
    unsigned long count[2];
    unsigned char buffer[64];
} hash_context;

static void
hash_initial( hash_context * c );
static void
hash_process( hash_context * c, unsigned char * data, unsigned len );
static void
hash_final( hash_context * c, unsigned long[HW] );

DEFUN_DLD (SHA1, args, ,
  "hash = SHA1 (...)\n\
SHA1 implements the Secure Hash Algorithm Cryptographic\n\
Hashing (One-Way) function.  (FIPS PUB 180-1)\n\
\n\
hash= SHA1( byte_stream, hash_initial )\n\
hash = Row Vector of  20 byte values;\n\
\n\
hash_initial default is 67452301 EFCDAB89 98BADCFE 10325476 C3D2E1F0\n\
\n\
Note: while it is possible to create a \"poor-man's\" MAC (message\n\
authenticity code) by setting hash_initial to a private value,\n\
it is better to use an algorithm like HMAC.\n\
\n\
HMAC= SHA1( [ passcode, SHA1( [passcode, data ] ) ); ")
{
   octave_value_list retval;
   octave_value tmp;
   int nargin = args.length ();
   hash_context c;

   if (nargin >2 || nargin ==0) {
      usage("SHA1");
      return retval;
   }
   else if (nargin ==2 ){
      ColumnVector init( args(1).vector_value() );
      if (init.length() != 20) 
         error("hash initializer must have 20 bytes");

      for( int i=0,k=0; i<5; i++) {
         c.state[i]= 0;
         for( int j=0; j<4; j++) 
            c.state[i]|= ( (unsigned char) init(k++) ) << (24 - 8*j);
//       printf("state=%d v=%08lX\n", i, c.state[i]);
      }
      c.count[0]= c.count[1]=0;
   }
   else {
      hash_initial( &c);
   }
 
   ColumnVector data( args(0).vector_value() );
   int len=data.length();

   for( int i=0; i< len; i++) {
      unsigned char d= (unsigned char) data(i);
      hash_process( &c, &d, 1);
   }

   unsigned long digest[5];
   hash_final( &c, digest);

   RowVector hash(20);
   for( int i=0; i<5; i++) {
      hash(4*i+0)= (digest[i] & 0xFF000000)>>24;
      hash(4*i+1)= (digest[i] & 0x00FF0000)>>16;
      hash(4*i+2)= (digest[i] & 0x0000FF00)>> 8;
      hash(4*i+3)= (digest[i] & 0x000000FF);
   }

   retval(0)= hash;
   return retval;
}   

/*
 * NOTE: The following code is not mine and has
 * the following copyright
 */

/*
This code is available from:
   http://ds.dial.pipex.com/george.barwood/v8/pegwit.htm
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain

Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/


#if !defined(LITTLE_ENDIAN_DEF) && !defined(BIG_ENDIAN_DEF)
  #if defined(_M_IX86) || defined(_M_I86) || defined(__alpha)
    #define LITTLE_ENDIAN_DEF
  #else
    #error "LITTLE_ENDIAN_DEF or BIG_ENDIAN_DEF must be defined"
  #endif
#endif

/* #define SHA1HANDSOFF * Copies data before messing with it. */


#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#ifdef LITTLE_ENDIAN_DEF
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#else
#define blk0(i) block->l[i]
#endif
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


/* Hash a single 512-bit block. This is the core of the algorithm. */

static
void SHA1Transform(unsigned long state[5], unsigned char buffer[64])
{
unsigned long a, b, c, d, e;
typedef union {
    unsigned char c[64];
    unsigned long l[16];
} CHAR64LONG16;
CHAR64LONG16* block;
#ifdef SHA1HANDSOFF
static unsigned char workspace[64];
    block = (CHAR64LONG16*)workspace;
    memcpy(block, buffer, 64);
#else
    block = (CHAR64LONG16*)buffer;
#endif
    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    /* Wipe variables */
    a = b = c = d = e = 0;
}


/* Initialize new context */

static
void hash_initial(hash_context* context)
{
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}


/* Run your data through this. */
static
void hash_process( hash_context * context, unsigned char * data, unsigned len )
{
unsigned int i, j;
unsigned long blen = ((unsigned long)len)<<3;

    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += blen) < blen ) context->count[1]++;
    context->count[1] += (len >> 29);
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64-j));
        SHA1Transform(context->state, context->buffer);
        for ( ; i + 63 < len; i += 64) {
            SHA1Transform(context->state, &data[i]);
        }
        j = 0;
    }
    else i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
}


/* Add padding and return the message digest. */

static
void hash_final( hash_context* context, unsigned long digest[5] )
{
unsigned long i, j;
unsigned char finalcount[8];

    for (i = 0; i < 8; i++) {
        finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
    }
    hash_process(context, (unsigned char *)"\200", 1);
    while ((context->count[0] & 504) != 448) {
        hash_process(context, (unsigned char *)"\0", 1);
    }
    hash_process(context, finalcount, 8);  /* Should cause a SHA1Transform() */
    for (i = 0; i < 5; i++) {
        digest[i] = context->state[i];
    }
    /* Wipe variables */
    i = j = 0;
    memset(context->buffer, 0, 64);
    memset(context->state, 0, 20);
    memset(context->count, 0, 8);
    memset(&finalcount, 0, 8);
#ifdef SHA1HANDSOFF  /* make SHA1Transform overwrite it's own static vars */
    SHA1Transform(context->state, context->buffer);
#endif
}
