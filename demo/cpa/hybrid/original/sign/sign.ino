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
char m[36];
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

  // p_hat
  FP2 wx, wy;
  FP2_rcopy(&wx, CURVE_Pxa, CURVE_Pxb);
  FP2_rcopy(&wy, CURVE_Pya, CURVE_Pyb);
  ECP2_set(&P_hat, &wx, &wy);

  memset(m, 0, 36);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  StartTime = micros();
  HASH256_init(&sh256);
  char *bc;
  bc = (char*) malloc((4 * ecp_size) + ecp2_size + 32 + big_size + 36);
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
  ESP.wdtFeed();

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
  ESP.wdtFeed();

  // message
  for (i = 0; i < 35; i++) HASH256_process(&sh256, m[i]);
  for (i = 0; i < 35; i++) bc[((4 * ecp_size) + ecp2_size + 32 + big_size) + i] = m[i];
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
  Udp.write(bc, (4 * ecp_size) + ecp2_size + 32 + big_size + 36);
  Udp.endPacket();
  ESP.wdtFeed();

  free(bc);
  ESP.wdtFeed();

  Serial.println(micros() - StartTime);
  ESP.wdtFeed();

}
