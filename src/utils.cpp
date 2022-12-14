#include "../include/utils.h"

void hash_ECP(hash256 *ctx, ECP ecp)
{
  char ch[ecp_size];
  memset(ch, 0, ecp_size);
  ECP_toChar(ch, &ecp);
  for (int i = 0; i < ecp_size; i++) HASH256_process(ctx, ch[i]);
}

void hash_ECP2(hash256 *ctx, ECP2 ecp)
{
  char ch[ecp2_size];
  memset(ch, 0, ecp2_size);
  ECP2_toChar(ch, &ecp);
  for (int i = 0; i < ecp2_size; i++) HASH256_process(ctx, ch[i]);
}

void ECP_toChar(char W[], ECP *P)
{
  BIG x, y;
  ECP_affine(P);
  ECP_get(x, y, P);
  BIG_toBytes(&(W[0]), x);
  BIG_toBytes(&(W[MODBYTES_B256_28 + 1]), y);
}

void ECP2_toChar(char W[], ECP2 *P)
{
  FP2 qx, qy;
  ECP2_get(&qx, &qy, P);
  FP2_toBytes(&W[0],&qx);
  FP2_toBytes(&(W[2 * MODBYTES_B256_28 + 1]), &qy);
}

int dec_fromHex(char hex)
{
  if (hex >= '0' && hex <= '9') return (int)(hex - 48);
  else if (hex >= 'a' && hex <= 'f') return (int)(hex - 97 + 10);
  else return (int)(hex - 66 + 10);
}

void BIG_fromChar(BIG *a, char ch[])
{
	byte i;
	BIG_zero(*a);
  for (i = 0 ; i < MODBYTES_B256_28 * 2 ; i++)
  {
    BIG_fshl(*a, 4);
    *a[0] += dec_fromHex(ch[i]);
  }
}

void ECP2_fromChar(ECP2 *ecp2, char ch[])
{
  FP2 qx, qy;
  FP2_fromBytes(&qx,&(ch[0]));
  FP2_fromBytes(&qy,&(ch[2 * MODBYTES_B256_28 + 1]));
  ECP2_set(ecp2, &qx, &qy);
}

void ECP_fromChar(ECP *ecp, char ch[])
{
  BIG x, y;
  BIG_fromBytes(x, &ch[0]);
  BIG_fromBytes(y, &ch[MODBYTES_B256_28 + 1]);
  ECP_set(ecp, x, y);
}

void BIG_toCon(BIG x, FILE *fp)
{
  int i, len;
  BIG b;
  len = BIG_nbits(x);
  if (len % 4 == 0) len /= 4;
  else
  {
    len /= 4;
    len++;
  }
  if (len < MODBYTES_B256_28 * 2) len = MODBYTES_B256_28 * 2;

  char t[70];
  for(i=0;i<70;i++) t[i]=0;
  for (i = len - 1; i >= 0; i--)
  {
    BIG_copy(b, x);
    BIG_shr(b, i * 4);
    t[i] = (unsigned int) b[0] & 15;
  }

  for(i=0;i<10;i++)
  {
    if(i>0) fprintf(fp, " ,0x");
    for(int j =6;j>=0; j--) fprintf(fp, "%01X", t[j+ (i*7)]);
  }  
}

void BIG_toFile(BIG a, FILE *fp)
{ 
	BIG b;
	int i, len;
	len = BIG_nbits(a);
	if (len % 4 == 0) len /= 4;
	else
	{
		len /= 4;
		len++;
	}
	if (len < MODBYTES_B256_28 * 2) len = MODBYTES_B256_28 * 2;

	for (i = len - 1; i >= 0; i--)
	{
		BIG_copy(b, a);
		BIG_shr(b, i * 4);
		fprintf(fp, "%01x", (unsigned int) b[0] & 15);
	}
	fprintf(fp, "\n");
}

void FP2_toFile(FP2 fp2, FILE *fp)
{
  byte i;
  BIG big_r, big_i;
	for (i = 0; i < NLEN_B256_28; i++) 
  {
    big_r[i] = 0;
    big_i[i] = 0;
  }

  FP2_reduce(&fp2);
  FP_redc(big_r, &fp2.a);
  FP_redc(big_i, &fp2.b);
  BIG_toFile(big_r, fp);
  BIG_toFile(big_i, fp);
}

void ECP_toFile(ECP ecp, FILE *fp)
{
  BIG x, y;
  byte i;
	for (i = 0; i < NLEN_B256_28; i++) 
  {
    x[i] = 0;
    y[i] = 0;
  }

  ECP_get(x, y, &ecp);
  BIG_toFile(x, fp);
  BIG_toFile(y, fp);
}

void ECP2_toFile(ECP2 ecp2, FILE *fp)
{
  FP2 fp2_x, fp2_y;
  ECP2_get(&fp2_x, &fp2_y, &ecp2);

  FP2_toFile(fp2_x, fp);
  FP2_toFile(fp2_y, fp);
}

void OCT_toFile(octet *oct, FILE *fp)
{
  fprintf(fp, "%d\n", oct->len);
  unsigned char ch;
  for (int i = 0; i < oct->len; i++)
  {
    ch = oct->val[i];
    fprintf(fp, "%02x", ch);
  }
  fprintf(fp, "\n");
}

void BIG_fromFile(FILE *fp, BIG *a)
{
  char *ch;
	ch = (char*) malloc(sizeof(char) * MODBYTES_B256_28 * 2 + 2);
	memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
	fgets(ch, MODBYTES_B256_28 * 2 + 2, (FILE *)fp);
	BIG_fromChar(a, ch);
	free(ch);
}

void OCT_fromFile(octet *oct, FILE *fp)
{
  fscanf(fp, "%d", &oct->len);
  fscanf(fp, "\n");
  char *ch;
	ch = (char*) malloc(sizeof(char) * 2 * oct->len + 2);
	memset(ch, 0, 2 * oct->len + 2);
	fgets(ch, 2 * oct->len + 1, (FILE *)fp);
  OCT_fromHex(oct, ch);
	free(ch);
}

void ECP2_fromFile(FILE *fp, ECP2 *ecp2)
{
  BIG big_r, big_i;
  FP2 fp2_x, fp2_y;
  byte i;

  char *ch;
	ch = (char*) malloc(sizeof(char) * MODBYTES_B256_28 * 2 + 2);
	memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
  fgets(ch, 100, (FILE*)fp);
  BIG_fromChar(&big_r, ch);

  memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
  fgets(ch, MODBYTES_B256_28 * 2 + 2, (FILE*)fp);
  BIG_fromChar(&big_i, ch);
  
  FP_nres(&(fp2_x.a), big_r);
  FP_nres(&(fp2_x.b), big_i);

	for (i = 0; i < NLEN_B256_28; i++) 
  {
    big_r[i] = 0;
    big_i[i] = 0;
  }

  memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
  fgets(ch, 100, (FILE*)fp);
  BIG_fromChar(&big_r, ch);

  memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
  fgets(ch, MODBYTES_B256_28 * 2 + 2, (FILE*)fp);
  BIG_fromChar(&big_i, ch);

  FP_nres(&(fp2_y.a), big_r);
  FP_nres(&(fp2_y.b), big_i);

  ECP2_set(ecp2, &fp2_x, &fp2_y);
  free(ch);
}

void ECP_fromFile(FILE *fp, ECP *ecp)
{
  BIG big_x, big_y;
  char *ch;
	ch = (char*) malloc(sizeof(char) * MODBYTES_B256_28 * 2 + 2);
	
  memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
  fgets(ch, MODBYTES_B256_28 * 2 + 2, (FILE*)fp);
  BIG_fromChar(&big_x, ch);
  
  memset(ch, 0, MODBYTES_B256_28 * 2 + 2);
  fgets(ch, MODBYTES_B256_28 * 2 + 2, (FILE*)fp);
  BIG_fromChar(&big_y, ch);

  ECP_set(ecp, big_x, big_y);
  free(ch);
}
