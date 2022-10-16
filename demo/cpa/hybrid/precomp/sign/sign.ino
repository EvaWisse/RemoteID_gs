#include "exfunc.h"
#include "drone_const.h"
#define FLIGHT_TIME 10

//set up to connect to an existing network (e.g. mobile hotspot from laptop that will run the python code)
const char* ssid = "espcopter";
const char* password = "password";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  //  port to listen on
char incomingPacket[255];  // buffer for incoming packets

BIG p, big, c;
ECP ecp;
ECP2 ecp2;
int rho_index, v_index, y_index;
char m[32];
byte i;
char ch[4 * ecp_size + ecp2_size + m_size + 2 * big_size + 1];
hash256 sha256;
char t[32];

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  // we recv one packet from the remote so we can know its IP and port
  bool readPacket = false;
  while (!readPacket) {
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
      // receive incoming UDP packets
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
      int len = Udp.read(incomingPacket, 255);
      if (len > 0)
      {
        incomingPacket[len] = 0;
      }
      Serial.printf("UDP packet contents: %s\n", incomingPacket);
      readPacket = true;
    }
  }
  BIG_rcopy(p, CURVE_Order);
  memset(m, 0, 36);
}

void loop()
{
  HASH256_init(&sha256);

  rho_index = random(FLIGHT_TIME);
  v_index = random(FLIGHT_TIME);
  y_index = random(FLIGHT_TIME);
  ESP.wdtFeed();

  // m1
  for (i = 0; i < NLEN_B256_28; i++)
  {
    ecp.x.g[i] = m1_x[rho_index][i];
    ecp.y.g[i] = m1_y[rho_index][i];
    ecp.z.g[i] = m1_z[rho_index][i];
  }
  ecp.x.XES = m1_xes[rho_index][0];
  ecp.y.XES = m1_xes[rho_index][1];
  ecp.z.XES = m1_xes[rho_index][2];
  ECP_toChar(ch, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sha256, ch[i]);
  ESP.wdtFeed();

  // m2
  for (i = 0; i < NLEN_B256_28; i++)
  {
    ecp.x.g[i] = m2_x[rho_index][i];
    ecp.y.g[i] = m2_y[rho_index][i];
    ecp.z.g[i] = m2_z[rho_index][i];
  }
  ecp.x.XES = m2_xes[rho_index][0];
  ecp.y.XES = m2_xes[rho_index][1];
  ecp.z.XES = m2_xes[rho_index][2];
  ECP_toChar(ch + ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sha256, ch[i + ecp_size]);
  ESP.wdtFeed();

  // Y
  for (i = 0; i < NLEN_B256_28; i++)
  {
    ecp.x.g[i] = Y_x[y_index][i];
    ecp.y.g[i] = Y_y[y_index][i];
    ecp.z.g[i] = Y_z[y_index][i];
  }
  ecp.x.XES = Y_xes[y_index][0];
  ecp.y.XES = Y_xes[y_index][1];
  ecp.z.XES = Y_xes[y_index][2];
  ECP_toChar(ch + 2 * ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sha256, ch[i + 2 * ecp_size]);
  ESP.wdtFeed();

  // Z
  for (i = 0; i < NLEN_B256_28; i++)
  {
    ecp.x.g[i] = Z_x[rho_index][y_index][i];
    ecp.y.g[i] = Z_y[rho_index][y_index][i];
    ecp.z.g[i] = Z_z[rho_index][y_index][i];
  }
  ecp.x.XES = Z_xes[rho_index][y_index][0];
  ecp.y.XES = Z_xes[rho_index][y_index][1];
  ecp.z.XES = Z_xes[rho_index][y_index][2];
  ECP_toChar(ch + 3 * ecp_size, &ecp);
  for (i = 0; i < ecp_size; i++) HASH256_process(&sha256, ch[i + 3 * ecp_size]);
  ESP.wdtFeed();

  // Y_hat
  for ( i = 0; i < NLEN_B256_28; i++)
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
  ECP2_toChar(ch + 4 * ecp_size, &ecp2);
  for (i = 0; i < ecp2_size; i++) HASH256_process(&sha256, ch[i + 4 * ecp_size]);
  ESP.wdtFeed();

 // N
 for (int i = 0; i < NLEN_B256_28; i++)
 {
   ecp.x.g[i] = N_x[v_index][i];
   ecp.y.g[i] = N_y[v_index][i];
   ecp.z.g[i] = N_z[v_index][i];
 }
 ecp.x.XES = N_xes[v_index][0];
 ecp.y.XES = N_xes[v_index][1];
 ecp.z.XES = N_xes[v_index][2];
 ECP_toChar(t, &ecp);
 for (i = 0; i < ecp_size; i++) HASH256_process(&sha256, t[i]);
 ESP.wdtFeed();

 // m
 for (i = 0; i < m_size; i++) HASH256_process(&sha256, m[i]);
 //  for (i = 0; i < m_size; i++) ch[i + 4 * ecp_size + ecp2_size] = m[i];
 ESP.wdtFeed();

 // c
 HASH256_hash(&sha256, ch + 4 * ecp_size + ecp2_size + m_size);
 ESP.wdtFeed();

 BIG_fromBytesLen(c, ch + 4 * ecp_size + ecp2_size + m_size, 32);
 ESP.wdtFeed();

 // z
 BIG_modmul(big, c, rho[rho_index], p); // c * rho mod p
 BIG_modadd(big, big, v[v_index], p); // v + c * rho
 BIG_toBytes(ch + 4 * ecp_size + ecp2_size + m_size + big_size, big);
 ESP.wdtFeed();

  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(ch, 4 * ecp_size + ecp2_size + m_size + 2 * big_size);
  Udp.endPacket();
  ESP.wdtFeed();

  memset(ch, 0, 4 * ecp_size + ecp2_size + 3 * big_size + 1);
  memset(t, 0, 32);
  ESP.wdtFeed();
  delay(1000);
}
