#include "exfunc.h"
#include "drone_const.h"

const int port = 9000;
IPAddress broadcast = IPAddress(192, 168, 244, 255);
const char* ssid = "espcopter";
const char* password = "password";

BIG p;
ECP ecp, m1, m2, Y, Z, P;
ECP2 ecp2, Y_hat, P_hat;
WiFiUDP Udp;
csprng RNG;
byte i;
BIG v, rho, y, big;
hash256 sh256;
unsigned long StartTime;

void setup() {
  delay(10000);
  ESP.wdtEnable(WDTO_15MS);
  Serial.begin(115200);

  char *raw;
  raw = (char*) malloc(100 * sizeof(char));
  RAND_seed(&RNG, 100, raw);
  free(raw);

  BIG_rcopy(p, CURVE_Order);

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
}

void loop() {
  StartTime = micros();
  byte i;
  BIG *big;
  big = (BIG*) malloc(5 * sizeof(BIG));
  hash256 sh256;
  HASH256_init(&sh256);
  char *bc;
  bc = (char*) malloc((4 * ecp_size) + (3 * ecp2_size) + 32 + (2 * big_size));
  ESP.wdtFeed();
  
  // m1
  BIG_randtrunc(big[0], p, 2 * CURVE_SECURITY_BN254, &RNG); // select rho in {1, ..., p-1}
  ECP_copy(&ecp, &m1); // copy m1
  ECP_clmul(&ecp, big[0], big[0]); // rho*m1
  ECP_toChar(bc, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i]);
  ESP.wdtFeed();
  
  // m2
  ECP_copy(&ecp, &m2); // copy m2
  ECP_clmul(&ecp, big[0], big[0]); // rho*m2
  ECP_toChar(bc + ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i + ecp_size]);
  ESP.wdtFeed();
  
  // sig Z
  BIG_randtrunc(big[1], p, 2 * CURVE_SECURITY_BN254, &RNG); // select y in {1, ..., p-1} // HERE
  ECP_copy(&ecp, &Z);
  BIG_modmul(big[2], big[0], big[1], p); // rho*y
  ECP_clmul(&ecp, big[2], big[2]); // rho*y*Z
  ECP_toChar(bc + 2 * ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i + (2 * ecp_size)]);
  ESP.wdtFeed();

  // sig Y
  ECP_copy(&ecp, &Y);
  BIG_invmodp(big[2], big[1], p); // inv y mod p
  ECP_clmul(&ecp, big[2], big[2]); // inv y * Y
  ECP_toChar(bc + 3 * ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i + (3 * ecp_size)]);
  ESP.wdtFeed();

  // sig Y_hat
  ECP2_copy(&ecp2, &Y_hat);// copy Y_hat
  ECP2_mul(&ecp2, big[2]); // inv y * Y_hat
  ECP2_toChar(bc + 4 * ecp_size, &ecp2);
  for (i = 0; i < ecp2_size; i++) HASH256_process(&sh256, bc[(4 * ecp_size) + i]);

  // C2
  BIG_randtrunc(big[1], p, 2 * CURVE_SECURITY_BN254, &RNG); // select u in {1, ..., p-1}
  BIG_modadd(big[2], big[0], big[1], p); // rho + u
  ECP2_copy(&ecp2, &P_hat); // copy P_hat
  ECP2_mul(&ecp2, big[2]); // (rho + u)P_hat
  ECP2_toChar(bc + (4 * ecp_size) + (ecp2_size), &ecp2);
  ESP.wdtFeed();
  
  // C1
  ECP2_copy(&ecp2, &Y_hat); // copy Y_hat
  ECP2_mul(&ecp2, big[1]); // uY_hat
  ECP2_toChar(bc + (4 * ecp_size) + (2 * ecp2_size), &ecp2);
  ESP.wdtFeed();
  
  // M1
  char *ch;
  ch = (char*) malloc(ecp2_size * sizeof(char));
  BIG_randtrunc(big[2], p, 2 * CURVE_SECURITY_BN254, &RNG); // select n in {1, ..., p-1}
  ECP2_copy(&ecp2, &Y_hat); // copy Y_hat
  ECP2_mul(&ecp2, big[2]); // nY_hat
  ECP2_toChar(ch, &ecp2);
  for (i = 0; i < ecp2_size; i++) HASH256_process(&sh256, ch[i]);
  ESP.wdtFeed();
  
  // M2
  BIG_randtrunc(big[3], p, 2 * CURVE_SECURITY_BN254, &RNG); // select v in {1, ..., p-1}
  BIG_modadd(big[4], big[2], big[3], p); // v+n
  ECP2_copy(&ecp2, &P_hat);
  ECP2_mul(&ecp2, big[4]); // (v+n)P_hat
  ECP2_toChar(ch, &ecp2);
  for (i = 0; i < ecp2_size; i++) HASH256_process(&sh256, ch[i]);
  ESP.wdtFeed();

  // N
  ECP_copy(&ecp, &P); // P copy
  ECP_clmul(&ecp, big[3], big[3]); // vP
  ECP_toChar(ch, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, ch[i]);
  free(ch);
  ESP.wdtFeed();

  // c
  HASH256_hash(&sh256, bc + (4 * ecp_size) + (3 * ecp2_size));
  BIG_fromBytesLen(big[4], bc + (4 * ecp_size) + (3 * ecp2_size), 32);
  ESP.wdtFeed();

  // z1
  BIG_modmul(big[0], big[4], big[0], p); // c * rho
  BIG_modadd(big[0], big[0], big[3], p); // v + c * rho
  BIG_toBytes(bc + (4 * ecp_size) + (3 * ecp2_size) + 32, big[0]);
  ESP.wdtFeed();

  // z2
  BIG_modmul(big[3], big[4], big[1], p); // c * u
  BIG_modadd(big[1], big[3], big[2], p); // n + c * u
  BIG_toBytes(bc + (4 * ecp_size) + (3 * ecp2_size) + 32 + big_size, big[1]);
  ESP.wdtFeed();

  // create udp packet
//  Udp.beginPacketMulticast(broadcast, port, WiFi.localIP());
//  Udp.write(bc, (4 * ecp_size) + (3 * ecp2_size) + 32 + (2 * big_size));
//  Udp.endPacket();

  free(big);
  free(bc);

  Serial.println(micros() - StartTime);
}
