#include "cca2_sign.h"
#include "drone_const.h"

int main()
{
  BIG_rcopy(p, CURVE_Order);
  ECP_generator(&P);
  ECP2_generator(&P_hat);

  for (int i = 0; i < NLEN_B256_28; i++)
  {
    m1.x.g[i] = m1_h[0][i];
    m1.y.g[i] = m1_h[1][i];
    m1.z.g[i] = m1_h[2][i];

    m2.x.g[i] = m2_h[0][i];
    m2.y.g[i] = m2_h[1][i];
    m2.z.g[i] = m2_h[2][i];

    Z.x.g[i] = Z_h[0][i];
    Z.y.g[i] = Z_h[1][i];
    Z.z.g[i] = Z_h[2][i];

    Y.x.g[i] = Y_h[0][i];
    Y.y.g[i] = Y_h[1][i];
    Y.z.g[i] = Y_h[2][i];

    Y_hat.x.a.g[i] = Y_hat_ha[0][i];
    Y_hat.y.a.g[i] = Y_hat_ha[1][i];
    Y_hat.z.a.g[i] = Y_hat_ha[2][i];

    Y_hat.x.b.g[i] = Y_hat_hb[0][i];
    Y_hat.y.b.g[i] = Y_hat_hb[1][i];
    Y_hat.z.b.g[i] = Y_hat_hb[2][i];
  }

  m1.x.XES = m1_xes[0];
  m1.y.XES = m1_xes[1];
  m1.z.XES = m1_xes[2];

  m2.x.XES = m2_xes[0];
  m2.y.XES = m2_xes[1];
  m2.z.XES = m2_xes[2];

  Y.x.XES = Y_xes[0];
  Y.y.XES = Y_xes[1];
  Y.z.XES = Y_xes[2];

  Z.x.XES = Z_xes[0];
  Z.y.XES = Z_xes[1];
  Z.z.XES = Z_xes[2];

  Y_hat.x.a.XES = xesa[0];
  Y_hat.y.a.XES = xesa[1];
  Y_hat.z.a.XES = xesa[2];

  Y_hat.x.b.XES = xesb[0];
  Y_hat.y.b.XES = xesb[1];
  Y_hat.z.b.XES = xesb[2];
  memset(m, 0, m_size);

  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len = 100;
  CREATE_CSPRNG(&RNG,  &RAW);

  sign();
  return EXIT_SUCCESS;
}

void sign()
{
  HASH256_init(&sh256);
  
  BIG rho;
  BIG_randtrunc(rho, p, 2 * CURVE_SECURITY_BN254, &RNG);// select u in {1, ..., p-1}

  // m1
  ECP_copy(&ecp, &m1);
  ECP_mul(&ecp, rho);
  hash_ECP(&sh256, ecp);
  ECP_toChar(bc, &ecp);

  // m2
  ECP_copy(&ecp, &m2);
  ECP_mul(&ecp, rho);
  hash_ECP(&sh256, ecp);
  ECP_toChar(bc + ecp_size, &ecp);

  BIG y, inv_y;
  BIG_randtrunc(y, p, 2 * CURVE_SECURITY_BN254, &RNG);
  BIG_invmodp(inv_y, y, p);
  
  // Z
  ECP_copy(&ecp, &Z);
  BIG_modmul(big, y, rho, p); // rho*y
  ECP_mul(&ecp, big);
  hash_ECP(&sh256, ecp);
  ECP_toChar(bc + 2 * ecp_size, &ecp);

  // Y
  ECP_copy(&ecp, &Y);
  ECP_mul(&ecp, inv_y);
  hash_ECP(&sh256, ecp);
  ECP_toChar(bc + 3 *  ecp_size, &ecp);

  // Y_hat 
  ECP2 ecp2_temp;
  ECP2_copy(&ecp2_temp, &Y_hat);
  ECP2_mul(&ecp2_temp, inv_y);
  hash_ECP2(&sh256, ecp2_temp);
  ECP2_toChar(bc + 4 * ecp_size, &ecp2_temp);

  // C1
  BIG u;
  BIG_randtrunc(u, p, 2 * CURVE_SECURITY_BN254, &RNG);// select u in {1, ..., p-1}
  ECP2_copy(&ecp2, &ecp2_temp);
  ECP2_mul(&ecp2, u); // uY_hat
  ECP2_toChar(bc + 4 * ecp_size + ecp2_size, &ecp2);

  // C2
  ECP2_copy(&ecp2, &P_hat);
  BIG_modadd(big, rho, u, p); // rho + u
  ECP2_mul(&ecp2, big); // (rho + u)P_hat
  ECP2_toChar(bc + 4 * ecp_size + 2 * ecp2_size, &ecp2);

  // Calculate N
  BIG v;
  BIG_randtrunc(v, p, 2 * CURVE_SECURITY_BN254, &RNG);// select v in {1, ..., p-1}
  ECP_copy(&ecp, &P);
  ECP_mul(&ecp, v); // N =  vP
  hash_ECP(&sh256, ecp);
  
  // M1_hat
  BIG n;
  BIG_randtrunc(n, p, 2 * CURVE_SECURITY_BN254, &RNG);// select u in {1, ..., p-1}
  ECP2_copy(&ecp2, &ecp2_temp);
  ECP2_mul(&ecp2, n); // M1 = nY_hat
  hash_ECP2(&sh256, ecp2);

  // M2_hat
  ECP2_copy(&ecp2, &P_hat);
  BIG_modadd(big, v, n, p); // v + n mod p
  ECP2_mul(&ecp2, big); //M2 = (v+n)P_hat
  hash_ECP2(&sh256, ecp2);

  // m
  char c_ch[32];
  for (int i = 0; i < m_size; i++) HASH256_process(&sh256, m[i]);
  HASH256_hash(&sh256, c_ch);
  for (int i = 0; i < m_size; i++) bc[4 * ecp_size + 3 * ecp2_size + i] = m[i];

  // digest to big num
  BIG c;
  BIG_fromBytesLen(c, c_ch, 32);
  BIG_toBytes(bc + 4 * ecp_size + 3 * ecp2_size + m_size, c);

  // z1
  BIG_modmul(big, c, rho, p); // c * rho
  BIG_modadd(big, big, v, p); // v + c * rho
  BIG_toBytes(bc + 4 * ecp_size + 3 * ecp2_size + m_size + big_size, big);

  // z2
  BIG_modmul(big, c, u, p); // c * u
  BIG_modadd(big, big, n, p); // n + c * u
  BIG_toBytes(bc + 4 * ecp_size + 3 * ecp2_size + m_size + 2 * big_size, big);

  #ifdef BROADCAST
  // send using drones broadcast mechanism
  #else
    FILE *fp = fopen("drone/broadcast.txt", "wb");
    fwrite(bc, sizeof(char), 4 * ecp_size + 3 * ecp2_size + m_size + 3 * big_size, fp);
    fclose(fp);
  #endif
}