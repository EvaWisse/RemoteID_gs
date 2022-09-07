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

/* CORE Elliptic Curve Functions */
/* SU=m, SU is Stack Usage (Weierstrass Curves) */

//#define HAS_MAIN

#include "ecp_BN254.h"

using namespace B256_28;
using namespace BN254;

/* test for P=O point-at-infinity */
int BN254::ECP_isinf(ECP *P)
{
    return (FP_iszilch(&(P->x)) && FP_iszilch(&(P->z)));
}

/* Constant time select from pre-computed table */
static void ECP_select(BN254::ECP *P, BN254::ECP W[], sign32 b)
{
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;
    ECP_copy(P, &W[babs]);
    if((int)(m & 1)) ECP_neg(P);
}

/* Set P=Q */
/* SU=16 */
void BN254::ECP_copy(ECP *P, ECP *Q)
{
    byte i;
    for (i = 0; i < NLEN_B256_28; i++) P->x.g[i] = Q->x.g[i];
    P->x.XES = Q->x.XES;

    for (i = 0; i < NLEN_B256_28; i++) P->y.g[i] = Q->y.g[i];
    P->y.XES = Q->y.XES;

    for (i = 0; i < NLEN_B256_28; i++) P->z.g[i] = Q->z.g[i];
    P->z.XES = Q->z.XES;
}

/* Set P=-Q */
/* SU=8 */
void BN254::ECP_neg(ECP *P)
{
    FP_neg(&(P->y), &(P->y));
    BIG_norm(P->y.g);
}

/* Calculate right Hand Side of curve equation y^2=RHS */
/* SU=56 */
void BN254::ECP_rhs(FP *v, FP *x)
{
    /* x^3+Ax+B */
    FP t;
    FP_sqr(&t, x);
    FP_mul(&t, &t, x);

    BIG_copy(v->g, t.g);
    v->XES = t.XES;

    FP_rcopy(&t, CURVE_B);
    FP_add(v, &t, v);
    FP_reduce(v);
}

/* Set P=(x,y) */

/* Extract (x,y) and return sign of y. If x and y are the same return only x */
/* SU=16 */
int BN254::ECP_get(BIG x, BIG y, ECP *P)
{
    ECP W;
    ECP_copy(&W, P);
    ECP_affine(&W);
    FP_redc(y, &(W.y));
    FP_redc(x, &(W.x));
    return FP_sign(&(W.y));
}

/* Set P=(x,{y}) */
/* SU=96 */
int BN254::ECP_set(ECP *P, BIG x, BIG y)
{
    FP rhs, y2;

    FP_nres(&y2, y);
    FP_sqr(&y2, &y2);
    FP_reduce(&y2);

    FP_nres(&rhs, x);
    ECP_rhs(&rhs, &rhs);

    FP_nres(&(P->x), x);
    FP_nres(&(P->y), y);
    FP_one(&(P->z));
    return 1;
}
/* Convert P to Affine, from (x,y,z) to (x,y) */
/* SU=160 */
void BN254::ECP_affine(ECP *P)
{
    FP one, iz;
    FP_one(&one);
    if (FP_equals(&(P->z), &one)) return;

    FP_inv(&iz, &(P->z), NULL);
    FP_mul(&(P->x), &(P->x), &iz);

    FP_mul(&(P->y), &(P->y), &iz);
    FP_reduce(&(P->y));

    FP_reduce(&(P->x));
    BIG_copy(P->z.g, one.g);
    P->z.XES = one.XES;
}


/* Set P=2P */
/* SU=272 */
void BN254::ECP_dbl(ECP *P)
{
    FP *t;
    t = (FP*) malloc(6 *sizeof(FP));


        FP_sqr(&t[0], &(P->y));                   //t[0].sqr();
        FP_mul(&t[1], &(P->y), &(P->z));          //t[1].mul(z);

        FP_sqr(&t[2], &(P->z));                   //t[2].sqr();

        FP_add(&(P->z), &t[0], &t[0]);          //z.add(t[0]);
        BIG_norm(P->z.g);                   //z.norm();
        FP_add(&(P->z), &(P->z), &(P->z));  //z.add(z);
        FP_add(&(P->z), &(P->z), &(P->z));  //z.add(z);
        BIG_norm(P->z.g);                   //z.norm();

        FP_imul(&t[2], &t[2], 3 * CURVE_B_I);   //t[2].imul(3*ROM.CURVE_B_I);
        FP_mul(&t[4], &t[2], &(P->z));          //t[4].mul(z);

        FP_add(&t[5], &t[0], &t[2]);              //t[5].add(t[2]);
        BIG_norm(t[5].g);                       //t[5].norm();
        FP_mul(&(P->z), &(P->z), &t[1]);      //z.mul(t[1]);

        FP_add(&t[1], &t[2], &t[2]);              //t[1].add(t[2]);
        FP_add(&t[2], &t[2], &t[1]);              //t[2].add(t[1]);
        FP_sub(&t[0], &t[0], &t[2]);              //t[0].sub(t[2]);
        BIG_norm(t[0].g);                       //t[0].norm();
        FP_mul(&t[5], &t[5], &t[0]);              //t[5].mul(t[0]);
        FP_add(&t[5], &t[5], &t[4]);              //t[5].add(t[4]);

        FP_mul(&t[1], &(P->x), &(P->y));          //t[1].mul(y);
        BIG_norm(t[0].g);                   //x.norm();
        FP_mul(&(P->x), &t[0], &t[1]);      //x.mul(t[1]);
        FP_add(&(P->x), &(P->x), &(P->x));  //x.add(x);
        BIG_norm(P->x.g);                   //x.norm();
        BIG_copy(P->y.g,  t[5].g);
        P->y.XES = t[5].XES;
        BIG_norm(P->y.g);                   //y.norm();
free(t);
}

/* Set P+=Q */
/* SU=248 */
void BN254::ECP_add(ECP *P, ECP *Q)
{

    int b3;
    FP *t; 
    t = (FP*) malloc(8 * sizeof(FP));

        b3 = 3 * CURVE_B_I;             //int b=3*ROM.CURVE_B_I;
        FP_mul(&t[0], &(P->x), &(Q->x));      //t[0].mul(Q.x);
        FP_mul(&t[1], &(P->y), &(Q->y));      //t[1].mul(Q.y);
        FP_mul(&t[2], &(P->z), &(Q->z));      //t[2].mul(Q.z);
        FP_add(&t[3], &(P->x), &(P->y));      //t[3].add(y);
        BIG_norm(t[3].g);                   //t[3].norm();
        FP_add(&t[4], &(Q->x), &(Q->y));      //t[4].add(Q.y);
        BIG_norm(t[4].g);                   //t[4].norm();
        FP_mul(&t[3], &t[3], &t[4]);          //t[3].mul(t[4]);
        FP_add(&t[4], &t[0], &t[1]);          //t[4].add(t[1]);

        FP_sub(&t[3], &t[3], &t[4]);          //t[3].sub(t[4]);
        BIG_norm(t[3].g);                   //t[3].norm();
        FP_add(&t[4], &(P->y), &(P->z));      //t[4].add(z);
        BIG_norm(t[4].g);                   //t[4].norm();
        FP_add(&t[5], &(Q->y), &(Q->z));      //t[5].add(Q.z);
        BIG_norm(t[5].g);                   //t[5].norm();

        FP_mul(&t[4], &t[4], &t[5]);          //t[4].mul(t[5]);
        FP_add(&t[5], &t[1], &t[2]);          //t[5].add(t[2]);

        FP_sub(&t[4], &t[4], &t[5]);          //t[4].sub(t[5]);
        BIG_norm(t[4].g);                   //t[4].norm();
        FP_add(&t[5], &(P->x), &(P->z));      //t[5].add(z);
        BIG_norm(t[5].g);                   //t[5].norm();
        FP_add(&t[6], &(Q->x), &(Q->z));      //t[6].add(Q.z);
        BIG_norm(t[6].g);                   //t[6].norm();
        FP_mul(&t[5], &t[5], &t[6]);          //t[5].mul(t[6]);
        FP_add(&t[6], &t[0], &t[2]);          //t[6].add(t[2]);
        FP_sub(&t[6], &t[5], &t[6]);          //t[6].rsub(t[5]);
        BIG_norm(t[6].g);                   //t[6].norm();
        FP_add(&t[5], &t[0], &t[0]);          //t[5].add(t[0]);
        FP_add(&t[0], &t[0], &t[5]);          //t[0].add(t[5]);
        BIG_norm(t[0].g);                   //t[0].norm();
        FP_imul(&t[2], &t[2], b3);          //t[2].imul(b);

        FP_add(&t[7], &t[1], &t[2]);          //t[7].add(t[2]);
        BIG_norm(t[7].g);                   //t[7].norm();
        FP_sub(&t[1], &t[1], &t[2]);          //t[1].sub(t[2]);
        BIG_norm(t[1].g);                   //t[1].norm();
        FP_imul(&t[6], &t[6], b3);          //t[6].imul(b);

        FP_mul(&t[5], &t[6], &t[4]);          //t[5].mul(t[4]);
        FP_mul(&t[2], &t[3], &t[1]);          //t[2].mul(t[1]);
        FP_sub(&(P->x), &t[2], &t[5]);          //t[5].rsub(t[2]);
        FP_mul(&t[6], &t[6], &t[0]);          //t[6].mul(t[0]);
        FP_mul(&t[1], &t[1], &t[7]);          //t[1].mul(t[7]);
        FP_add(&(P->y), &t[6], &t[1]);          //t[6].add(t[1]);
        FP_mul(&t[0], &t[0], &t[3]);          //t[0].mul(t[3]);
        FP_mul(&t[7], &t[7], &t[4]);          //t[7].mul(t[4]);
        FP_add(&(P->z), &t[7], &t[0]);          //t[7].add(t[0]);

        BIG_norm(P->x.g);               //x.norm();
        BIG_norm(P->y.g);               //y.norm();
        BIG_norm(P->z.g);               //z.norm();
        free(t);
}

/* Set P-=Q */
/* SU=16 */
void  BN254::ECP_sub(ECP *P, ECP *Q)
{
    ECP NQ;
    ECP_copy(&NQ, Q);
    ECP_neg(&NQ);
    ECP_add(P, &NQ);
}

// .. but this one does not (typically set maxe=r)
// Set P=e*P 
void BN254::ECP_clmul(ECP *P, BIG e, BIG maxe)
{
    BIG cm;
    BIG_or(cm,e,maxe);
    int max=BIG_nbits(cm);
    
    /* fixed size windows */
    int nb, s;
    BIG mt, t;
    ECP *W;
    W = (ECP*) malloc(10 * sizeof(ECP)); 
    
    sign8 *w;
    w = (sign8*) malloc((1 + (NLEN_B256_28 * BASEBITS_B256_28 + 3) / 4) * sizeof(sign8));

    /* precompute table */

    ECP_copy(&W[8], P);
    ECP_dbl(&W[8]);

    ECP_copy(&W[0], P);

    for (s = 1; s < 8; s++)
    {
        ECP_copy(&W[s], &W[s - 1]);
        ECP_add(&W[s], &W[8]);
    }

    /* make exponent odd - add 2P if even, P if odd */
    BIG_copy(t, e);
    s = BIG_parity(t);
    BIG_inc(t, 1);
    BIG_norm(t);
    nb = BIG_parity(t);
    BIG_copy(mt, t);
    BIG_inc(mt, 1);
    BIG_norm(mt);
    if(s) BIG_copy(t, mt);
    if(nb) ECP_copy(&W[8], P);
    ECP_copy(&W[9], &W[8]);

    nb = 1 + (max + 3) / 4;
    /* convert exponent to signed 4-bit window */
    for (s = 0; s < nb; s++)
    {
        w[s] = BIG_lastbits(t, 5) - 16;
        BIG_dec(t, w[s]);
        BIG_norm(t);
        BIG_fshr(t, 4);
    }
    w[nb] = BIG_lastbits(t, 5);

    ECP_select(P, W, w[nb]);
    for (s = nb - 1; s >= 0; s--)
    {
        ECP_select(&W[8], W, w[s]);
        ECP_dbl(P);
        ECP_dbl(P);
        ECP_dbl(P);
        ECP_dbl(P);
        ECP_add(P, &W[8]);
    }
    ECP_sub(P, &W[9]); /* apply correction */
    free(w);
    free(W);
}
int BN254::ECP_generator(ECP *G)
{
    BIG x, y;
    BIG_rcopy(x, CURVE_Gx);
    BIG_rcopy(y, CURVE_Gy);
    return ECP_set(G, x, y);
}
