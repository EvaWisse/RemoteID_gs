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

/* CORE basic functions for BIG type */
/* SU=m, SU is Stack Usage */

#include "big_B256_28.h"

/* test a=0? */
int B256_28::BIG_iszilch(BIG a)
{
    byte i;
    chunk d=0;
    for (i = 0; i < NLEN_B256_28; i++) d|=a[i];
    return (1 & ((d-1)>>BASEBITS_B256_28));
}

/* test a=1? */
int B256_28::BIG_isunity(BIG a)
{
    byte i;
    chunk d=0;
    for (i = 1; i < NLEN_B256_28; i++) d|=a[i];
    return (1 & ((d-1)>>BASEBITS_B256_28) & (((a[0]^1)-1)>>BASEBITS_B256_28));
}

/* test a=0? */
int B256_28::BIG_diszilch(DBIG a)
{
    byte i;
    chunk d=0;
    for (i = 0; i < DNLEN_B256_28; i++) d|=a[i];
    return (1 & ((d-1)>>BASEBITS_B256_28));
}

/* convert BIG to/from bytes */
/* SU= 64 */
void B256_28::BIG_toBytes(char *b, BIG a)
{
    int i;
    BIG c;
    BIG_copy(c, a);
    BIG_norm(c);
    for (i = MODBYTES_B256_28 - 1; i >= 0; i--)
    {
        b[i] = c[0] & 0xff;
        BIG_fshr(c, 8);
    }
}

/* SU= 16 */
void B256_28::BIG_fromBytes(BIG a, char *b)
{
    byte i;
    BIG_zero(a);
    for (i = 0; i < MODBYTES_B256_28; i++)
    {
        BIG_fshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
}

void B256_28::BIG_fromBytesLen(BIG a, char *b, int s)
{
    int len = s;
    byte i;
    BIG_zero(a);

    if (len > MODBYTES_B256_28) len = MODBYTES_B256_28;
    for (i = 0; i < len; i++)
    {
        BIG_fshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
}

/* Copy b=a */
void B256_28::BIG_copy(BIG b, BIG a)
{
    byte i;
    for (i = 0; i < NLEN_B256_28; i++)  b[i] = a[i];
}

/* Copy from ROM b=a */
void B256_28::BIG_rcopy(BIG b, const BIG a)
{
    byte i;
    for (i = 0; i < NLEN_B256_28; i++) b[i] = a[i];
}

/* double length DBIG copy b=a */
void B256_28::BIG_dcopy(DBIG b, DBIG a)
{
    byte i;
    for (i = 0; i < DNLEN_B256_28; i++) b[i] = a[i];
}

/* Copy BIG to bottom half of DBIG */
void B256_28::BIG_dscopy(DBIG b, BIG a)
{
    byte i;
    for (i = 0; i < NLEN_B256_28 - 1; i++) b[i] = a[i];

    b[NLEN_B256_28 - 1] = a[NLEN_B256_28 - 1] & BMASK_B256_28; /* top word normalized */
    b[NLEN_B256_28] = a[NLEN_B256_28 - 1] >> BASEBITS_B256_28;

    for (i = NLEN_B256_28 + 1; i < DNLEN_B256_28; i++) b[i] = 0;
}

/* Copy BIG to top half of DBIG */
void B256_28::BIG_dsucopy(DBIG b, BIG a)
{
    byte i;
    for (i = 0; i < NLEN_B256_28; i++) b[i] = 0;
    for (i = NLEN_B256_28; i < DNLEN_B256_28; i++)  b[i] = a[i - NLEN_B256_28];
}

/* Copy bottom half of DBIG to BIG */
void B256_28::BIG_sdcopy(BIG b, DBIG a)
{
    byte i;
    for (i = 0; i < NLEN_B256_28; i++) b[i] = a[i];
}

/* Copy top half of DBIG to BIG */
void B256_28::BIG_sducopy(BIG b, DBIG a)
{
    byte i;
    for (i = 0; i < NLEN_B256_28; i++)  b[i] = a[i + NLEN_B256_28];
}

/* Set a=0 */
void B256_28::BIG_zero(BIG a)
{
    byte i;
    for (i = 0; i < NLEN_B256_28; i++) a[i] = 0;
}

void B256_28::BIG_dzero(DBIG a)
{
    byte i;
    for (i = 0; i < DNLEN_B256_28; i++) a[i] = 0;
}

/* set a=1 */
void B256_28::BIG_one(BIG a)
{
    byte i;
    a[0] = 1;
    for (i = 1; i < NLEN_B256_28; i++) a[i] = 0;
}

/* Set c=a+b */
/* SU= 8 */
void B256_28::BIG_add(BIG c, BIG a, BIG b)
{
    byte i;
    for (i = 0; i < NLEN_B256_28; i++)  c[i] = a[i] + b[i];
}

/* Set c=a or b */
/* SU= 8 */
void B256_28::BIG_or(BIG c, BIG a, BIG b)
{
    byte i;
    BIG_norm(a);
    BIG_norm(b);
    for (i = 0; i < NLEN_B256_28; i++) c[i] = a[i] | b[i];
}


/* Set c=c+d */
void B256_28::BIG_inc(BIG c, int d)
{
    BIG_norm(c);
    c[0] += (chunk)d;
}

/* Set c=a-b */
/* SU= 8 */
void B256_28::BIG_sub(BIG c, BIG a, BIG b)
{
    byte i;
    for (i = 0; i < NLEN_B256_28; i++) c[i] = a[i] - b[i];
}

/* SU= 8 */

void B256_28::BIG_dsub(DBIG c, DBIG a, DBIG b)
{
    byte i;
    for (i = 0; i < DNLEN_B256_28; i++) c[i] = a[i] - b[i];
}

void B256_28::BIG_dadd(DBIG c, DBIG a, DBIG b)
{
    byte i;
    for (i = 0; i < DNLEN_B256_28; i++) c[i] = a[i] + b[i];
}

/* Set c=c-1 */
void B256_28::BIG_dec(BIG c, int d)
{
    BIG_norm(c);
    c[0] -= (chunk)d;
}

/* multiplication r=a*c by c<=NEXCESS_B256_28 */
void B256_28::BIG_imul(BIG r, BIG a, int c)
{
    byte i;
    for (i = 0; i < NLEN_B256_28; i++) r[i] = a[i] * c;
}

/* multiplication r=a*c by larger integer - c<=FEXCESS */
/* SU= 24 */
chunk B256_28::BIG_pmul(BIG r, BIG a, int c)
{
    byte i;
    chunk ak, carry = 0;
    for (i = 0; i < NLEN_B256_28; i++)
    {
        ak = a[i];
        r[i] = 0;
        carry = muladd(ak, (chunk)c, carry, &r[i]);
    }

    return carry;
}

/* multiplication c=a*b by even larger integer b>FEXCESS, resulting in DBIG */
/* SU= 24 */
void B256_28::BIG_pxmul(DBIG c, BIG a, int b)
{
    byte j;
    chunk carry;
    BIG_dzero(c);
    carry = 0;
    for (j = 0; j < NLEN_B256_28; j++) carry = muladd(a[j], (chunk)b, carry, &c[j]);
    c[NLEN_B256_28] = carry;
}

/* .. if you know the result will fit in a BIG, c must be distinct from a and b */
/* SU= 40 */
void B256_28::BIG_smul(BIG c, BIG a, BIG b)
{
    byte i, j;
    chunk carry;

    BIG_zero(c);
    for (i = 0; i < NLEN_B256_28; i++)
    {
        carry = 0;
        for (j = 0; j < NLEN_B256_28; j++)
        {
            if (i + j < NLEN_B256_28) carry = muladd(a[i], b[j], carry, &c[i + j]);
        }
    }
}

/* Set c=a*b */
/* SU= 72 */
void B256_28::BIG_mul(DBIG c, BIG a, BIG b)
{
    dchunk co,t;
    dchunk s;
    dchunk *d;
    d  = (dchunk*) malloc(NLEN_B256_28 * sizeof(dchunk));

    d[0]=(dchunk)a[0]*b[0];
	d[1]=(dchunk)a[1]*b[1];
	d[2]=(dchunk)a[2]*b[2];
	d[3]=(dchunk)a[3]*b[3];
	d[4]=(dchunk)a[4]*b[4];
	d[5]=(dchunk)a[5]*b[5];
	d[6]=(dchunk)a[6]*b[6];
	d[7]=(dchunk)a[7]*b[7];
	d[8]=(dchunk)a[8]*b[8];
	d[9]=(dchunk)a[9]*b[9];

	s=d[0];
	t = s; c[0]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28;
	s+=d[1]; t=co+s +(dchunk)(a[1]-a[0])*(b[0]-b[1]); c[1]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s+=d[2]; t=co+s +(dchunk)(a[2]-a[0])*(b[0]-b[2]); c[2]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s+=d[3]; t=co+s +(dchunk)(a[3]-a[0])*(b[0]-b[3])+(dchunk)(a[2]-a[1])*(b[1]-b[2]); c[3]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s+=d[4]; t=co+s +(dchunk)(a[4]-a[0])*(b[0]-b[4])+(dchunk)(a[3]-a[1])*(b[1]-b[3]); c[4]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s+=d[5]; t=co+s +(dchunk)(a[5]-a[0])*(b[0]-b[5])+(dchunk)(a[4]-a[1])*(b[1]-b[4])+(dchunk)(a[3]-a[2])*(b[2]-b[3]); c[5]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s+=d[6]; t=co+s +(dchunk)(a[6]-a[0])*(b[0]-b[6])+(dchunk)(a[5]-a[1])*(b[1]-b[5])+(dchunk)(a[4]-a[2])*(b[2]-b[4]); c[6]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s+=d[7]; t=co+s +(dchunk)(a[7]-a[0])*(b[0]-b[7])+(dchunk)(a[6]-a[1])*(b[1]-b[6])+(dchunk)(a[5]-a[2])*(b[2]-b[5])+(dchunk)(a[4]-a[3])*(b[3]-b[4]); c[7]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s+=d[8]; t=co+s +(dchunk)(a[8]-a[0])*(b[0]-b[8])+(dchunk)(a[7]-a[1])*(b[1]-b[7])+(dchunk)(a[6]-a[2])*(b[2]-b[6])+(dchunk)(a[5]-a[3])*(b[3]-b[5]); c[8]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s+=d[9]; t=co+s +(dchunk)(a[9]-a[0])*(b[0]-b[9])+(dchunk)(a[8]-a[1])*(b[1]-b[8])+(dchunk)(a[7]-a[2])*(b[2]-b[7])+(dchunk)(a[6]-a[3])*(b[3]-b[6])+(dchunk)(a[5]-a[4])*(b[4]-b[5]); c[9]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 

	s-=d[0]; t=co+s +(dchunk)(a[9]-a[1])*(b[1]-b[9])+(dchunk)(a[8]-a[2])*(b[2]-b[8])+(dchunk)(a[7]-a[3])*(b[3]-b[7])+(dchunk)(a[6]-a[4])*(b[4]-b[6]); c[10]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s-=d[1]; t=co+s +(dchunk)(a[9]-a[2])*(b[2]-b[9])+(dchunk)(a[8]-a[3])*(b[3]-b[8])+(dchunk)(a[7]-a[4])*(b[4]-b[7])+(dchunk)(a[6]-a[5])*(b[5]-b[6]); c[11]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s-=d[2]; t=co+s +(dchunk)(a[9]-a[3])*(b[3]-b[9])+(dchunk)(a[8]-a[4])*(b[4]-b[8])+(dchunk)(a[7]-a[5])*(b[5]-b[7]); c[12]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s-=d[3]; t=co+s +(dchunk)(a[9]-a[4])*(b[4]-b[9])+(dchunk)(a[8]-a[5])*(b[5]-b[8])+(dchunk)(a[7]-a[6])*(b[6]-b[7]); c[13]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s-=d[4]; t=co+s +(dchunk)(a[9]-a[5])*(b[5]-b[9])+(dchunk)(a[8]-a[6])*(b[6]-b[8]); c[14]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s-=d[5]; t=co+s +(dchunk)(a[9]-a[6])*(b[6]-b[9])+(dchunk)(a[8]-a[7])*(b[7]-b[8]); c[15]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s-=d[6]; t=co+s +(dchunk)(a[9]-a[7])*(b[7]-b[9]); c[16]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s-=d[7]; t=co+s +(dchunk)(a[9]-a[8])*(b[8]-b[9]); c[17]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	s-=d[8]; t=co+s ; c[18]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	c[19]=(chunk)co;


free(d);
}

/* Set c=a*a */
/* SU= 80 */
void B256_28::BIG_sqr(DBIG c, BIG a)
{
    dchunk t, co;
	t=(dchunk)a[0]*a[0]; c[0]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28;
	t= +(dchunk)a[1]*a[0]; t+=t; t+=co; c[1]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[2]*a[0]; t+=t; t+=co; t+=(dchunk)a[1]*a[1]; c[2]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[3]*a[0]+(dchunk)a[2]*a[1]; t+=t; t+=co; c[3]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[4]*a[0]+(dchunk)a[3]*a[1]; t+=t; t+=co; t+=(dchunk)a[2]*a[2]; c[4]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[5]*a[0]+(dchunk)a[4]*a[1]+(dchunk)a[3]*a[2]; t+=t; t+=co; c[5]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[6]*a[0]+(dchunk)a[5]*a[1]+(dchunk)a[4]*a[2]; t+=t; t+=co; t+=(dchunk)a[3]*a[3]; c[6]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[7]*a[0]+(dchunk)a[6]*a[1]+(dchunk)a[5]*a[2]+(dchunk)a[4]*a[3]; t+=t; t+=co; c[7]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[8]*a[0]+(dchunk)a[7]*a[1]+(dchunk)a[6]*a[2]+(dchunk)a[5]*a[3]; t+=t; t+=co; t+=(dchunk)a[4]*a[4]; c[8]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[9]*a[0]+(dchunk)a[8]*a[1]+(dchunk)a[7]*a[2]+(dchunk)a[6]*a[3]+(dchunk)a[5]*a[4]; t+=t; t+=co; c[9]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 

	t= +(dchunk)a[9]*a[1]+(dchunk)a[8]*a[2]+(dchunk)a[7]*a[3]+(dchunk)a[6]*a[4]; t+=t; t+=co; t+=(dchunk)a[5]*a[5]; c[10]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[9]*a[2]+(dchunk)a[8]*a[3]+(dchunk)a[7]*a[4]+(dchunk)a[6]*a[5]; t+=t; t+=co; c[11]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[9]*a[3]+(dchunk)a[8]*a[4]+(dchunk)a[7]*a[5]; t+=t; t+=co; t+=(dchunk)a[6]*a[6]; c[12]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[9]*a[4]+(dchunk)a[8]*a[5]+(dchunk)a[7]*a[6]; t+=t; t+=co; c[13]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[9]*a[5]+(dchunk)a[8]*a[6]; t+=t; t+=co; t+=(dchunk)a[7]*a[7]; c[14]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[9]*a[6]+(dchunk)a[8]*a[7]; t+=t; t+=co; c[15]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[9]*a[7]; t+=t; t+=co; t+=(dchunk)a[8]*a[8]; c[16]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t= +(dchunk)a[9]*a[8]; t+=t; t+=co; c[17]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
	t=co; t+=(dchunk)a[9]*a[9]; c[18]=(chunk)t&BMASK_B256_28; co=t>>BASEBITS_B256_28; 
 	c[19]=(chunk)co;
}

/* Montgomery reduction */
void B256_28::BIG_monty(BIG a, BIG md, chunk MC, DBIG d)
{
    dchunk c,t,s;
    dchunk *dd;
    dd = (dchunk*) malloc(NLEN_B256_28 * sizeof(dchunk));
    chunk *v;
    v = (chunk*) malloc(NLEN_B256_28 * sizeof(chunk));


    	t=d[0]; v[0]=((chunk)t*MC)&BMASK_B256_28; t+=(dchunk)v[0]*md[0];  s=0; c=(t>>BASEBITS_B256_28);

	t=d[1]+c+s+(dchunk)v[0]*md[1]; v[1]=((chunk)t*MC)&BMASK_B256_28; t+=(dchunk)v[1]*md[0];  dd[1]=(dchunk)v[1]*md[1]; s+=dd[1]; c=(t>>BASEBITS_B256_28); 
	t=d[2]+c+s+(dchunk)v[0]*md[2]; v[2]=((chunk)t*MC)&BMASK_B256_28; t+=(dchunk)v[2]*md[0];  dd[2]=(dchunk)v[2]*md[2]; s+=dd[2]; c=(t>>BASEBITS_B256_28); 
	t=d[3]+c+s+(dchunk)v[0]*md[3]+(dchunk)(v[1]-v[2])*(md[2]-md[1]); v[3]=((chunk)t*MC)&BMASK_B256_28; t+=(dchunk)v[3]*md[0];  dd[3]=(dchunk)v[3]*md[3]; s+=dd[3]; c=(t>>BASEBITS_B256_28); 
	t=d[4]+c+s+(dchunk)v[0]*md[4]+(dchunk)(v[1]-v[3])*(md[3]-md[1]); v[4]=((chunk)t*MC)&BMASK_B256_28; t+=(dchunk)v[4]*md[0];  dd[4]=(dchunk)v[4]*md[4]; s+=dd[4]; c=(t>>BASEBITS_B256_28); 
	t=d[5]+c+s+(dchunk)v[0]*md[5]+(dchunk)(v[1]-v[4])*(md[4]-md[1])+(dchunk)(v[2]-v[3])*(md[3]-md[2]); v[5]=((chunk)t*MC)&BMASK_B256_28; t+=(dchunk)v[5]*md[0];  dd[5]=(dchunk)v[5]*md[5]; s+=dd[5]; c=(t>>BASEBITS_B256_28); 
	t=d[6]+c+s+(dchunk)v[0]*md[6]+(dchunk)(v[1]-v[5])*(md[5]-md[1])+(dchunk)(v[2]-v[4])*(md[4]-md[2]); v[6]=((chunk)t*MC)&BMASK_B256_28; t+=(dchunk)v[6]*md[0];  dd[6]=(dchunk)v[6]*md[6]; s+=dd[6]; c=(t>>BASEBITS_B256_28); 
	t=d[7]+c+s+(dchunk)v[0]*md[7]+(dchunk)(v[1]-v[6])*(md[6]-md[1])+(dchunk)(v[2]-v[5])*(md[5]-md[2])+(dchunk)(v[3]-v[4])*(md[4]-md[3]); v[7]=((chunk)t*MC)&BMASK_B256_28; t+=(dchunk)v[7]*md[0];  dd[7]=(dchunk)v[7]*md[7]; s+=dd[7]; c=(t>>BASEBITS_B256_28); 
	t=d[8]+c+s+(dchunk)v[0]*md[8]+(dchunk)(v[1]-v[7])*(md[7]-md[1])+(dchunk)(v[2]-v[6])*(md[6]-md[2])+(dchunk)(v[3]-v[5])*(md[5]-md[3]); v[8]=((chunk)t*MC)&BMASK_B256_28; t+=(dchunk)v[8]*md[0];  dd[8]=(dchunk)v[8]*md[8]; s+=dd[8]; c=(t>>BASEBITS_B256_28); 
	t=d[9]+c+s+(dchunk)v[0]*md[9]+(dchunk)(v[1]-v[8])*(md[8]-md[1])+(dchunk)(v[2]-v[7])*(md[7]-md[2])+(dchunk)(v[3]-v[6])*(md[6]-md[3])+(dchunk)(v[4]-v[5])*(md[5]-md[4]); v[9]=((chunk)t*MC)&BMASK_B256_28; t+=(dchunk)v[9]*md[0];  dd[9]=(dchunk)v[9]*md[9]; s+=dd[9]; c=(t>>BASEBITS_B256_28); 

	t=d[10]+c+s+(dchunk)(v[1]-v[9])*(md[9]-md[1])+(dchunk)(v[2]-v[8])*(md[8]-md[2])+(dchunk)(v[3]-v[7])*(md[7]-md[3])+(dchunk)(v[4]-v[6])*(md[6]-md[4]); a[0]=(chunk)t&BMASK_B256_28;  s-=dd[1]; c=(t>>BASEBITS_B256_28); 
	t=d[11]+c+s+(dchunk)(v[2]-v[9])*(md[9]-md[2])+(dchunk)(v[3]-v[8])*(md[8]-md[3])+(dchunk)(v[4]-v[7])*(md[7]-md[4])+(dchunk)(v[5]-v[6])*(md[6]-md[5]); a[1]=(chunk)t&BMASK_B256_28;  s-=dd[2]; c=(t>>BASEBITS_B256_28); 
	t=d[12]+c+s+(dchunk)(v[3]-v[9])*(md[9]-md[3])+(dchunk)(v[4]-v[8])*(md[8]-md[4])+(dchunk)(v[5]-v[7])*(md[7]-md[5]); a[2]=(chunk)t&BMASK_B256_28;  s-=dd[3]; c=(t>>BASEBITS_B256_28); 
	t=d[13]+c+s+(dchunk)(v[4]-v[9])*(md[9]-md[4])+(dchunk)(v[5]-v[8])*(md[8]-md[5])+(dchunk)(v[6]-v[7])*(md[7]-md[6]); a[3]=(chunk)t&BMASK_B256_28;  s-=dd[4]; c=(t>>BASEBITS_B256_28); 
	t=d[14]+c+s+(dchunk)(v[5]-v[9])*(md[9]-md[5])+(dchunk)(v[6]-v[8])*(md[8]-md[6]); a[4]=(chunk)t&BMASK_B256_28;  s-=dd[5]; c=(t>>BASEBITS_B256_28); 
	t=d[15]+c+s+(dchunk)(v[6]-v[9])*(md[9]-md[6])+(dchunk)(v[7]-v[8])*(md[8]-md[7]); a[5]=(chunk)t&BMASK_B256_28;  s-=dd[6]; c=(t>>BASEBITS_B256_28); 
	t=d[16]+c+s+(dchunk)(v[7]-v[9])*(md[9]-md[7]); a[6]=(chunk)t&BMASK_B256_28;  s-=dd[7]; c=(t>>BASEBITS_B256_28); 
	t=d[17]+c+s+(dchunk)(v[8]-v[9])*(md[9]-md[8]); a[7]=(chunk)t&BMASK_B256_28;  s-=dd[8]; c=(t>>BASEBITS_B256_28); 
	t=d[18]+c+s; a[8]=(chunk)t&BMASK_B256_28;  s-=dd[9]; c=(t>>BASEBITS_B256_28); 
	a[9]=d[19]+((chunk)c&BMASK_B256_28);

free(dd);
free(v);
}

/* General shift left of a by n bits */
/* a MUST be normalised */
/* SU= 32 */
void B256_28::BIG_shl(BIG a, int k)
{
    int i;
    int n = k % BASEBITS_B256_28;
    int m = k / BASEBITS_B256_28;

    a[NLEN_B256_28 - 1] = ((a[NLEN_B256_28 - 1 - m] << n));
    if (NLEN_B256_28 >= m + 2) a[NLEN_B256_28 - 1] |= (a[NLEN_B256_28 - m - 2] >> (BASEBITS_B256_28 - n));

    for (i = NLEN_B256_28 - 2; i > m; i--)
        a[i] = ((a[i - m] << n)&BMASK_B256_28) | (a[i - m - 1] >> (BASEBITS_B256_28 - n));
    a[m] = (a[0] << n)&BMASK_B256_28;
    for (i = 0; i < m; i++) a[i] = 0;

}

/* Fast shift left of a by n bits, where n less than a word, Return excess (but store it as well) */
/* a MUST be normalised */
/* SU= 16 */
void B256_28::BIG_fshl(BIG a, int n)
{
    int i;

    a[NLEN_B256_28 - 1] = ((a[NLEN_B256_28 - 1] << n)) | (a[NLEN_B256_28 - 2] >> (BASEBITS_B256_28 - n)); /* top word not masked */
    for (i = NLEN_B256_28 - 2; i > 0; i--)
        a[i] = ((a[i] << n)&BMASK_B256_28) | (a[i - 1] >> (BASEBITS_B256_28 - n));
    a[0] = (a[0] << n)&BMASK_B256_28;
}

/* double length left shift of a by k bits - k can be > BASEBITS_B256_28 , a MUST be normalised */
/* SU= 32 */
void B256_28::BIG_dshl(DBIG a, int k)
{
    int i;
    int n = k % BASEBITS_B256_28;
    int m = k / BASEBITS_B256_28;

    a[DNLEN_B256_28 - 1] = ((a[DNLEN_B256_28 - 1 - m] << n)) | (a[DNLEN_B256_28 - m - 2] >> (BASEBITS_B256_28 - n));

    for (i = DNLEN_B256_28 - 2; i > m; i--)
        a[i] = ((a[i - m] << n)&BMASK_B256_28) | (a[i - m - 1] >> (BASEBITS_B256_28 - n));
    a[m] = (a[0] << n)&BMASK_B256_28;
    for (i = 0; i < m; i++) a[i] = 0;

}

/* General shift right of a by k bits */
/* a MUST be normalised */
/* SU= 32 */
void B256_28::BIG_shr(BIG a, int k)
{
    int i;
    int n = k % BASEBITS_B256_28;
    int m = k / BASEBITS_B256_28;
    for (i = 0; i < NLEN_B256_28 - m - 1; i++)
        a[i] = (a[m + i] >> n) | ((a[m + i + 1] << (BASEBITS_B256_28 - n))&BMASK_B256_28);
    if (NLEN_B256_28 > m)  a[NLEN_B256_28 - m - 1] = a[NLEN_B256_28 - 1] >> n;
    for (i = NLEN_B256_28 - m; i < NLEN_B256_28; i++) a[i] = 0;

}

/* Fast combined shift, subtract and norm. Return sign of result */
int B256_28::BIG_ssn(BIG r, BIG a, BIG m)
{
    int i, n = NLEN_B256_28 - 1;
    chunk carry;
    m[0] = (m[0] >> 1) | ((m[1] << (BASEBITS_B256_28 - 1))&BMASK_B256_28);
    r[0] = a[0] - m[0];
    carry = r[0] >> BASEBITS_B256_28;
    r[0] &= BMASK_B256_28;

    for (i = 1; i < n; i++)
    {
        m[i] = (m[i] >> 1) | ((m[i + 1] << (BASEBITS_B256_28 - 1))&BMASK_B256_28);
        r[i] = a[i] - m[i] + carry;
        carry = r[i] >> BASEBITS_B256_28;
        r[i] &= BMASK_B256_28;
    }

    m[n] >>= 1;
    r[n] = a[n] - m[n] + carry;

    return ((r[n] >> (CHUNK - 1)) & 1);
}

/* Faster shift right of a by k bits. Return shifted out part */
/* a MUST be normalised */
/* SU= 16 */
int B256_28::BIG_fshr(BIG a, int k)
{
    int i;
    chunk r = a[0] & (((chunk)1 << k) - 1); /* shifted out part */
    for (i = 0; i < NLEN_B256_28 - 1; i++)
        a[i] = (a[i] >> k) | ((a[i + 1] << (BASEBITS_B256_28 - k))&BMASK_B256_28);
    a[NLEN_B256_28 - 1] = a[NLEN_B256_28 - 1] >> k;
    return (int)r;
}

/* double length right shift of a by k bits - can be > BASEBITS_B256_28 */
/* SU= 32 */
void B256_28::BIG_dshr(DBIG a, int k)
{
    int i;
    int n = k % BASEBITS_B256_28;
    int m = k / BASEBITS_B256_28;
    for (i = 0; i < DNLEN_B256_28 - m - 1; i++)
        a[i] = (a[m + i] >> n) | ((a[m + i + 1] << (BASEBITS_B256_28 - n))&BMASK_B256_28);
    a[DNLEN_B256_28 - m - 1] = a[DNLEN_B256_28 - 1] >> n;
    for (i = DNLEN_B256_28 - m; i < DNLEN_B256_28; i++ ) a[i] = 0;
}

/* Split DBIG d into two BIGs t|b. Split happens at n bits, where n falls into NLEN_B256_28 word */
/* d MUST be normalised */
/* SU= 24 */
chunk B256_28::BIG_split(BIG t, BIG b, DBIG d, int n)
{
    int i;
    chunk nw, carry = 0;
    int m = n % BASEBITS_B256_28;

    if (m == 0)
    {
        for (i = 0; i < NLEN_B256_28; i++) b[i] = d[i];
        if (t != b)
        {
            for (i = NLEN_B256_28; i < 2 * NLEN_B256_28; i++) t[i - NLEN_B256_28] = d[i];
            carry = t[NLEN_B256_28 - 1] >> BASEBITS_B256_28;
            t[NLEN_B256_28 - 1] = t[NLEN_B256_28 - 1] & BMASK_B256_28; /* top word normalized */
        }
        return carry;
    }

    for (i = 0; i < NLEN_B256_28 - 1; i++) b[i] = d[i];

    b[NLEN_B256_28 - 1] = d[NLEN_B256_28 - 1] & (((chunk)1 << m) - 1);

    if (t != b)
    {
        carry = (d[DNLEN_B256_28 - 1] << (BASEBITS_B256_28 - m));
        for (i = DNLEN_B256_28 - 2; i >= NLEN_B256_28 - 1; i--)
        {
            nw = (d[i] >> m) | carry;
            carry = (d[i] << (BASEBITS_B256_28 - m))&BMASK_B256_28;
            t[i - NLEN_B256_28 + 1] = nw;
        }
    }

    return carry;
}

/* you gotta keep the sign of carry! Look - no branching! */
/* Note that sign bit is needed to disambiguate between +ve and -ve values */
/* normalise BIG - force all digits < 2^BASEBITS_B256_28 */
void B256_28::BIG_norm(BIG a)
{
    int i;
    chunk d, carry;

    carry=a[0]>>BASEBITS_B256_28;
    a[0]&=BMASK_B256_28;

    for (i = 1; i < NLEN_B256_28 - 1; i++)
    {
        d = a[i] + carry;
        a[i] = d & BMASK_B256_28;
        carry = d >> BASEBITS_B256_28;
    }
    a[NLEN_B256_28 - 1] = (a[NLEN_B256_28 - 1] + carry);
}

void B256_28::BIG_dnorm(DBIG a)
{
    int i;
    chunk d, carry;

    carry=a[0]>>BASEBITS_B256_28;
    a[0]&=BMASK_B256_28;

    for (i = 1; i < DNLEN_B256_28 - 1; i++)
    {
        d = a[i] + carry;
        a[i] = d & BMASK_B256_28;
        carry = d >> BASEBITS_B256_28;
    }
    a[DNLEN_B256_28 - 1] = (a[DNLEN_B256_28 - 1] + carry);

}

/* Compare a and b. Return 1 for a>b, -1 for a<b, 0 for a==b */
/* a and b MUST be normalised before call */
/* sodium constant time implementation */

int B256_28::BIG_comp(BIG a, BIG b)
{
    int i;
    chunk gt=0; chunk eq=1;
    for (i = NLEN_B256_28-1; i>=0; i--)
    {
        gt |= ((b[i]-a[i]) >> BASEBITS_B256_28) & eq;
        eq &= ((b[i]^a[i])-1) >> BASEBITS_B256_28;
    }
    return (int)(gt+gt+eq-1);
}

int B256_28::BIG_dcomp(DBIG a, DBIG b)
{
    int i;
    chunk gt=0; chunk eq=1;
    for (i = DNLEN_B256_28-1; i>=0; i--)
    {
        gt |= ((b[i]-a[i]) >> BASEBITS_B256_28) & eq;
        eq &= ((b[i]^a[i])-1) >> BASEBITS_B256_28;
    }
    return (int)(gt+gt+eq-1);
}

/* return number of bits in a */
/* SU= 8 */
int B256_28::BIG_nbits(BIG a)
{
    int bts, k = NLEN_B256_28 - 1;
    BIG t;
    chunk c;
    BIG_copy(t, a);
    BIG_norm(t);
    while (k >= 0 && t[k] == 0) k--;
    if (k < 0) return 0;
    bts = BASEBITS_B256_28 * k;
    c = t[k];
    while (c != 0)
    {
        c /= 2;
        bts++;
    }
    return bts;
}

/* SU= 8, Calculate number of bits in a DBIG - output normalised */
int B256_28::BIG_dnbits(DBIG a)
{
    int bts, k = DNLEN_B256_28 - 1;
    DBIG t;
    chunk c;
    BIG_dcopy(t, a);
    BIG_dnorm(t);
    while (k >= 0 && t[k] == 0) k--;
    if (k < 0) return 0;
    bts = BASEBITS_B256_28 * k;
    c = t[k];
    while (c != 0)
    {
        c /= 2;
        bts++;
    }
    return bts;
}

// Set b=b mod m in constant time (if bd is known at compile time)
// bd is Max number of bits in b - Actual number of bits in m
void B256_28::BIG_ctmod(BIG b, BIG m, int bd)
{
    int k=bd;
    BIG r,c;
    BIG_copy(c,m);
    BIG_norm(b);

    BIG_shl(c,k);
    while (k>=0)
    {
        BIG_sub(r, b, c);
        BIG_norm(r);
        if(1 - ((r[NLEN_B256_28 - 1] >> (CHUNK - 1)) & 1)) BIG_copy(b, r);
        BIG_fshr(c, 1);
        k--;
    }
}

/* Set b=b mod m */
/* SU= 16 */
void B256_28::BIG_mod(BIG b, BIG m)
{
    int k=BIG_nbits(b)-BIG_nbits(m);
    if (k<0) k=0;
    BIG_ctmod(b,m,k);
}


// Set a=b mod m in constant time (if bd is known at compile time)
// bd is Max number of bits in b - Actual number of bits in m
void B256_28::BIG_ctdmod(BIG a, DBIG b, BIG m, int bd)
{
    int k=bd;
    DBIG c,r;
    BIG_dscopy(c,m);
    BIG_dnorm(b);

    BIG_dshl(c,k);
    while (k>=0)
    {
        BIG_dsub(r, b, c);
        BIG_dnorm(r);
        if(1 - ((r[DNLEN_B256_28 - 1] >> (CHUNK - 1)) & 1)) BIG_dcopy(b, r);
        BIG_dshr(c, 1);
        k--;
    }
    BIG_sdcopy(a,b);
}

/* Set a=b mod c, b is destroyed. Slow but rarely used. */
/* SU= 96 */
void B256_28::BIG_dmod(BIG a, DBIG b, BIG m)
{
    int k=BIG_dnbits(b)-BIG_nbits(m);
    if (k<0) k=0;
    BIG_ctdmod(a,b,m,k);
}

/* return LSB of a */
int B256_28::BIG_parity(BIG a)
{
    return a[0] % 2;
}

/* return n-th bit of a */
/* SU= 16 */
int B256_28::BIG_bit(BIG a, int n)
{
    return (int)((a[n / BASEBITS_B256_28] & ((chunk)1 << (n % BASEBITS_B256_28))) >> (n%BASEBITS_B256_28));
}

/* return last n bits of a, where n is small < BASEBITS_B256_28 */
/* SU= 16 */
int B256_28::BIG_lastbits(BIG a, int n)
{
    int msk = (1 << n) - 1;
    BIG_norm(a);
    return ((int)a[0])&msk;
}

/* get 8*MODBYTES_B256_28 size random number */
void B256_28::BIG_random(BIG m, csprng *rng)
{
    int i, b, j = 0, r = 0;
    int len = 8 * MODBYTES_B256_28;

    BIG_zero(m);
    /* generate random BIG */
    for (i = 0; i < len; i++)
    {
        if (j == 0) r = RAND_byte(rng);
        else r >>= 1;
        b = r & 1;
        BIG_shl(m, 1);
        m[0] += b;
        j++;
        j &= 7;
    }

}

/* get random BIG from rng, modulo q. Done one bit at a time, so its portable */

extern int NFILLPOOL;

void B256_28::BIG_randomnum(BIG m, BIG q, csprng *rng)
{
    int i, b, j = 0, r = 0;
    int n=2 * BIG_nbits(q);
    DBIG d;
    BIG_dzero(d);

    /* generate random DBIG */
    for (i = 0; i < n; i++)
    {
        if (j == 0) r = RAND_byte(rng);
        else r >>= 1;
        b = r & 1;
        BIG_dshl(d, 1);
        d[0] += b;
        j++;
        j &= 7;
    }
    /* reduce modulo a BIG. Removes bias */
    BIG_dmod(m, d, q);

}

/* create randum BIG less than r and less than trunc bits */
void B256_28::BIG_randtrunc(BIG s, BIG r, int trunc, csprng *rng)
{
    BIG_randomnum(s, r, rng);
    if (BIG_nbits(r) > trunc)
        BIG_mod2m(s, trunc);
}

/* Set r=a*b mod m */
/* SU= 96 */
void B256_28::BIG_modmul(BIG r, BIG a1, BIG b1, BIG m)
{
    DBIG d;
    BIG a, b;
    BIG_copy(a, a1);
    BIG_copy(b, b1);
    BIG_mod(a, m);
    BIG_mod(b, m);

    BIG_mul(d, a, b);
    BIG_ctdmod(r, d, m,BIG_nbits(m));
}

/* Set r=-a mod m */
/* SU= 16 */
void B256_28::BIG_modneg(BIG r, BIG a1, BIG m)
{
    BIG a;
    BIG_copy(a, a1);
    BIG_mod(a, m);
    BIG_sub(r, m, a); BIG_norm(r);
}

/* Set r=a+b mod m */
void B256_28::BIG_modadd(BIG r, BIG a1, BIG b1, BIG m)
{
    BIG a, b;
    BIG_copy(a, a1);
    BIG_copy(b, b1);
    BIG_mod(a, m);
    BIG_mod(b, m);
    BIG_add(r,a,b); BIG_norm(r);
    BIG_ctmod(r,m,1);
}

// Set r=1/a mod p. Binary method 
// NOTE: This function is NOT side-channel safe
// If a is a secret then ALWAYS calculate 1/a = m*(1/am) mod p 
// where m is a random masking value
void B256_28::BIG_invmodp(BIG r, BIG a, BIG p)
{
    BIG u, v, x1, x2, t, one;

    BIG_copy(u, a);
    BIG_copy(v, p);
    BIG_one(one);
    BIG_copy(x1, one);
    BIG_zero(x2);

    while (BIG_comp(u, one) != 0 && BIG_comp(v, one) != 0)
    {
        while (BIG_parity(u) == 0)
        {
            BIG_fshr(u, 1);
            BIG_add(t,x1,p);
            if(BIG_parity(x1)) BIG_copy(x1,t);
            BIG_norm(x1);
            BIG_fshr(x1,1);
        }
        while (BIG_parity(v) == 0)
        {
            BIG_fshr(v, 1);
            BIG_add(t,x2,p);
            if(BIG_parity(x2)) BIG_copy(x2,t);
            BIG_norm(x2);
            BIG_fshr(x2,1);
        } 
        if (BIG_comp(u, v) >= 0) 
        {
            BIG_sub(u, u, v);
            BIG_norm(u);
            BIG_add(t,x1,p);
            if((BIG_comp(x1,x2)>>1)&1) BIG_copy(x1,t); // move if x1<x2 
            BIG_sub(x1,x1,x2);
            BIG_norm(x1);
        }
        else
        {
            BIG_sub(v, v, u);
            BIG_norm(v);
            BIG_add(t,x2,p);
            if((BIG_comp(x2,x1)>>1)&1) BIG_copy(x2,t); // move if x2<x1 
            BIG_sub(x2,x2,x1);
            BIG_norm(x2);
        }
    }
    if(BIG_comp(u,one)&1) BIG_copy(r,x2);
    else BIG_copy(r,x1);
}


/* set x = x mod 2^m */
void B256_28::BIG_mod2m(BIG x, int m)
{
    int i, wd, bt;
    chunk msk;
    BIG_norm(x);

    wd = m / BASEBITS_B256_28;
    bt = m % BASEBITS_B256_28;
    msk = ((chunk)1 << bt) - 1;
    x[wd] &= msk;
    for (i = wd + 1; i < NLEN_B256_28; i++) x[i] = 0;
}

