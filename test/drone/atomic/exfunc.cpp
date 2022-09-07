#include "exfunc.h"

void ECP2_toChar(char W[], ECP2 *P)
{
  int i;
  BIG t;
  FP2 qx, qy;

  ECP2_affine(P);  
  BIG_copy(qx.a.g, P->x.a.g);
  qx.a.XES = P->x.a.XES;
    
  BIG_copy(qx.b.g, P->x.b.g);
  qx.b.XES = P->x.b.XES;

  BIG_copy(qy.a.g, P->y.a.g);
  qy.a.XES = P->y.a.XES;
    
  BIG_copy(qy.b.g, P->y.b.g);
  qy.b.XES = P->y.b.XES;

  FP_redc(t, &(qx.b));
  BIG_norm(t);
  for (i = MODBYTES_B256_28 - 1; i >= 0; i--)
  {
    W[i] = t[0] & 0xff;
    BIG_fshr(t, 8);
  }

  FP_redc(t, &(qx.a));
  BIG_norm(t);
  for (i = MODBYTES_B256_28 - 1; i >= 0; i--)
  {
    W[i + MODBYTES_B256_28] = t[0] & 0xff;
    BIG_fshr(t, 8);
  }

  FP_redc(t, &(qy.b));
  BIG_norm(t);
  for (i = MODBYTES_B256_28 - 1; i >= 0; i--)
  {
    W[i + 2 * MODBYTES_B256_28 + 1 ] = t[0] & 0xff;
    BIG_fshr(t, 8);
  }

  FP_redc(t, &(qy.a));
  BIG_norm(t);
  for (i = MODBYTES_B256_28 - 1; i >= 0; i--)
  {
    W[i  + MODBYTES_B256_28 + 2 * MODBYTES_B256_28 + 1 ] = t[0] & 0xff;
    BIG_fshr(t, 8);
  }
}

void ECP_toChar(char W[], ECP *P)
{
  int i;
  BIG x, y;
  ECP_affine(P);

  ECP_affine(P);
  FP_redc(y, &(P->y));
  FP_redc(x, &(P->x));

  BIG_norm(y);
  BIG_norm(x);
  for (i = MODBYTES_B256_28 - 1; i >= 0; i--)
  {
    W[i] = x[0] & 0xff;
    BIG_fshr(x, 8);
    W[MODBYTES_B256_28 + 1 + i] = y[0] & 0xff;
    BIG_fshr(y, 8);
  }
}


// void ECP2_fromChar(ECP2 *ecp2, char ch[])
// {
//   FP2 qx, qy;
//   FP2_fromBytes(&qx,&(ch[0]));
//   FP2_fromBytes(&qy,&(ch[2 * MODBYTES_B256_28 + 1]));
//   ECP2_set(ecp2, &qx, &qy);
// }

// void ECP_fromChar(ECP *ecp, char ch[])
// {
//   BIG x, y;
//   BIG_fromBytes(x, &ch[0]);
//   BIG_fromBytes(y, &ch[MODBYTES_B256_28 + 1]);
//   ECP_set(ecp, x, y);
// }
// // remove on drone
// int dec_fromHex(char hex)
// {
//   if (hex >= '0' && hex <= '9') return (int)(hex - 48);
//   else if (hex >= 'a' && hex <= 'f') return (int)(hex - 97 + 10);
//   else return (int)(hex - 66 + 10);
// }

// /* return 1 if P==Q, else 0 */
// /* SU= 312 */
// int ECP2_equals(ECP2 *P, ECP2 *Q)
// {
//     FP2 a, b;

//     FP2_mul(&a, &(P->x), &(Q->z));
//     FP2_mul(&b, &(Q->x), &(P->z));
//     if (!FP2_equals(&a, &b)) return 0;

//     FP2_mul(&a, &(P->y), &(Q->z));
//     FP2_mul(&b, &(Q->y), &(P->z));
//     if (!FP2_equals(&a, &b)) return 0;
//     return 1;
// }

// /* Test P == Q */
// /* SU=168 */
// int ECP_equals(ECP *P, ECP *Q)
// {
//     FP a, b;
//     FP_mul(&a, &(P->x), &(Q->z));
//     FP_mul(&b, &(Q->x), &(P->z));
//     if (!FP_equals(&a, &b)) return 0;

//     FP_mul(&a, &(P->y), &(Q->z));
//     FP_mul(&b, &(Q->y), &(P->z));
//     if (!FP_equals(&a, &b)) return 0;

//     return 1;
// }

// void FP2_fromBytes(FP2 *x,char *b)
// {
//     FP_fromBytes(&(x->b),b);
//     FP_fromBytes(&(x->a),&b[MODBYTES_B256_28]);
// }

// void FP_fromBytes(FP *x,char *b)
// {
//     BIG t;
//     BIG_fromBytes(t, b);
//     FP_nres(x, t);
// }


// void ECP_toFile(ECP ecp, FILE *fp)
// {
//   BIG x, y;
//   byte i;
// 	for (i = 0; i < NLEN_B256_28; i++) 
//   {
//     x[i] = 0;
//     y[i] = 0;
//   }

//   ECP_get(x, y, &ecp);
//   BIG_toFile(x, fp);
//   BIG_toFile(y, fp);
// }

// void ECP2_toFile(ECP2 ecp2, FILE *fp)
// {
//   FP2 fp2_x, fp2_y;
//   ECP2_get(&fp2_x, &fp2_y, &ecp2);

//   FP2_toFile(fp2_x, fp);
//   FP2_toFile(fp2_y, fp);
// }

// void FP2_toFile(FP2 fp2, FILE *fp)
// {
//   byte i;
//   BIG big_r, big_i;
// 	for (i = 0; i < NLEN_B256_28; i++) 
//   {
//     big_r[i] = 0;
//     big_i[i] = 0;
//   }

//   FP2_reduce(&fp2);
//   FP_redc(big_r, &fp2.a);
//   FP_redc(big_i, &fp2.b);
//   BIG_toFile(big_r, fp);
//   BIG_toFile(big_i, fp);
// }

// void BIG_fromChar(BIG *a, char ch[])
// {
// 	byte i;
// 	BIG_zero(*a);
//   for (i = 0 ; i < MODBYTES_B256_28 * 2 ; i++)
//   {
//     BIG_fshl(*a, 4);
//     *a[0] += dec_fromHex(ch[i]);
//   }
// }
// void ECP2_fromFile(FILE *fp, ECP2 *ecp2)
// {
//   BIG big_r, big_i;
//   FP2 fp2_x, fp2_y;
//   byte i;

//   char *ch;
// 	ch = (char*) malloc(sizeof(char) * MODBYTES_B256_28 * 2 + 2);
// 	memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
//   fgets(ch, 100, (FILE*)fp);
//   BIG_fromChar(&big_r, ch);

//   memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
//   fgets(ch, MODBYTES_B256_28 * 2 + 2, (FILE*)fp);
//   BIG_fromChar(&big_i, ch);
  
//   FP_nres(&(fp2_x.a), big_r);
//   FP_nres(&(fp2_x.b), big_i);

// 	for (i = 0; i < NLEN_B256_28; i++) 
//   {
//     big_r[i] = 0;
//     big_i[i] = 0;
//   }

//   memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
//   fgets(ch, 100, (FILE*)fp);
//   BIG_fromChar(&big_r, ch);

//   memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
//   fgets(ch, MODBYTES_B256_28 * 2 + 2, (FILE*)fp);
//   BIG_fromChar(&big_i, ch);

//   FP_nres(&(fp2_y.a), big_r);
//   FP_nres(&(fp2_y.b), big_i);

//   ECP2_set(ecp2, &fp2_x, &fp2_y);
//   free(ch);
// }

// void ECP_fromFile(FILE*fp, ECP *ecp)
// {
//   BIG big_x, big_y;
//   char *ch;
// 	ch = (char*) malloc(sizeof(char) * MODBYTES_B256_28 * 2 + 2);
	
//   memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
//   fgets(ch, MODBYTES_B256_28 * 2 + 2, (FILE*)fp);
//   BIG_fromChar(&big_x, ch);
  
//   memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
//   fgets(ch, MODBYTES_B256_28 * 2 + 2, (FILE*)fp);
//   BIG_fromChar(&big_y, ch);

//   ECP_set(ecp, big_x, big_y);
//   free(ch);
// }

// void BIG_toFile(BIG a, FILE *fp)
// { 
// 	BIG b;
// 	int i, len;
// 	len = BIG_nbits(a);
// 	if (len % 4 == 0) len /= 4;
// 	else
// 	{
// 		len /= 4;
// 		len++;
// 	}
// 	if (len < MODBYTES_B256_28 * 2) len = MODBYTES_B256_28 * 2;

// 	for (i = len - 1; i >= 0; i--)
// 	{
// 		BIG_copy(b, a);
// 		BIG_shr(b, i * 4);
// 		fprintf(fp, "%01x", (unsigned int) b[0] & 15);
// 	}
// 	fprintf(fp, "\n");
// }

// void BIG_fromFile(FILE *fp, BIG *a)

// {
//   char *ch;
// 	ch = (char*) malloc(sizeof(char) * MODBYTES_B256_28 * 2 + 2);
// 	memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
// 	fgets(ch, MODBYTES_B256_28 * 2 + 2, (FILE *)fp);
// 	BIG_fromChar(a, ch);
// 	free(ch);
// }

