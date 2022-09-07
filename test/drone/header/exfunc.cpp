#include "exfunc.h"

void ECP2_toChar(char W[], ECP2 *P)
{
  FP2 qx, qy;
  ECP2_get(&qx, &qy, P);
  FP2_toBytes(&W[0],&qx);
  FP2_toBytes(&(W[2 * MODBYTES_B256_28 + 1]), &qy);
}

void ECP_toChar(char W[], ECP *P)
{
  BIG x, y;
  ECP_affine(P);
  ECP_get(x, y, P);
  BIG_toBytes(&(W[0]), x);
  BIG_toBytes(&(W[MODBYTES_B256_28 + 1]), y);
}

int dec_fromHex(char hex)
{
  if (hex >= '0' && hex <= '9') return (int)(hex - 48);
  else if (hex >= 'a' && hex <= 'f') return (int)(hex - 97 + 10);
  else return (int)(hex - 66 + 10);
}

void FP2_fromBytes(FP2 *x,char *b)
{
    FP_fromBytes(&(x->b),b);
    FP_fromBytes(&(x->a),&b[MODBYTES_B256_28]);
}

void FP_fromBytes(FP *x,char *b)
{
    BIG t;
    BIG_fromBytes(t, b);
    FP_nres(x, t);
}