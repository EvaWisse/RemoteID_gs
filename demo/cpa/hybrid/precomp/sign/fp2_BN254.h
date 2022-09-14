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

#ifndef FP2_BN254_H
#define FP2_BN254_H

#include "fp_BN254.h"

using namespace core;

namespace BN254 {

/**
	@brief FP2 Structure - quadratic extension field
*/

typedef struct
{
    FP a; /**< real part of FP2 */
    FP b; /**< imaginary part of FP2 */
} FP2;

/* FP2 prototypes */

/**	@brief Tests for FP2 equal to zero
 *
	@param x FP2 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP2_iszilch(FP2 *x);

/**	@brief Serialize out FP2  
 *
    @param b buffer for output
	@param x FP2 number to be serialized
 */
extern void FP2_toBytes(char *b,FP2 *x);

/**	@brief Tests for FP2 equal to one
 *
	@param x FP2 instance to be tested
	@return 1 if x=1, else returns 0
 */
extern int FP2_isunity(FP2 *x);
/**	@brief Tests for equality of two FP2s
 *
	@param x FP2 instance to be compared
	@param y FP2 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP2_equals(FP2 *x, FP2 *y);
/**	@brief Initialise FP2 from two FP numbers
 *
	@param x FP2 instance to be initialised
	@param a FP to form real part of FP2
	@param b FP to form imaginary part of FP2
 */
extern void FP2_from_FPs(FP2 *x, FP *a, FP *b);

/**	@brief Initialise FP2 from two integers
 *
	@param x FP2 instance to be initialised
	@param a int to form real part of FP2
	@param b int to form imaginary part of FP2
 */
extern void FP2_from_ints(FP2 *x, int a, int b);



/**	@brief Initialise FP2 from single FP
 *
	Imaginary part is set to zero
	@param x FP2 instance to be initialised
	@param an FP to form real part of FP2
 */
extern void FP2_from_FP(FP2 *x, FP *a);
/**	@brief Initialise FP2 from single BIG
 *
	Imaginary part is set to zero
	@param x FP2 instance to be initialised
	@param a BIG to form real part of FP2
 */
extern void FP2_from_BIG(FP2 *x, B256_28::BIG a);
/**	@brief Copy FP2 to another FP2
 *
	@param x FP2 instance, on exit = y
	@param y FP2 instance to be copied
 */
extern void FP2_copy(FP2 *x, FP2 *y);
/**	@brief Set FP2 to zero
 *
	@param x FP2 instance to be set to zero
 */
extern void FP2_zero(FP2 *x);
/**	@brief Set FP2 to unity
 *
	@param x FP2 instance to be set to one
 */
extern void FP2_one(FP2 *x);

/**	@brief Copy from ROM to an FP2
 *
	@param w FP2 number to be copied to
	@param a BIG real part to be copied from ROM
	@param b BIG imag part to be copied from ROM
 */
extern void FP2_rcopy(FP2 *w,const B256_28::BIG a,const B256_28::BIG b);

/**	@brief Negation of FP2
 *
	@param x FP2 instance, on exit = -y
	@param y FP2 instance
 */
extern void FP2_neg(FP2 *x, FP2 *y);

/**	@brief addition of two FP2s
 *
	@param x FP2 instance, on exit = y+z
	@param y FP2 instance
	@param z FP2 instance
 */
extern void FP2_add(FP2 *x, FP2 *y, FP2 *z);
/**	@brief subtraction of two FP2s
 *
	@param x FP2 instance, on exit = y-z
	@param y FP2 instance
	@param z FP2 instance
 */
extern void FP2_sub(FP2 *x, FP2 *y, FP2 *z);

/**	@brief Multiplication of an FP2 by a small integer
 *
	@param x FP2 instance, on exit = y*i
	@param y FP2 instance
	@param i an integer
 */
extern void FP2_imul(FP2 *x, FP2 *y, int i);
/**	@brief Squaring an FP2
 *
	@param x FP2 instance, on exit = y^2
	@param y FP2 instance
 */
extern void FP2_sqr(FP2 *x, FP2 *y);
/**	@brief Multiplication of two FP2s
 *
	@param x FP2 instance, on exit = y*z
	@param y FP2 instance
	@param z FP2 instance
 */
extern void FP2_mul(FP2 *x, FP2 *y, FP2 *z);

/**	@brief Inverting an FP2
 *
	@param x FP2 instance, on exit = 1/y
	@param y FP2 instance
 */
extern void FP2_inv(FP2 *x, FP2 *y, FP *h);

/**	@brief Multiply an FP2 by (1+sqrt(-1))
 *
	Note that (1+sqrt(-1)) is irreducible for FP4
	@param x FP2 instance, on exit = x*(1+sqrt(-1))
 */
extern void FP2_mul_ip(FP2 *x);

/**	@brief Divide an FP2 by (1+sqrt(-1))
 *
	Note that (1+sqrt(-1)) is irreducible for FP4
	@param x FP2 instance, on exit = x/(1+sqrt(-1))
 */
extern void FP2_div_ip(FP2 *x);
/**	@brief Normalises the components of an FP2
 *
	@param x FP2 instance to be normalised
 */
extern void FP2_norm(FP2 *x);
/**	@brief Reduces all components of possibly unreduced FP2 mod Modulus
 *
	@param x FP2 instance, on exit reduced mod Modulus
 */
extern void FP2_reduce(FP2 *x);

/**	@brief Multiply an FP2 by sqrt(-1)
 *
	Note that -1 is QNR
	@param x FP2 instance, on exit = x*sqrt(-1)
 */
extern void FP2_times_i(FP2 *x);

}

#endif
