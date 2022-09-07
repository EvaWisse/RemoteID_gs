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

/* CORE Weierstrass elliptic curve functions over FP2 */
/* SU=m, m is Stack Usage */

#include "ecp2_BN254.h"
#include "ecp_BN254.h"

using namespace B256_28;
using namespace BN254;

int BN254::ECP2_isinf(ECP2 *P)
{
    return (FP2_iszilch(&(P->x)) & FP2_iszilch(&(P->z)));
}

/* Set P=Q */
/* SU= 16 */
void BN254::ECP2_copy(ECP2 *P, ECP2 *Q)
{
    byte i;
	for(i=0; i<NLEN_B256_28; i++)
	{
        P->x.a.g[i] = Q->x.a.g[i];
        P->x.b.g[i] = Q->x.b.g[i];
    }
    
	P->x.a.XES = Q->x.a.XES;
	P->x.b.XES = Q->x.b.XES;

	for(i=0; i<NLEN_B256_28; i++)
    {
        P->y.a.g[i] = Q->y.a.g[i];
        P->y.b.g[i] = Q->y.b.g[i];
    }

	P->y.a.XES = Q->y.a.XES;
	P->y.b.XES = Q->y.b.XES;

	for(i=0; i<NLEN_B256_28; i++)
    {
        P->z.a.g[i] = Q->z.a.g[i];
        P->z.b.g[i] = Q->z.b.g[i];
	}
	P->z.a.XES = Q->z.a.XES;
	P->z.b.XES = Q->z.b.XES;
}

/* Constant time select from pre-computed table */
static void ECP2_select(BN254::ECP2 *P, BN254::ECP2 W[], sign32 b)
{
    BN254::ECP2 MP;
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;

    ECP2_copy(P, &W[babs]);
    if((int)(m & 1)) ECP2_neg(P);

}



/* Make P affine (so z=1) */
/* SU= 232 */
void BN254::ECP2_affine(ECP2 *P)
{
    FP2 one, iz;
    if (ECP2_isinf(P)) return;

    FP2_one(&one);
    if (FP2_isunity(&(P->z)))
    {
        FP2_reduce(&(P->x));
        FP2_reduce(&(P->y));
        return;
    }

    FP2_inv(&iz, &(P->z), NULL);
    FP2_mul(&(P->x), &(P->x), &iz);
    FP2_mul(&(P->y), &(P->y), &iz);

    FP2_reduce(&(P->x));
    FP2_reduce(&(P->y));
    FP2_copy(&(P->z), &one);
}

/* extract x, y from point P */
/* SU= 16 */
int BN254::ECP2_get(FP2 *x, FP2 *y, ECP2 *P)
{
    ECP2 W;
    ECP2_copy(&W, P);
    ECP2_affine(&W);
    if (ECP2_isinf(&W)) return -1;

    FP2_copy(y, &(W.y));
    FP2_copy(x, &(W.x));
    return 0;
}

/* SU= 128 */
/* Calculate RHS of twisted curve equation x^3+B/i or x^3+Bi*/
void BN254::ECP2_rhs(FP2 *rhs, FP2 *x)
{
    /* calculate RHS of elliptic curve equation */
    FP2 t;
    BIG b;

    FP2_sqr(&t, x);
    FP2_mul(rhs, &t, x);

    /* Assuming CURVE_A=0 */

    BIG_rcopy(b, CURVE_B);
    FP2_from_BIG(&t, b);
    FP2_div_ip(&t);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */

    FP2_add(rhs, &t, rhs);
    FP2_reduce(rhs);
}


/* Set P=(x,y). Return 1 if (x,y) is on the curve, else return 0*/
/* SU= 232 */
int BN254::ECP2_set(ECP2 *P, FP2 *x, FP2 *y)
{
    FP2 rhs, y2;
    FP2_sqr(&y2, y);
    ECP2_rhs(&rhs, x);

    FP2_copy(&(P->x), x);
    FP2_copy(&(P->y), y);

    FP2_one(&(P->z));
    return 1;
}

/* Set P=-P */
/* SU= 8 */
void BN254::ECP2_neg(ECP2 *P)
{
    FP2_norm(&(P->y));
    FP2_neg(&(P->y), &(P->y));
    FP2_norm(&(P->y));
}

/* R+=R */
/* return -1 for Infinity, 0 for addition, 1 for doubling */
/* SU= 448 */
int BN254::ECP2_dbl(ECP2 *P)
{
    FP2 t0, t1, t2, iy, x3, y3;

    FP2_copy(&iy, &(P->y));     //FP2 iy=new FP2(y);

    FP2_mul_ip(&iy);            //iy.mul_ip();
    FP2_norm(&iy);              //iy.norm();

    FP2_sqr(&t0, &(P->y));          //t0.sqr();

    FP2_mul_ip(&t0);            //t0.mul_ip();

    FP2_mul(&t1, &iy, &(P->z)); //t1.mul(z);
    FP2_sqr(&t2, &(P->z));              //t2.sqr();

    FP2_add(&(P->z), &t0, &t0); //z.add(t0);
    FP2_norm(&(P->z));              //z.norm();
    FP2_add(&(P->z), &(P->z), &(P->z)); //z.add(z);
    FP2_add(&(P->z), &(P->z), &(P->z)); //z.add(z);
    FP2_norm(&(P->z));          //z.norm();

    FP2_imul(&t2, &t2, 3 * CURVE_B_I); //t2.imul(3*ROM.CURVE_B_I);

    FP2_mul(&x3, &t2, &(P->z)); //x3.mul(z);

    FP2_add(&y3, &t0, &t2);     //y3.add(t2);
    FP2_norm(&y3);              //y3.norm();
    FP2_mul(&(P->z), &(P->z), &t1); //z.mul(t1);

    FP2_add(&t1, &t2, &t2);     //t1.add(t2);
    FP2_add(&t2, &t2, &t1);     //t2.add(t1);
    FP2_norm(&t2);              //t2.norm();
    FP2_sub(&t0, &t0, &t2);     //t0.sub(t2);
    FP2_norm(&t0);              //t0.norm();                           //y^2-9bz^2
    FP2_mul(&y3, &y3, &t0);     //y3.mul(t0);
    FP2_add(&(P->y), &y3, &x3);     //y3.add(x3);                      //(y^2+3z*2)(y^2-9z^2)+3b.z^2.8y^2
    FP2_mul(&t1, &(P->x), &iy);     //t1.mul(iy);

    FP2_norm(&t0);          //x.norm();
    FP2_mul(&(P->x), &t0, &t1); //x.mul(t1);
    FP2_add(&(P->x), &(P->x), &(P->x)); //x.add(x);       //(y^2-9bz^2)xy2

    FP2_norm(&(P->x));          //x.norm();
    FP2_norm(&(P->y));          //y.norm();

    return 1;
}

/* Set P+=Q */
/* SU= 400 */
int BN254::ECP2_add(ECP2 *P, ECP2 *Q)
{
    FP2 *t;
    t = (FP2*) malloc(8 * sizeof(FP2)); 
    int b3 = 3 * CURVE_B_I;

    FP2_mul(&t[0], &(P->x), &(Q->x)); //t[0].mul(Q.x);         // x.Q.x
    FP2_mul(&t[1], &(P->y), &(Q->y)); //t[1].mul(Q.y);       // y.Q.y

    FP2_mul(&t[2], &(P->z), &(Q->z)); //t[2].mul(Q.z);

    FP2_add(&t[3], &(P->x), &(P->y)); //t[3].add(y);
    FP2_norm(&t[3]);              //t[3].norm();          //t[3]=X1+Y1
    FP2_add(&t[4], &(Q->x), &(Q->y)); //t[4].add(Q.y);
    FP2_norm(&t[4]);              //t[4].norm();            //t[4]=X2+Y2
    FP2_mul(&t[3], &t[3], &t[4]);     //t[3].mul(t[4]);                       //t[3]=(X1+Y1)(X2+Y2)

    FP2_add(&t[4], &t[0], &t[1]);     //t[4].add(t[1]);       //t[4]=X1.X2+Y1.Y2

    FP2_sub(&t[3], &t[3], &t[4]);     //t[3].sub(t[4]);
    FP2_norm(&t[3]);              //t[3].norm();

    FP2_mul_ip(&t[3]);            //t[3].mul_ip();
    FP2_norm(&t[3]);              //t[3].norm();         //t[3]=(X1+Y1)(X2+Y2)-(X1.X2+Y1.Y2) = X1.Y2+X2.Y1

    FP2_add(&t[4], &(P->y), &(P->z)); //t[4].add(z);
    FP2_norm(&t[4]);              //t[4].norm();            //t[4]=Y1+Z1
    FP2_add(&t[5], &(Q->y), &(Q->z)); //t[5].add(Q.z);
    FP2_norm(&t[5]);              //t[5].norm();            //t[5]=Y2+Z2

    FP2_mul(&t[4], &t[4], &t[5]);     //t[4].mul(t[5]);                       //t[4]=(Y1+Z1)(Y2+Z2)
    FP2_add(&t[5], &t[1], &t[2]);     //t[5].add(t[2]);                       //X3=Y1.Y2+Z1.Z2

    FP2_sub(&t[4], &t[4], &t[5]);     //t[4].sub(t[5]);
    FP2_norm(&t[4]);              //t[4].norm();

    FP2_mul_ip(&t[4]);            //t[4].mul_ip();
    FP2_norm(&t[4]);              //t[4].norm();          //t[4]=(Y1+Z1)(Y2+Z2) - (Y1.Y2+Z1.Z2) = Y1.Z2+Y2.Z1

    FP2_add(&t[5], &(P->x), &(P->z)); //t[5].add(z);
    FP2_norm(&t[5]);              //t[5].norm();    // t[5]=X1+Z1
    FP2_add(&t[6], &(Q->x), &(Q->z)); //t[6].add(Q.z);
    FP2_norm(&t[6]);              //t[6].norm();                // t[6]=X2+Z2
    FP2_mul(&t[5], &t[5], &t[6]);     //t[5].mul(t[6]);                           // t[5]=(X1+Z1)(X2+Z2)
    FP2_add(&t[6], &t[0], &t[2]);     //t[6].add(t[2]);                           // t[6]=X1.X2+Z1+Z2
    FP2_sub(&t[6], &t[5], &t[6]);     //t[6].rsub(t[5]);
    FP2_norm(&t[6]);              //t[6].norm();                // t[6]=(X1+Z1)(X2+Z2) - (X1.X2+Z1.Z2) = X1.Z2+X2.Z1

    FP2_mul_ip(&t[0]);            //t[0].mul_ip();
    FP2_norm(&t[0]);              //t[0].norm(); // x.Q.x
    FP2_mul_ip(&t[1]);            //t[1].mul_ip();
    FP2_norm(&t[1]);              //t[1].norm(); // y.Q.y


    FP2_add(&t[5], &t[0], &t[0]);     //t[5].add(t[0]);
    FP2_add(&t[0], &t[0], &t[5]);     //t[0].add(t[5]);
    FP2_norm(&t[0]);              //t[0].norm();
    FP2_imul(&t[2], &t[2], b3);     //t[2].imul(b);

    FP2_add(&t[7], &t[1], &t[2]);     //t[7].add(t[2]);
    FP2_norm(&t[7]);              //t[7].norm();
    FP2_sub(&t[1], &t[1], &t[2]);     //t[1].sub(t[2]);
    FP2_norm(&t[1]);              //t[1].norm();

    FP2_imul(&t[6], &t[6], b3);     //t[6].imul(b);

    FP2_mul(&t[5], &t[6], &t[4]);     //t[5].mul(t[4]);
    FP2_mul(&t[2], &t[3], &t[1]);     //t[2].mul(t[1]);
    FP2_sub(&(P->x), &t[2], &t[5]);     //t[5].rsub(t[2]);
    FP2_mul(&t[6], &t[6], &t[0]);     //t[6].mul(t[0]);
    FP2_mul(&t[1], &t[1], &t[7]);     //t[1].mul(t[7]);
    FP2_add(&(P->y), &t[6], &t[1]);     //t[6].add(t[1]);

    FP2_mul(&t[0], &t[0], &t[3]);     //t[0].mul(t[3]);
    FP2_mul(&t[7], &t[7], &t[4]);     //t[7].mul(t[4]);
    FP2_add(&(P->z), &t[7], &t[0]);     //t[7].add(t[0]);

    FP2_norm(&(P->x));          //x.norm();
    FP2_norm(&(P->y));          //y.norm();
    FP2_norm(&(P->z));          //z.norm();
    free(t);
    return 0;
}

/* Set P-=Q */
/* SU= 16 */
void BN254::ECP2_sub(ECP2 *P, ECP2 *Q)
{
    ECP2 NQ;
    ECP2_copy(&NQ, Q);
    ECP2_neg(&NQ);
    ECP2_add(P, &NQ);
}

/* P*=e */
/* SU= 280 */
void BN254::ECP2_mul(ECP2 *P, BIG e)
{
    /* fixed size windows */
    int i, nb, s, ns;
    BIG mt, t;
    ECP2 Q, W[8], C;
    sign8 w[1 + (NLEN_B256_28 * BASEBITS_B256_28 + 3) / 4];

    if (ECP2_isinf(P)) return;

    /* precompute table */

    ECP2_copy(&Q, P);
    ECP2_dbl(&Q);
    ECP2_copy(&W[0], P);

    for (i = 1; i < 8; i++)
    {
        ECP2_copy(&W[i], &W[i - 1]);
        ECP2_add(&W[i], &Q);
    }

    /* make exponent odd - add 2P if even, P if odd */
    BIG_copy(t, e);
    s = BIG_parity(t);
    BIG_inc(t, 1);
    BIG_norm(t);
    ns = BIG_parity(t);
    BIG_copy(mt, t);
    BIG_inc(mt, 1);
    BIG_norm(mt);
    if(s) BIG_copy(t, mt);
    if(ns) ECP2_copy(&Q, P);
    ECP2_copy(&C, &Q);

    nb = 1 + (BIG_nbits(t) + 3) / 4;

    /* convert exponent to signed 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_lastbits(t, 5) - 16;
        BIG_dec(t, w[i]);
        BIG_norm(t);
        BIG_fshr(t, 4);
    }
    w[nb] = BIG_lastbits(t, 5);

    //ECP2_copy(P, &W[(w[nb] - 1) / 2]);
    ECP2_select(P, W, w[nb]);
    for (i = nb - 1; i >= 0; i--)
    {
        ECP2_select(&Q, W, w[i]);
        ECP2_dbl(P);
        ECP2_dbl(P);
        ECP2_dbl(P);
        ECP2_dbl(P);
        ECP2_add(P, &Q);
    }
    ECP2_sub(P, &C); /* apply correction */
}

int BN254::ECP2_generator(ECP2 *G)
{
    FP2 wx, wy;
    FP2_rcopy(&wx,CURVE_Pxa,CURVE_Pxb);
    FP2_rcopy(&wy,CURVE_Pya,CURVE_Pyb);
    return ECP2_set(G, &wx, &wy);
}


