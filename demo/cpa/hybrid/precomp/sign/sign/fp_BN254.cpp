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

/* CORE mod p functions */
/* Small Finite Field arithmetic */
/* SU=m, SU is Stack Usage (NOT_SPECIAL Modulus) */

#include "fp_BN254.h"

using namespace B256_28;

/* Fast Modular Reduction Methods */

/* r=d mod m */
/* d MUST be normalised */
/* Products must be less than pR in all cases !!! */
/* So when multiplying two numbers, their product *must* be less than MODBITS_BN254+BASEBITS_B256_28*NLEN_B256_28 */
/* Results *may* be one bit bigger than MODBITS_BN254 */

/* convert to Montgomery n-residue form */
void BN254::FP_nres(FP *y, BIG x)
{
    DBIG d;
    BIG r;
    BIG_rcopy(r, R2modp);
    BIG_mul(d, x, r);
    FP_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void BN254::FP_redc(BIG x, FP *y)
{
    DBIG d;
    BIG_dzero(d);
    BIG_dscopy(d, y->g);
    FP_mod(x, d);
}


/* reduce a DBIG to a BIG using Montgomery's no trial division method */
/* d is expected to be dnormed before entry */
/* SU= 112 */
void BN254::FP_mod(BIG a, DBIG d)
{
    BIG mdls;
    BIG_rcopy(mdls, Modulus);
    BIG_monty(a, mdls, MConst, d);
}



void BN254::FP_from_int(FP *x,int a)
{
    BIG w;
    if (a<0) BIG_rcopy(w, Modulus);
    else BIG_zero(w); 
    BIG_inc(w,a); BIG_norm(w); 
    FP_nres(x,w);
}

/* test x==0 ? */
/* SU= 48 */
int BN254::FP_iszilch(FP *x)
{
    BIG m;
    FP y;
    BIG_copy(y.g, x->g);
    y.XES = x->XES;
    FP_reduce(&y);
    FP_redc(m,&y);
    return BIG_iszilch(m);
}

/* input must be reduced */
int BN254::FP_isunity(FP *x)
{
    BIG m;
    FP y;
     BIG_copy(y.g, x->g);
    y.XES = x->XES;
    FP_reduce(&y);
    FP_redc(m,&y);
    return BIG_isunity(m);
}

void BN254::FP_rcopy(FP *y, const BIG c)
{
    BIG b;
    BIG_rcopy(b, c);
    FP_nres(y, b);
}

int BN254::FP_equals(FP *x, FP *y)
{
    FP xg, yg;
    BIG_copy(xg.g, x->g);
    xg.XES = x->XES;

    BIG_copy(yg.g, y->g);
    yg.XES = y->XES;

    FP_reduce(&xg); FP_reduce(&yg);

    if (BIG_comp(xg.g, yg.g) == 0) return 1;
    return 0;
}

void BN254::FP_toBytes(char *b,FP *x)
{
    BIG t;
    FP_redc(t, x);
    BIG_toBytes(b, t);
}

/* r=a*b mod Modulus */
/* product must be less that p.R - and we need to know this in advance! */
/* SU= 88 */
void BN254::FP_mul(FP *r, FP *a, FP *b)
{
    DBIG d;

    if ((sign64)a->XES * b->XES > (sign64)FEXCESS_BN254)
    {
        FP_reduce(a);  /* it is sufficient to fully reduce just one of them < p */
    }

    BIG_mul(d, a->g, b->g);
    FP_mod(r->g, d);
    r->XES = 2;
}


/* multiplication by an integer, r=a*c */
/* SU= 136 */
void BN254::FP_imul(FP *r, FP *a, int c)
{
    DBIG d;
    BIG k;
    FP f;

    int s = 0;

    if (c < 0)
    {
        c = -c;
        s = 1;
    }

    //Montgomery
    if (a->XES * c <= FEXCESS_BN254)
    {
        BIG_pmul(r->g, a->g, c);
        r->XES = a->XES * c; // careful here - XES jumps!
    }
    else
    {   // don't want to do this - only a problem for Montgomery modulus and larger constants
        BIG_zero(k);
        BIG_inc(k, c);
        BIG_norm(k);
        FP_nres(&f, k);
        FP_mul(r, a, &f);
    }


    if (s)
    {
        FP_neg(r, r);
        BIG_norm(r->g);
    }
}

/* Set r=a^2 mod m */
/* SU= 88 */
void BN254::FP_sqr(FP *r, FP *a)
{
    DBIG d;

    if ((sign64)a->XES * a->XES > (sign64)FEXCESS_BN254)
    {
        FP_reduce(a);
    }

    BIG_sqr(d, a->g);
    FP_mod(r->g, d);
    r->XES = 2;
}

/* SU= 16 */
/* Set r=a+b */
void BN254::FP_add(FP *r, FP *a, FP *b)
{
    BIG_add(r->g, a->g, b->g);
    r->XES = a->XES + b->XES;
    if (r->XES > FEXCESS_BN254)
    {
        FP_reduce(r);
    }
}

/* Set r=a-b mod m */
/* SU= 56 */
void BN254::FP_sub(FP *r, FP *a, FP *b)
{
    FP n;
    FP_neg(&n, b);
    FP_add(r, a, &n);
}

// https://graphics.stanford.edu/~seander/bithacks.html
// constant time log to base 2 (or number of bits in)

static int logb2(unsign32 v)
{
    int r;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    r = (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
    return r;
}

// find appoximation to quotient of a/m
// Out by at most 2.
// Note that MAXXES is bounded to be 2-bits less than half a word
static int quo(BIG n, BIG m)
{
    int sh;
    chunk num, den;
    int hb = CHUNK / 2;
    if (TBITS_BN254 < hb)
    {
        sh = hb - TBITS_BN254;
        num = (n[NLEN_B256_28 - 1] << sh) | (n[NLEN_B256_28 - 2] >> (BASEBITS_B256_28 - sh));
        den = (m[NLEN_B256_28 - 1] << sh) | (m[NLEN_B256_28 - 2] >> (BASEBITS_B256_28 - sh));
    }
    else
    {
        num = n[NLEN_B256_28 - 1];
        den = m[NLEN_B256_28 - 1];
    }
    return (int)(num / (den + 1));
}

/* SU= 48 */
/* Fully reduce a mod Modulus */
void BN254::FP_reduce(FP *a)
{
    BIG m, r;
    int sr, sb, q;
    chunk carry;

    BIG_rcopy(m, Modulus);
    BIG_norm(a->g);

    if (a->XES > 16)
    {
        q = quo(a->g, m);
        carry = BIG_pmul(r, m, q);
        r[NLEN_B256_28 - 1] += (carry << BASEBITS_B256_28); // correction - put any carry out back in again
        BIG_sub(a->g, a->g, r);
        BIG_norm(a->g);
        sb = 2;
    }
    else sb = logb2(a->XES - 1); // sb does not depend on the actual data

    BIG_fshl(m, sb);
    while (sb > 0)
    {
// constant time...
        sr = BIG_ssn(r, a->g, m); // optimized combined shift, subtract and norm
        if(1 - sr) BIG_copy(a->g, r);
        sb--;
    }

    a->XES = 1;
}

/* Set r=-a mod Modulus */
/* SU= 64 */
void BN254::FP_neg(FP *r, FP *a)
{
    int sb;
    BIG m;

    BIG_rcopy(m, Modulus);

    sb = logb2(a->XES - 1);
    BIG_fshl(m, sb);
    BIG_sub(r->g, m, a->g);
    r->XES = ((sign32)1 << sb) + 1; // +1 to cover case where a is zero ?

    if (r->XES > FEXCESS_BN254)
    {
        FP_reduce(r);
    }

}

// Could leak size of b
// but not used here with secret exponent b
void BN254::FP_pow(FP *r, FP *a, BIG b)
{
    sign8 w[1 + (NLEN_B256_28 * BASEBITS_B256_28 + 3) / 4];
    FP tb[16];
    BIG t;
    int i, nb;

    BIG_copy(r->g, a->g);
    r->XES = a->XES;
    BIG_norm(r->g);
    BIG_copy(t, b);
    BIG_norm(t);
    nb = 1 + (BIG_nbits(t) + 3) / 4;
    // convert exponent to 4-bit window
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_lastbits(t, 4);
        BIG_dec(t, w[i]);
        BIG_norm(t);
        BIG_fshr(t, 4);
    }

    FP_one(&tb[0]);
    BIG_copy(tb[1].g, r->g);
    tb[1].XES = r->XES;
    for (i = 2; i < 16; i++) FP_mul(&tb[i], &tb[i - 1], r);
    BIG_copy(r->g, tb[w[nb - 1]].g);
    r->XES = tb[w[nb - 1]].XES;
    for (i = nb - 2; i >= 0; i--)
    {
        FP_sqr(r, r);
        FP_sqr(r, r);
        FP_sqr(r, r);
        FP_sqr(r, r);
        FP_mul(r, r, &tb[w[i]]);
    }
    FP_reduce(r);
}


// calculates r=x^(p-1-2^e)/2^{e+1) where 2^e|p-1
void BN254::FP_progen(FP *r,FP *x)
{
    BIG m;
    BIG_rcopy(m, Modulus);
    BIG_dec(m,1);
    BIG_shr(m,1);
    BIG_dec(m,1);
    BIG_fshr(m,1);
    FP_pow(r,x,m);
}

/* Is x a QR? return optional hint for fast follow-up square root */
int BN254::FP_qr(FP *x,FP *h)
{
    FP r;
    int i;
    FP_progen(&r,x);
    if (h!=NULL) 
    {
        BIG_copy(h->g, r.g);
        h->XES = r.XES;
    }

    FP_sqr(&r,&r);
    FP_mul(&r,x,&r);

    return FP_isunity(&r);
}

/* Modular inversion */
void BN254::FP_inv(FP *r,FP *x,FP *h)
{
    FP s,t;
    BIG_norm(x->g);
    BIG_copy(s.g, x->g);
    s.XES = x->XES;

    if (h==NULL)
        FP_progen(&t,x);
    else
    {
        BIG_copy(t.g, h->g);
        t.XES = h->XES;
    }
    FP_sqr(&t,&t);
    FP_sqr(&t,&t);

    FP_mul(r,&t,&s);
    FP_reduce(r);
}

/* SU=8 */
/* set n=1 */
void BN254::FP_one(FP *n)
{
    BIG b;
    BIG_one(b);
    FP_nres(n, b);
}

int BN254::FP_sign(FP *x)
{
    BIG m;
    FP y;
    BIG_copy(y.g, x->g);
    y.XES = x->XES;
    FP_reduce(&y);
    FP_redc(m,&y);
    return BIG_parity(m);
}

