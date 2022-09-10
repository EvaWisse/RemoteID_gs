// #include "../../../../../../include/miracl.h"
#include "../../../../../../include/utils.h"
#include "drone_const.h"

#define FLIGHT_TIME 1
int main()
{ 
  //create variables for reading header
  BIG p;
  ECP m1, m2, P, Y, Z;
  ECP2 P_hat, Y_hat;

  // read constants from header
  BIG_rcopy(p, CURVE_Order);

  // m1
  BIG x, y;
  BIG_fromChar(&x, ch_m1);
  BIG_fromChar(&y, ch_m1 + (1<<6));
  ECP_set(&m1, x, y);

  // m2
  BIG_fromChar(&x, ch_m2);
  BIG_fromChar(&y, ch_m2 + (1<<6));
  ECP_set(&m2, x, y);

  // P
  BIG_rcopy(x, CURVE_Gx);
  BIG_rcopy(y, CURVE_Gy);
  ECP_set(&P, x, y);

  // Y
  BIG_fromChar(&x, ch_Y);
  BIG_fromChar(&y, ch_Y + (1<<6));
  ECP_set(&Y, x, y);

  // Z
  BIG_fromChar(&x, ch_Z);
  BIG_fromChar(&y, ch_Z + (1<<6));
  ECP_set(&Z, x, y);

  // Y_hat
  FP2 wx, wy;
  BIG_fromChar(&x, ch_Y_hat);
  BIG_fromChar(&y, ch_Y_hat + (1<<6));

  FP_nres(&(wx.a), x);
  FP_nres(&(wx.b), y);

  BIG_fromChar(&x, ch_Y_hat + (1<<7));
  BIG_fromChar(&y, ch_Y_hat + (1<<7) + 64);

  FP_nres(&(wy.a), x);
  FP_nres(&(wy.b), y);
  ECP2_set(&Y_hat, &wx, &wy);

  // p_hat
  FP2_rcopy(&wx, CURVE_Pxa, CURVE_Pxb);
  FP2_rcopy(&wy, CURVE_Pya, CURVE_Pyb);
  ECP2_set(&P_hat, &wx, &wy);

  // create precomputed values
  BIG precomp_rho[FLIGHT_TIME], precomp_v[FLIGHT_TIME], precomp_y[FLIGHT_TIME], precomp_inv[FLIGHT_TIME], big;
  ECP precomp_m1[FLIGHT_TIME], precomp_m2[FLIGHT_TIME], precomp_Y[FLIGHT_TIME], precomp_Z[FLIGHT_TIME][FLIGHT_TIME], precomp_N[FLIGHT_TIME];
  ECP2 precomp_Y_hat[FLIGHT_TIME];

  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len = 100;
  CREATE_CSPRNG(&RNG, &RAW);

  // calulate precomputes values
  for(int i=0; i<FLIGHT_TIME; i++)
  {
    ECP_copy(&precomp_m1[i], &m1);
    ECP_copy(&precomp_m2[i], &m2);
    ECP_copy(&precomp_Y[i], &Y);
    ECP_copy(&precomp_N[i], &P);
    ECP2_copy(&precomp_Y_hat[i], &Y_hat);

    BIG_random(precomp_rho[i], &RNG);
    BIG_random(precomp_v[i], &RNG);
    BIG_random(precomp_y[i], &RNG);

    BIG_invmodp(precomp_inv[i], precomp_y[i], p);

    ECP_mul(&precomp_m1[i], precomp_rho[i]);
    ECP_mul(&precomp_m2[i], precomp_rho[i]);
    ECP_mul(&precomp_Y[i], precomp_inv[i]);
    ECP_mul(&precomp_N[i], precomp_v[i]);
    ECP2_mul(&precomp_Y_hat[i], precomp_inv[i]);

    for(int j; j<FLIGHT_TIME; j++)
    {
      ECP_copy(&precomp_Z[i][j], &Z);
      BIG_modmul(big, precomp_rho[i], precomp_v[j], p);
      ECP_mul(&precomp_Z[i][j], big);
    }
  }

  // write values to file
  FILE *fp =  fopen("demo/cpa/hybrid/precomp/sign/sign/precomp.h", "w");
  if(!fp)
  {
    printf("\tERROR, could not create \"precomp.h\"\n");
    return EXIT_FAILURE;
  }
  fprintf(fp, "#include\"exfunc.h\"\n#ifndef PRECOMP_UAS_H\n#define PRECOMP_UAS_H\n");
  fprintf(fp, "char ch_m1[]=\"");
  for(int i=0; i<FLIGHT_TIME; i++) ECP_toheader(precomp_m1[i], fp); 
  fprintf(fp, "\";\nchar ch_m2[] =\"");
  for(int i=0; i<FLIGHT_TIME; i++) ECP_toheader(precomp_m2[i], fp); 
  fprintf(fp, "\";\nchar ch_Y[] =\"");
  for(int i=0; i<FLIGHT_TIME; i++) ECP_toheader(precomp_Y[i], fp); 
  fprintf(fp, "\";\nchar ch_Y_hat[] =\"");
  for(int i=0; i<FLIGHT_TIME; i++) ECP2_toheader(precomp_Y_hat[i], fp); 
  fprintf(fp, "\";\nchar ch_N[] =\"");
  for(int i=0; i<FLIGHT_TIME; i++) ECP_toheader(precomp_N[i], fp); 
  fprintf(fp, "\";\nchar ch_y[] =\"");
  for(int i=0; i<FLIGHT_TIME; i++)  BIG_toFile(precomp_y[i], fp); 
   fprintf(fp, "\";\nchar ch_v[] =\"");
  for(int i=0; i<FLIGHT_TIME; i++)  BIG_toFile(precomp_v[i], fp);
  fprintf(fp, "\";\nchar ch_rho[] =\"");
  for(int i=0; i<FLIGHT_TIME; i++)  BIG_toFile(precomp_rho[i], fp); 
  fprintf(fp, "\";\nchar ch_Z[] =\"");
  ECP_toheader(Z, fp); 
  fprintf(fp, "\";\n#endif\n");
  fclose(fp);

  return EXIT_SUCCESS;
}