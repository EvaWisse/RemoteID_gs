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

/* CORE Fp^2 functions */
/* SU=m, m is Stack Usage (no lazy )*/

/* FP2 elements are of the form a+ib, where i is sqrt(-1) */

#include "fp2_BN254.h"

using namespace B256_28;

/* test x==0 ? */
/* SU= 8 */
int BN254::FP2_iszilch(FP2 *x)
{
    return (FP_iszilch(&(x->a)) & FP_iszilch(&(x->b)));
}

/* test x==1 ? */
/* SU= 48 */
int BN254::FP2_isunity(FP2 *x)
{
    FP one;
    FP_one(&one);
    return (FP_equals(&(x->a), &one) & FP_iszilch(&(x->b)));
}

/* SU= 8 */
/* Fully reduce a and b mod Modulus */
void BN254::FP2_reduce(FP2 *w)
{
    FP_reduce(&(w->a));
    FP_reduce(&(w->b));
}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int BN254::FP2_equals(FP2 *x, FP2 *y)
{
    return (FP_equals(&(x->a), &(y->a)) & FP_equals(&(x->b), &(y->b)));
}

void BN254::FP2_toBytes(char *b,FP2 *x)
{
    BIG t;
    FP_redc(t, &(x->b));
    BIG_toBytes(b, t);

    FP_redc(t, &(x->a));
    BIG_toBytes(b + MODBYTES_B256_28, t);
}

/* Create FP2 from two FPs */
/* SU= 16 */
void BN254::FP2_from_FPs(FP2 *w, FP *x, FP *y)
{
    BIG_copy(w->a.g, x->g);
    w->a.XES = x->XES;

    BIG_copy(w->b.g, y->g);
    w->b.XES = y->XES;
}

/* Create FP2 from two ints */
void BN254::FP2_from_ints(FP2 *w, int xa, int xb)
{
    FP a,b;
    FP_from_int(&a,xa);
    FP_from_int(&b,xb);
    FP2_from_FPs(w,&a,&b);
}

/* Create FP2 from FP */
/* SU= 8 */
void BN254::FP2_from_FP(FP2 *w, FP *x)
{
    BIG_copy(w->a.g, x->g);
    w->a.XES = x->XES;

    BIG_zero(w->b.g);
    w->b.XES = 1;
}

/* Create FP2 from BIG */
/* SU= 8 */
void BN254::FP2_from_BIG(FP2 *w, BIG x)
{
    FP_nres(&(w->a), x);
    BIG_zero(w->b.g);
    w->b.XES = 1;
}

/* FP2 copy w=x */
/* SU= 16 */
void BN254::FP2_copy(FP2 *w, FP2 *x)
{
    BIG_copy(w->a.g, x->a.g);
    w->a.XES = x->a.XES;
    
    BIG_copy(w->b.g, x->b.g);
    w->b.XES = x->b.XES;
}

/* FP2 set w=0 */
/* SU= 8 */
void BN254::FP2_zero(FP2 *w)
{
    BIG_zero(w->a.g);
    w->a.XES = 1;

    BIG_zero(w->b.g);
    w->b.XES = 1;
}

/* FP2 set w=1 */
/* SU= 48 */
void BN254::FP2_one(FP2 *w)
{
    FP one;
    FP_one(&one);
    FP2_from_FP(w, &one);
}

void BN254::FP2_rcopy(FP2 *w,const BIG a,const BIG b)
{
    BIG big;
    BIG_rcopy(big, a);
    FP_nres(&(w->a), big);
    
    BIG_rcopy(big, b);
    FP_nres(&(w->b), big);
}

/* Set w=-x */
/* SU= 88 */
void BN254::FP2_neg(FP2 *w, FP2 *x)
{
    /* Just one neg! */
    FP m, t;
    FP_add(&m, &(x->a), &(x->b));
    FP_neg(&m, &m);
    FP_add(&t, &m, &(x->b));
    FP_add(&(w->b), &m, &(x->a));
    BIG_copy(w->a.g, t.g);
    w->a.XES = t.XES;

}

/* Set w=x+y */
/* SU= 16 */
void BN254::FP2_add(FP2 *w, FP2 *x, FP2 *y)
{
    FP_add(&(w->a), &(x->a), &(y->a));
    FP_add(&(w->b), &(x->b), &(y->b));
}

/* Set w=x-y */
/* Input y MUST be normed */
void BN254::FP2_sub(FP2 *w, FP2 *x, FP2 *y)
{
    FP2 m;
    FP2_neg(&m, y);
    FP2_add(w, x, &m);
}

/* SU= 16 */
/* Set w=s*x, where s is int */
void BN254::FP2_imul(FP2 *w, FP2 *x, int s)
{
    FP_imul(&(w->a), &(x->a), s);
    FP_imul(&(w->b), &(x->b), s);
}

/* Set w=x^2 */
/* SU= 128 */
void BN254::FP2_sqr(FP2 *w, FP2 *x)
{
    FP w1, w3, mb;

    FP_add(&w1, &(x->a), &(x->b));
    FP_neg(&mb, &(x->b));

    FP_add(&w3, &(x->a), &(x->a));
    BIG_norm(w3.g);
    FP_mul(&(w->b), &w3, &(x->b));

    FP_add(&(w->a), &(x->a), &mb);

    BIG_norm(w1.g);
    BIG_norm(w->a.g);

    FP_mul(&(w->a), &w1, &(w->a));   /* w->a#2 w->a=1 w1&w2=6 w1*w2=2 */

//  BN254::fp2sqrs++;
}

/* Set w=x*y */
/* Inputs MUST be normed  */
/* Now uses Lazy reduction */
void BN254::FP2_mul(FP2 *w, FP2 *x, FP2 *y)
{
    DBIG A, B, E, F, pR;
    BIG C, D, p;

    BIG_rcopy(p, Modulus);
    BIG_dsucopy(pR, p);

// reduce excesses of a and b as required (so product < pR)

    if ((sign64)(x->a.XES + x->b.XES) * (y->a.XES + y->b.XES) > (sign64)FEXCESS_BN254)
    {
        if (x->a.XES > 1) FP_reduce(&(x->a));
        if (x->b.XES > 1) FP_reduce(&(x->b));
    }

    BIG_mul(A, x->a.g, y->a.g);
    BIG_mul(B, x->b.g, y->b.g);

    BIG_add(C, x->a.g, x->b.g); BIG_norm(C);
    BIG_add(D, y->a.g, y->b.g); BIG_norm(D);

    BIG_mul(E, C, D);
    BIG_dadd(F, A, B);
    BIG_dsub(B, pR, B); //

    BIG_dadd(A, A, B);  // A<pR? Not necessarily, but <2pR
    BIG_dsub(E, E, F);  // E<pR ? Yes

    BIG_dnorm(A); FP_mod(w->a.g, A);  w->a.XES = 3; // may drift above 2p...
    BIG_dnorm(E); FP_mod(w->b.g, E);  w->b.XES = 2;

//  BN254::fp2muls++;
}

/* Set w=1/x */
/* SU= 128 */
void BN254::FP2_inv(FP2 *w, FP2 *x, FP *h)
{
    FP w1, w2;

    FP2_norm(x);
    FP_sqr(&w1, &(x->a));
    FP_sqr(&w2, &(x->b));
    FP_add(&w1, &w1, &w2);
    FP_inv(&w1, &w1, h);
    FP_mul(&(w->a), &(x->a), &w1);
    FP_neg(&w1, &w1);
    BIG_norm(w1.g);
    FP_mul(&(w->b), &(x->b), &w1);
}

/* Input MUST be normed */
void BN254::FP2_times_i(FP2 *w)
{
    FP z;
    BIG_copy(z.g, w->a.g);
    z.XES = w->a.XES;

    FP_neg(&(w->a), &(w->b));
    BIG_copy(w->b.g, z.g);
    w->b.XES = z.XES;

//    Output NOT normed, so use with care
}

/* Input MUST be normed */
void BN254::FP2_mul_ip(FP2 *w)
{
    FP2 t;

    FP2_copy(&t, w);
    FP2_times_i(w);

    FP2_add(w, &t, w);
}

/* Set w/=(2^i+sqrt(-1)) */
/* Slow */
void BN254::FP2_div_ip(FP2 *w)
{
    FP2 z;
    FP2_norm(w);
    FP2_from_ints(&z, (1 << 0), 1);
    FP2_inv(&z, &z, NULL);
    FP2_mul(w, &z, w);
}

/* SU= 8 */
/* normalise a and b components of w */
void BN254::FP2_norm(FP2 *w)
{
    BIG_norm(w->a.g);
    BIG_norm(w->b.g);
}
