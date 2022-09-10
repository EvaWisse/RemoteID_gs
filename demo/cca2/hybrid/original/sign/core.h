/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "arch.h"

namespace core {

//#define CORE_ARDUINO

/**
 * @brief SHA256 hash function instance */
typedef struct
{
    unsign32 length[2]; /**< 64-bit input length */
    unsign32 h[8];      /**< Internal state */
    unsign32 w[64];	/**< Internal state */
    int hlen;		/**< Hash length in bytes */
} hash256;

#define SHA256 32 /**< SHA-256 hashing */
#define SHA3_HASH256 32 /**< SHA3 256 bit hash */
#define uchar unsigned char  /**<  Unsigned char */

/* Marsaglia & Zaman Random number generator constants */

#define NK   21 /**< PRNG constant */
#define NJ   6  /**< PRNG constant */
#define NV   8  /**< PRNG constant */


/**
	@brief Cryptographically secure pseudo-random number generator instance
*/

typedef struct
{
    unsign32 ira[NK]; /**< random number array   */
    int      rndptr;  /**< pointer into array */
    unsign32 borrow;  /**<  borrow as a result of subtraction */
    int pool_ptr;     /**< pointer into random pool */
    char pool[32];    /**< random pool */
} csprng;

typedef struct
{
    int len;   /**< length in bytes  */
    int max;   /**< max length allowed - enforce truncation  */
    char *val; /**< byte array  */
} octet;

/* Hash function */
/**	@brief Initialise an instance of SHA256
 *
	@param H an instance SHA256
 */
extern void HASH256_init(hash256 *H);

/**	@brief Add a byte to the hash
 *
	@param H an instance SHA256
	@param b byte to be included in hash
 */
extern void HASH256_process(hash256 *H, int b);
/**	@brief Generate 32-byte final hash
 *
	@param H an instance SHA256
	@param h is the output 32-byte hash
 */
extern void HASH256_hash(hash256 *H, char *h);

/* random numbers */
/**	@brief Seed a random number generator from an array of bytes
 *
	The provided seed should be truly random
	@param R an instance of a Cryptographically Secure Random Number Generator
	@param n the number of seed bytes provided
	@param b an array of seed bytes

 */
extern void RAND_seed(csprng *R, int n, char *b);
/**	@brief Delete all internal state of a random number generator
 *
	@param R an instance of a Cryptographically Secure Random Number Generator
 */
extern void RAND_clean(csprng *R);
/**	@brief Return a random byte from a random number generator
 *
	@param R an instance of a Cryptographically Secure Random Number Generator
	@return a random byte
 */
extern int RAND_byte(csprng *R);

}

#endif
