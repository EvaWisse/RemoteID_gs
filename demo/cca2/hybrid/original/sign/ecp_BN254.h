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

#ifndef ECP_BN254_H
#define ECP_BN254_H

#include "fp_BN254.h"
#include "config_curve_BN254.h"

using namespace core;

namespace BN254 {

/* Curve Params - see rom*.cpp */
extern const int CURVE_B_I;
extern const int CURVE_Cof_I;
extern const B256_28::BIG CURVE_B;     /**< Elliptic curve B parameter */
extern const B256_28::BIG CURVE_Order; /**< Elliptic curve group order */
extern const B256_28::BIG CURVE_Cof;   /**< Elliptic curve cofactor */
extern const B256_28::BIG CURVE_HTPC;  /**< Hash to Point precomputation */
extern const B256_28::BIG CURVE_HTPC2;  /**< Hash to Point precomputation for G2 */

extern const B256_28::BIG CURVE_Ad;
extern const B256_28::BIG CURVE_Bd;
extern const B256_28::BIG PC[];

extern const B256_28::BIG CURVE_Adr;
extern const B256_28::BIG CURVE_Adi;
extern const B256_28::BIG CURVE_Bdr;
extern const B256_28::BIG CURVE_Bdi;
extern const B256_28::BIG PCR[];
extern const B256_28::BIG PCI[];

/* Generator point on G1 */
extern const B256_28::BIG CURVE_Gx; /**< x-coordinate of generator point in group G1  */
extern const B256_28::BIG CURVE_Gy; /**< y-coordinate of generator point in group G1  */


/* For Pairings only */

/* Generator point on G2 */
extern const B256_28::BIG CURVE_Pxa; /**< real part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxb; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pya; /**< real part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pyb; /**< imaginary part of y-coordinate of generator point in group G2 */

/*** needed for BLS24 curves ***/

extern const B256_28::BIG CURVE_Pxaa; /**< real part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxab; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxba; /**< real part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxbb; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pyaa; /**< real part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pyab; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pyba; /**< real part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pybb; /**< imaginary part of y-coordinate of generator point in group G2 */

/*** needed for BLS48 curves ***/

extern const B256_28::BIG CURVE_Pxaaa; /**< real part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxaab; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxaba; /**< real part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxabb; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxbaa; /**< real part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxbab; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxbba; /**< real part of x-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pxbbb; /**< imaginary part of x-coordinate of generator point in group G2 */

extern const B256_28::BIG CURVE_Pyaaa; /**< real part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pyaab; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pyaba; /**< real part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pyabb; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pybaa; /**< real part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pybab; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pybba; /**< real part of y-coordinate of generator point in group G2 */
extern const B256_28::BIG CURVE_Pybbb; /**< imaginary part of y-coordinate of generator point in group G2 */



extern const B256_28::BIG CURVE_Bnx; /**< BN curve x parameter */



extern const B256_28::BIG Fra; /**< real part of BN curve Frobenius Constant */
extern const B256_28::BIG Frb; /**< imaginary part of BN curve Frobenius Constant */


extern const B256_28::BIG CURVE_W[2];	 /**< BN curve constant for GLV decomposition */
extern const B256_28::BIG CURVE_SB[2][2]; /**< BN curve constant for GLV decomposition */
extern const B256_28::BIG CURVE_WB[4];	 /**< BN curve constant for GS decomposition */
extern const B256_28::BIG CURVE_BB[4][4]; /**< BN curve constant for GS decomposition */


/**
	@brief ECP structure - Elliptic Curve Point over base field
*/

typedef struct
{

    BN254::FP x; /**< x-coordinate of point */
    BN254::FP y; /**< y-coordinate of point.  */
    BN254::FP z;/**< z-coordinate of point */
} ECP;


/* ECP E(Fp) prototypes */
/**	@brief Tests for ECP point equal to infinity
 *
	@param P ECP point to be tested
	@return 1 if infinity, else returns 0
 */
extern int ECP_isinf(ECP *P);

/**	@brief Copy ECP point to another ECP point
 *
	@param P ECP instance, on exit = Q
	@param Q ECP instance to be copied
 */
extern void ECP_copy(ECP *P, ECP *Q);
/**	@brief Negation of an ECP point
 *
	@param P ECP instance, on exit = -P
 */
extern void ECP_neg(ECP *P);

/**	@brief Calculate Right Hand Side of curve equation y^2=f(x)
 *
	Function f(x) depends on form of elliptic curve, Weierstrass, or Montgomery.
	Used internally.
	@param r BIG n-residue value of f(x)
	@param x BIG n-residue x
 */
extern void ECP_rhs(BN254::FP *r, BN254::FP *x);

/**	@brief Set ECP to point(x,y) given x and y
 *
	Point P set to infinity if no such point on the curve.
	@param P ECP instance to be set (x,y)
	@param x BIG x coordinate of point
	@param y BIG y coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP_set(ECP *P, B256_28::BIG x, B256_28::BIG y);
/**	@brief Extract x and y coordinates of an ECP point P
 *
	If x=y, returns only x
	@param x BIG on exit = x coordinate of point
	@param y BIG on exit = y coordinate of point (unless x=y)
	@param P ECP instance (x,y)
	@return sign of y, or -1 if P is point-at-infinity
 */
extern int ECP_get(B256_28::BIG x, B256_28::BIG y, ECP *P);
/**	@brief Adds ECP instance Q to ECP instance P
 *
	@param P ECP instance, on exit =P+Q
	@param Q ECP instance to be added to P
 */
extern void ECP_add(ECP *P, ECP *Q);
/**	@brief Subtracts ECP instance Q from ECP instance P
 *
	@param P ECP instance, on exit =P-Q
	@param Q ECP instance to be subtracted from P
 */
extern void ECP_sub(ECP *P, ECP *Q);
#endif
/**	@brief Converts an ECP point from Projective (x,y,z) coordinates to affine (x,y) coordinates
 *
	@param P ECP instance to be converted to affine form
 */
extern void ECP_affine(ECP *P);

/**	@brief Doubles an ECP instance P
 *
	@param P ECP instance, on exit =2*P
 */
extern void ECP_dbl(ECP *P);

/**	@brief Multiplies an ECP instance P by a BIG, side-channel resistant
 *
	Uses Montgomery ladder for Montgomery curves, otherwise fixed sized windows.
	@param P ECP instance, on exit =b*P
	@param e BIG number multiplier
    @param maxe maximum e

 */
extern void ECP_clmul(ECP *P, B256_28::BIG e, B256_28::BIG maxe);

/**	@brief Get Group Generator from ROM
 *
	@param G ECP instance
    @return true or false
 */
extern int ECP_generator(ECP *G);

}
