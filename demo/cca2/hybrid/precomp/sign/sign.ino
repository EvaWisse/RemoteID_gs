#include "exfunc.h"
#include "drone_const.h"

#define FLIGHT_TIME 5 // sec

const int port = 9000;
IPAddress broadcast = IPAddress(192, 168, 244, 255);
const char* ssid = "espcopter";
const char* password = "password";

BIG p, c, big;
ECP ecp;
ECP2 ecp2;
WiFiUDP Udp;
byte i;
hash256 sh256;
int rho_index, v_index, y_index, u_index, n_index;
char m[36];
unsigned long StartTime;

void setup() {
  delay(10000);
  ESP.wdtEnable(WDTO_15MS);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  memset(m, 0, 36);
  BIG_rcopy(p, CURVE_Order);
}

void loop() {
  StartTime = micros();
  char *bc;
  bc = (char*) malloc((4 * ecp_size) + (3 * ecp2_size) + 32 + (2 * big_size) + 36);

  n_index = random(FLIGHT_TIME);
  u_index = random(FLIGHT_TIME);
  rho_index = random(FLIGHT_TIME);
  v_index = random(FLIGHT_TIME);
  y_index = random(FLIGHT_TIME);
  ESP.wdtFeed();

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
  ECP_toChar(bc, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i]);
  ESP.wdtFeed();

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
  ECP_toChar(bc + ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i + ecp_size]);
  ESP.wdtFeed();

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
  ECP_toChar(bc + 2 * ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i + (2 * ecp_size)]);
  ESP.wdtFeed();

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
  ECP_toChar(bc + 3 * ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, bc[i + (3 * ecp_size)]);
  ESP.wdtFeed();

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
  ECP2_toChar(bc + 4 * ecp_size, &ecp2);
  for (i = 0; i < ecp2_size; i++) HASH256_process(&sh256, bc[(4 * ecp_size) + i]);
  ESP.wdtFeed();

  // N
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
  ECP_toChar(ch, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sh256, ch[i]);
  ESP.wdtFeed();

  // M1
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
  ECP_toChar(ch, &ecp);
  for (i = 0; i < ecp2_size; i++) HASH256_process(&sh256, ch[i]);
  ESP.wdtFeed();

  // M2
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
  ECP_toChar(ch, &ecp);
  for (i = 0; i < ecp2_size; i++) HASH256_process(&sh256, ch[i]);
  ESP.wdtFeed();

  // hash message
  for (i = 0; i < 35; i++) HASH256_process(&sh256, m[i]);
  for (i = 0; i < 35; i++) bc[i + (4 * ecp_size) + (3 * ecp2_size) + 32 + (2 * big_size)] = m[i];
  free(ch);
  ESP.wdtFeed();

  // c
  HASH256_hash(&sh256, bc + (4 * ecp_size) + (3 * ecp2_size));
  BIG_fromBytesLen(c, bc + (4 * ecp_size) + (3 * ecp2_size), 32);
  ESP.wdtFeed();

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
  ECP2_toChar(bc + (4 * ecp_size) + (ecp2_size) + 32, &ecp2);
  ESP.wdtFeed();

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
  ECP2_toChar(bc + (4 * ecp_size) + (2 * ecp2_size) + 32, &ecp2);
  ESP.wdtFeed();

  // z1
  BIG_modmul(big, c, rho[rho_index], p); // c * rho
  BIG_modadd(big, big, v[v_index], p); // v + c * rho
  BIG_toBytes(bc + (4 * ecp_size) + (3 * ecp2_size) + 32, big);
  ESP.wdtFeed();

  // z2
  BIG_modmul(big, c, u[u_index], p); // c * u
  BIG_modadd(big, big, n[n_index], p); // n + c * u
  BIG_toBytes(bc + (4 * ecp_size) + (3 * ecp2_size) + 32 + big_size, big);
  ESP.wdtFeed();

  // create udp packet
  Udp.beginPacketMulticast(broadcast, port, WiFi.localIP());
  Udp.write(bc, (4 * ecp_size) + (3 * ecp2_size) + 32 + (2 * big_size) + 36);
  Udp.endPacket();
  ESP.wdtFeed();

  free(bc);
  ESP.wdtFeed();

  Serial.println(micros() - StartTime);
  ESP.wdtFeed();
}
