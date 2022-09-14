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

#ifndef ECP2_BN254_H
#define ECP2_BN254_H

#include "fp2_BN254.h"
#include "config_curve_BN254.h"

using namespace core;


namespace BN254 {

extern const B256_28::BIG Fra; /**< real part of BN curve Frobenius Constant */
extern const B256_28::BIG Frb; /**< imaginary part of BN curve Frobenius Constant */

}

namespace BN254 {

/**
	@brief ECP2 Structure - Elliptic Curve Point over quadratic extension field
*/

typedef struct
{
//    int inf; /**< Infinity Flag */
    BN254::FP2 x;   /**< x-coordinate of point */
    BN254::FP2 y;   /**< y-coordinate of point */
    BN254::FP2 z;   /**< z-coordinate of point */
} ECP2;


/* Curve Params - see rom*.cpp */

extern const int CURVE_B_I;		/**< Elliptic curve B parameter */
extern const B256_28::BIG CURVE_B;     /**< Elliptic curve B parameter */
extern const B256_28::BIG CURVE_Order; /**< Elliptic curve group order */
extern const B256_28::BIG CURVE_Cof;   /**< Elliptic curve cofactor */
extern const B256_28::BIG CURVE_Bnx;   /**< Elliptic curve parameter */
extern const B256_28::BIG CURVE_HTPC;  /**< Hash to Point precomputation */

/* Generator point on G1 */
extern const B256_28::BIG CURVE_Gx; /**< x-coordinate of generator point in group G1  */
extern const B256_28::BIG CURVE_Gy; /**< y-coordinate of generator point in group G1  */

/* For Pairings only */

/* Generator point on G2 */
extern const B256_28::BIG CURVE_Pxa; /**< real part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxb; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pya; /**< real part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pyb; /**< imaginary part of y-coordinate of generator point in group G2 */



/* ECP2 E(Fp2) prototypes */
/**	@brief Tests for ECP2 point equal to infinity
 *
	@param P ECP2 point to be tested
	@return 1 if infinity, else returns 0
 */
extern int ECP2_isinf(ECP2 *P);
/**	@brief Copy ECP2 point to another ECP2 point
 *
	@param P ECP2 instance, on exit = Q
	@param Q ECP2 instance to be copied
 */
extern void ECP2_copy(ECP2 *P, ECP2 *Q);

/**	@brief Converts an ECP2 point from Projective (x,y,z) coordinates to affine (x,y) coordinates
 *
	@param P ECP2 instance to be converted to affine form
 */
extern void ECP2_affine(ECP2 *P);

/**	@brief Extract x and y coordinates of an ECP2 point P
 *
	If x=y, returns only x
	@param x FP2 on exit = x coordinate of point
	@param y FP2 on exit = y coordinate of point (unless x=y)
	@param P ECP2 instance (x,y)
	@return -1 if P is point-at-infinity, else 0
 */
extern int ECP2_get(BN254::FP2 *x, BN254::FP2 *y, ECP2 *P);

/**	@brief Calculate Right Hand Side of curve equation y^2=f(x)
 *
	Function f(x)=x^3+Ax+B
	Used internally.
	@param r FP2 value of f(x)
	@param x FP2 instance
 */
extern void ECP2_rhs(BN254::FP2 *r, BN254::FP2 *x);

/**	@brief Set ECP2 to point(x,y) given x and y
 *
	Point P set to infinity if no such point on the curve.
	@param P ECP2 instance to be set (x,y)
	@param x FP2 x coordinate of point
	@param y FP2 y coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP2_set(ECP2 *P, BN254::FP2 *x, BN254::FP2 *y);

/**	@brief Negation of an ECP2 point
 *
	@param P ECP2 instance, on exit = -P
 */
extern void ECP2_neg(ECP2 *P);

/**	@brief Doubles an ECP2 instance P
 *
	@param P ECP2 instance, on exit =2*P
 */
extern int ECP2_dbl(ECP2 *P);

/**	@brief Adds ECP2 instance Q to ECP2 instance P
 *
	@param P ECP2 instance, on exit =P+Q
	@param Q ECP2 instance to be added to P
 */
extern int ECP2_add(ECP2 *P, ECP2 *Q);

/**	@brief Subtracts ECP instance Q from ECP2 instance P
 *
	@param P ECP2 instance, on exit =P-Q
	@param Q ECP2 instance to be subtracted from P
 */
extern void ECP2_sub(ECP2 *P, ECP2 *Q);

/**	@brief Multiplies an ECP2 instance P by a BIG, side-channel resistant
 *
	Uses fixed sized windows.
	@param P ECP2 instance, on exit =b*P
	@param b BIG number multiplier
 */
extern void ECP2_mul(ECP2 *P, B256_28::BIG b);

/**	@brief Get Group Generator from ROM
 *
	@param G ECP2 instance
	@return 1 if point exists, else 0
 */
extern int ECP2_generator(ECP2 *G);
}

#endif
