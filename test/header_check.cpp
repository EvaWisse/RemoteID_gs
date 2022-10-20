#include "../include/miracl.h"
#include "../include/utils.h"
#define ROUNDS 100
// #define WRITE

BIG big[ROUNDS], p;
ECP ecp[ROUNDS], ecp_file[ROUNDS], g1;
ECP2 ecp2[ROUNDS], ecp2_file[ROUNDS], g2;

int header();

#ifdef WRITE
int main()
{
  BIG_rcopy(p, CURVE_Order);
  ECP_generator(&g1);
  ECP2_generator(&g2);

  csprng RNG;
  char raw[100];
  octet RAW = {0, sizeof(raw), raw};
  RAW.len =100;
  CREATE_CSPRNG(&RNG,  &RAW);

  FILE *fp = fopen("test/big.txt", "wb");
  for(int i=0; i<ROUNDS; i++)
  {
    BIG_randomnum(big[i], p, &RNG);
    BIG_toFile(big[i], fp);
  }
  
  for(int i=0; i<ROUNDS; i++)
  {
    ECP_copy(&ecp[i], &g1);
    ECP_mul(&ecp[i], big[i]);
    ECP_toFile(ecp[i], fp);
  }

  for(int i=0; i<ROUNDS; i++)
  {
    ECP2_copy(&ecp2[i], &g2);
    ECP2_mul(&ecp2[i], big[i]);
    ECP2_toFile(ecp2[i], fp);
  }
  
  fclose(fp);
  header();
}
#else
#include "test.h"
int main()
{
  ECP_generator(&g1);
  ECP2_generator(&g2);

  FILE *fp = fopen("test/big.txt", "rb");
  for(int i=0; i<ROUNDS; i++)
  {
    BIG_fromFile(fp, &big[i]);
  }
  
  for(int i=0; i<ROUNDS; i++)
  {
    ECP_fromFile(fp, &ecp_file[i]);
    ECP_copy(&ecp[i], &g1);
    ECP_mul(&ecp[i], big[i]);
    if(!ECP_equals(&ecp[i], &ecp_file[i])) printf("%d: failed ECP\n", i);
  }

  for(int i=0; i<ROUNDS; i++)
  {
    ECP2_fromFile(fp, &ecp2_file[i]);
    ECP2_copy(&ecp2[i], &g2);
    ECP2_mul(&ecp2[i], big[i]);
    if(!ECP2_equals(&ecp2[i], &ecp2_file[i])) printf("%d: failed ECP2\n", i);
  }
  fclose(fp);

  for(int i=0; i<ROUNDS; i++)
  {
    ECP_copy(&ecp[i], &g1);
    ECP2_copy(&ecp2[i], &g2);
  } 

  for(int j=0; j<ROUNDS; j++)
  {
    for (int i = 0; i < NLEN_B256_28; i++)
    {
      ecp[j].x.g[i] = ecp_x[j][i];
      ecp[j].y.g[i] = ecp_y[j][i];
      ecp[j].z.g[i] = ecp_z[j][i];
    }
    ecp[j].x.XES = ecp_xes[j][0];
    ecp[j].y.XES = ecp_xes[j][1];
    ecp[j].z.XES = ecp_xes[j][2];
  }

  for(int j=0; j<ROUNDS; j++)
  {
    for (int i = 0; i < NLEN_B256_28; i++)
    {
      ecp2[j].x.a.g[i] = ecp2_xa[j][i];
      ecp2[j].y.a.g[i] = ecp2_ya[j][i];
      ecp2[j].z.a.g[i] = ecp2_za[j][i];

      ecp2[j].x.b.g[i] = ecp2_xb[j][i];
      ecp2[j].y.b.g[i] = ecp2_yb[j][i];
      ecp2[j].z.b.g[i] = ecp2_zb[j][i];
    }
    ecp2[j].x.a.XES = xesa[j][0];
    ecp2[j].y.a.XES = xesa[j][1];
    ecp2[j].z.a.XES = xesa[j][2];

    ecp2[j].x.b.XES = xesb[j][0];
    ecp2[j].y.b.XES = xesb[j][1];
    ecp2[j].z.b.XES = xesb[j][2];
  }

  for (int i = 0; i<ROUNDS; i++)
  {
    if(!ECP_equals(&ecp[i], &ecp_file[i])) printf("%d header ecp error\n", i);
    if(!ECP2_equals(&ecp2[i], &ecp2_file[i])) printf("%d header ecp error\n", i);
  }
  printf("done");
}
#endif

int header()
{
  FILE *fp = fopen("test/test.h", "w");
  if(!fp)
  {
    printf("Could not create drone_const.h\n");
    return EXIT_FAILURE;
  }
  
  fprintf(fp, "#include \"../include/miracl.h\"\n#ifndef CONST_UAS_H\n#define CONST_UAS_H\n");

  // Y 
  fprintf(fp, "BIG ecp_x[%d] = {", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", ecp[j].x.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG ecp_y[%d] = {", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", ecp[j].y.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG ecp_z[%d] = {", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{"); 
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", ecp[j].z.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  
  fprintf(fp, "};\nconst sign32 ecp_xes[%d][3]= {", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    fprintf(fp, "%#04x, ", ecp[j].x.XES);
    fprintf(fp, "%#04x, ", ecp[j].y.XES);
    fprintf(fp, "%#04x, ", ecp[j].z.XES);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "};\n");

  fprintf(fp, "BIG ecp2_ya[%d]={", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", ecp2[j].y.a.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG ecp2_yb[%d]={", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", ecp2[j].y.b.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG ecp2_za[%d]={", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", ecp2[j].z.a.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG ecp2_zb[%d]={", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", ecp2[j].z.b.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG ecp2_xa[%d]={", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", ecp2[j].x.a.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nBIG ecp2_xb[%d]={", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", ecp2[j].x.b.g[i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nconst sign32 xesa[%d][3]= {", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    fprintf(fp, "%#04x, ", ecp2[j].x.a.XES);
    fprintf(fp, "%#04x, ", ecp2[j].y.a.XES);
    fprintf(fp, "%#04x, ", ecp2[j].z.a.XES);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);

  fprintf(fp, "};\nconst sign32 xesb[%d][3]= {", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    fprintf(fp, "%#04x, ", ecp2[j].x.b.XES);
    fprintf(fp, "%#04x, ", ecp2[j].y.b.XES);
    fprintf(fp, "%#04x, ", ecp2[j].z.b.XES);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "};\n");
    
  fprintf(fp, "BIG big_h[%d] = {", ROUNDS);
  for(int j=0; j<ROUNDS; j++)
  {
    fprintf(fp, "{");
    for(int i=0;i<NLEN_B256_28;i++) fprintf(fp, "%#08x, ", big[j][i]);
    fseek(fp, -2, SEEK_CUR);
    fprintf(fp, "}, ");
  }
  fseek(fp, -2, SEEK_CUR);
  fprintf(fp, "};\n#endif");
  fclose(fp);
  return EXIT_SUCCESS;

}
