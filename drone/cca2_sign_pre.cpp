#include "cca2_sign_pre.h"
#include "drone_const.h"
#define FLIGHT_TIME 5

int main()
{
  BIG_rcopy(p, CURVE_Order); 
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

  rho_index = rand() % FLIGHT_TIME;
  u_index = rand() % FLIGHT_TIME;
  v_index = rand() % FLIGHT_TIME;
  n_index = rand() % FLIGHT_TIME;
  y_index = rand() % FLIGHT_TIME;

  // m1
  for (int i = 0; i < NLEN_B256_28; i++)
  {
    ecp.x.g[i] = m1_x[rho_index][i];
    ecp.y.g[i] = m1_y[rho_index][i];
    ecp.z.g[i] = m1_z[rho_index][i];
  }
  ecp.x.XES = m1_xes[rho_index][0];
  ecp.y.XES = m1_xes[rho_index][1];
  ecp.z.XES = m1_xes[rho_index][2];
  hash_ECP(&sh256, ecp);
  ECP_toChar(bc, &ecp);

  // m2
  for (int i = 0; i < NLEN_B256_28; i++)
  {
    ecp.x.g[i] = m2_x[rho_index][i];
    ecp.y.g[i] = m2_y[rho_index][i];
    ecp.z.g[i] = m2_z[rho_index][i];
  }
  ecp.x.XES = m2_xes[rho_index][0];
  ecp.y.XES = m2_xes[rho_index][1];
  ecp.z.XES = m2_xes[rho_index][2];
  hash_ECP(&sh256, ecp);
  ECP_toChar(bc + ecp_size, &ecp);

  // Z
  for (int i = 0; i < NLEN_B256_28; i++)
  {
    ecp.x.g[i] = Z_x[rho_index][y_index][i];
    ecp.y.g[i] = Z_y[rho_index][y_index][i];
    ecp.z.g[i] = Z_z[rho_index][y_index][i];
  }
  ecp.x.XES = Z_xes[rho_index][y_index][0];
  ecp.y.XES = Z_xes[rho_index][y_index][1];
  ecp.z.XES = Z_xes[rho_index][y_index][2];
  hash_ECP(&sh256, ecp);
  ECP_toChar(bc + 2 * ecp_size, &ecp);

  // Y
  for (int i = 0; i < NLEN_B256_28; i++)
  {
    ecp.x.g[i] = Y_x[y_index][i];
    ecp.y.g[i] = Y_y[y_index][i];
    ecp.z.g[i] = Y_z[y_index][i];
  }
  ecp.x.XES = Y_xes[y_index][0];
  ecp.y.XES = Y_xes[y_index][1];
  ecp.z.XES = Y_xes[y_index][2];
  hash_ECP(&sh256, ecp);
  ECP_toChar(bc + 3 *  ecp_size, &ecp);

  // Y_hat 
  for (int i = 0; i < NLEN_B256_28; i++)
  {
    ecp2.x.a.g[i] = Y_hat_xa[y_index][i];
    ecp2.y.a.g[i] = Y_hat_ya[y_index][i];
    ecp2.z.a.g[i] = Y_hat_za[y_index][i];

    ecp2.x.b.g[i] = Y_hat_xb[y_index][i];
    ecp2.y.b.g[i] = Y_hat_yb[y_index][i];
    ecp2.z.b.g[i] = Y_hat_zb[y_index][i];
  }
  ecp2.x.a.XES = xesa[y_index][0];
  ecp2.y.a.XES = xesa[y_index][1];
  ecp2.z.a.XES = xesa[y_index][2];

  ecp2.x.b.XES = xesb[y_index][0];
  ecp2.y.b.XES = xesb[y_index][1];
  ecp2.z.b.XES = xesb[y_index][2];
  hash_ECP2(&sh256, ecp2);
  ECP2_toChar(bc + 4 * ecp_size, &ecp2);

  // C1
  for (int i = 0; i < NLEN_B256_28; i++)
  {
    ecp2.x.a.g[i] = C1_xa[y_index][u_index][i];
    ecp2.y.a.g[i] = C1_ya[y_index][u_index][i];
    ecp2.z.a.g[i] = C1_za[y_index][u_index][i];

    ecp2.x.b.g[i] = C1_xb[y_index][u_index][i];
    ecp2.y.b.g[i] = C1_yb[y_index][u_index][i];
    ecp2.z.b.g[i] = C1_zb[y_index][u_index][i];
  }
  ecp2.x.a.XES = C1_xesa[y_index][u_index][0];
  ecp2.y.a.XES = C1_xesa[y_index][u_index][1];
  ecp2.z.a.XES = C1_xesa[y_index][u_index][2];

  ecp2.x.b.XES = C1_xesb[y_index][u_index][0];
  ecp2.y.b.XES = C1_xesb[y_index][u_index][1];
  ecp2.z.b.XES = C1_xesb[y_index][u_index][2];
  ECP2_toChar(bc + 4 * ecp_size + ecp2_size, &ecp2);

  // C2
  for (int i = 0; i < NLEN_B256_28; i++)
  {
    ecp2.x.a.g[i] = C2_xa[rho_index][u_index][i];
    ecp2.y.a.g[i] = C2_ya[rho_index][u_index][i];
    ecp2.z.a.g[i] = C2_za[rho_index][u_index][i];

    ecp2.x.b.g[i] = C2_xb[rho_index][u_index][i];
    ecp2.y.b.g[i] = C2_yb[rho_index][u_index][i];
    ecp2.z.b.g[i] = C2_zb[rho_index][u_index][i];
  }
  ecp2.x.a.XES = C2_xesa[rho_index][u_index][0];
  ecp2.y.a.XES = C2_xesa[rho_index][u_index][1];
  ecp2.z.a.XES = C2_xesa[rho_index][u_index][2];

  ecp2.x.b.XES = C2_xesb[rho_index][u_index][0];
  ecp2.y.b.XES = C2_xesb[rho_index][u_index][1];
  ecp2.z.b.XES = C2_xesb[rho_index][u_index][2];
  ECP2_toChar(bc + 4 * ecp_size + 2 * ecp2_size, &ecp2);

  // Calculate N
  char *ch;
  ch = (char*) malloc(sizeof(char) * ecp2_size + 1);
  for (int i = 0; i < NLEN_B256_28; i++)
  {
    ecp.x.g[i] = N_x[v_index][i];
    ecp.y.g[i] = N_y[v_index][i];
    ecp.z.g[i] = N_z[v_index][i];
  }
  ecp.x.XES = N_xes[v_index][0];
  ecp.y.XES = N_xes[v_index][1];
  ecp.z.XES = N_xes[v_index][2];
  hash_ECP(&sh256, ecp);
  
  // M1_hat
  for (int i = 0; i < NLEN_B256_28; i++)
  {
    ecp2.x.a.g[i] = M1_xa[y_index][n_index][i];
    ecp2.y.a.g[i] = M1_ya[y_index][n_index][i];
    ecp2.z.a.g[i] = M1_za[y_index][n_index][i];

    ecp2.x.b.g[i] = M1_xb[y_index][n_index][i];
    ecp2.y.b.g[i] = M1_yb[y_index][n_index][i];
    ecp2.z.b.g[i] = M1_zb[y_index][n_index][i];
  }
  ecp2.x.a.XES = M1_xesa[y_index][n_index][0];
  ecp2.y.a.XES = M1_xesa[y_index][n_index][1];
  ecp2.z.a.XES = M1_xesa[y_index][n_index][2];

  ecp2.x.b.XES = M1_xesb[y_index][n_index][0];
  ecp2.y.b.XES = M1_xesb[y_index][n_index][1];
  ecp2.z.b.XES = M1_xesb[y_index][n_index][2];
  hash_ECP2(&sh256, ecp2);

  // M2_hat
  for (int i = 0; i < NLEN_B256_28; i++)
  {
    ecp2.x.a.g[i] = M2_xa[v_index][n_index][i];
    ecp2.y.a.g[i] = M2_ya[v_index][n_index][i];
    ecp2.z.a.g[i] = M2_za[v_index][n_index][i];

    ecp2.x.b.g[i] = M2_xb[v_index][n_index][i];
    ecp2.y.b.g[i] = M2_yb[v_index][n_index][i];
    ecp2.z.b.g[i] = M2_zb[v_index][n_index][i];
  }
  ecp2.x.a.XES = M2_xesa[v_index][n_index][0];
  ecp2.y.a.XES = M2_xesa[v_index][n_index][1];
  ecp2.z.a.XES = M2_xesa[v_index][n_index][2];

  ecp2.x.b.XES = M2_xesb[v_index][n_index][0];
  ecp2.y.b.XES = M2_xesb[v_index][n_index][1];
  ecp2.z.b.XES = M2_xesb[v_index][n_index][2];
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
  BIG_modmul(big, c, rho[rho_index], p); // c * rho
  BIG_modadd(big, big, v[v_index], p); // v + c * rho
  BIG_toBytes(bc + 4 * ecp_size + 3 * ecp2_size + m_size + big_size, big);

  // z2
  BIG_modmul(big, c, u[u_index], p); // c * u
  BIG_modadd(big, big, n[n_index], p); // n + c * u
  BIG_toBytes(bc + 4 * ecp_size + 3 * ecp2_size + m_size + 2 * big_size, big);

  #ifdef BROADCAST
  // send using drones broadcast mechanism
  #else
    FILE *fp = fopen("drone/broadcast.txt", "wb");
    fwrite(bc, sizeof(char), 4 * ecp_size + 3 * ecp2_size + m_size + 3 * big_size, fp);
    fclose(fp);
  #endif
}