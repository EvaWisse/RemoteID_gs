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

void setup() {
  delay(10000);
  ESP.wdtEnable(WDTO_15MS);
  Serial.begin(115200);

  char *raw;
  raw = (char*) malloc(100 * sizeof(char));
  RAND_seed(&RNG, 100, raw);
  free(raw);

  BIG_rcopy(p, CURVE_Order);

  // m1
  BIG x, y;
  BIG_rcopy(x, ECP0_x);
  BIG_rcopy(y, ECP0_y);
  ECP_set(&m1, x, y);

  // m2
  BIG_rcopy(x, ECP1_x);
  BIG_rcopy(y, ECP1_y);
  ECP_set(&m2, x, y);

  // P
  BIG_rcopy(x, CURVE_Gx);
  BIG_rcopy(y, CURVE_Gy);
  ECP_set(&P, x, y);

  // Y
  BIG_rcopy(x, ECP2_x);
  BIG_rcopy(y, ECP2_y);
  ECP_set(&Y, x, y);

  // Z
  BIG_rcopy(x, ECP3_x);
  BIG_rcopy(y, ECP3_y);
  ECP_set(&Z, x, y);

  // Y_hat
  FP2 wx, wy;
  FP2_rcopy(&wx, ECP0_x1, ECP0_y1);
  FP2_rcopy(&wy, ECP0_x2, ECP0_y2);
  ECP2_set(&Y_hat, &wx, &wy);

  // p_hat
  FP2_rcopy(&wx, CURVE_Pxa, CURVE_Pxb);
  FP2_rcopy(&wy, CURVE_Pya, CURVE_Pyb);
  ECP2_set(&P_hat, &wx, &wy);
}

void loop() {
  Serial.println(F("statr"));

  HASH256_init(&sh256);
  char *bc;
  bc = (char*) malloc((4 * ecp_size) + ecp2_size + 32 + big_size);
  ESP.wdtFeed();

  // m1
  BIG_randomnum(rho, p, &RNG); // select rho in {1, ..., p-1}
  ECP_copy(&ecp, &m1); // copy m1
  ECP_clmul(&ecp, rho, rho); // rho*m1
  ECP_toChar(bc, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i]);
  ESP.wdtFeed();

  // m2
  ECP_copy(&ecp, &m2); // copy m2
  ECP_clmul(&ecp, rho, rho); // rho*m2
  ECP_toChar(bc + ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i + ecp_size]);

  // sig Z
  BIG_randomnum(y, p, &RNG); // select y in {1, ..., p-1}
  ECP_copy(&ecp, &Z);
  BIG_modmul(big, y, rho, p); // rho*y
  ECP_clmul(&ecp, big, big); // rho*y*Z
  ECP_toChar(bc + 2 * ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i + (2 * ecp_size)]);
  ESP.wdtFeed();

  //   sig Y
  ECP_copy(&ecp, &Y);
  BIG_invmodp(big, y, p); // inv y mod p
  ECP_clmul(&ecp, big, big); // inv y * Y
  ECP_toChar(bc + 3 * ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i + (3 * ecp_size)]);
  ESP.wdtFeed();

  // sig Y_hat
  ECP2_copy(&ecp2, &Y_hat);// copy Y_hat
  ECP2_mul(&ecp2, big); // inv y * Y_hat
  ECP2_toChar(bc + 4 * ecp_size, &ecp2);
  for (i = 0; i < ecp2_size; i++) HASH256_process(&sh256, bc[(4 * ecp_size) + i]);
  ESP.wdtFeed();

  // N
  char *ch;
  ch = (char*) malloc(sizeof(char) * ecp_size + 1);
  BIG_randomnum(rho, p, &RNG); // select rho in {1, ..., p-1}
  ECP_copy(&ecp, &P); // P copy
  ECP_clmul(&ecp, v, v); // vP
  ECP_toChar(ch, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, ch[i]);
  free(ch);
  ESP.wdtFeed();

  // c
  HASH256_hash(&sh256, bc + (4 * ecp_size) + (ecp2_size));
  BIG_fromBytesLen(big, bc + (4 * ecp_size) + (ecp2_size), 32);
  ESP.wdtFeed();

  // z1
  BIG_modmul(big, big, rho, p); // c * rho
  BIG_modadd(big, big, v, p); // v + c * rho
  BIG_toBytes(bc + (4 * ecp_size) + (ecp2_size) + 32, big);
  ESP.wdtFeed();

  // create udp packet
  Udp.beginPacketMulticast(broadcast, port, WiFi.localIP());
  Udp.write(bc, (4 * ecp_size) + ecp2_size + 32 + big_size);
  Udp.endPacket();
  free(bc);
  Serial.println(F("done"));
}
