#include "exfunc.h"
#include "precomp.h"
#define FLIGHT_TIME 60 // sec

const int port = 9000;
IPAddress broadcast = IPAddress(192, 168, 244, 255);
const char* ssid = "espcopter";
const char* password = "password";

int long StartTime;
BIG p;
ECP ecp, Z;
ECP2 ecp2;
WiFiUDP Udp;
byte i;
int rho_index, v_index, y_index;
BIG big, x, y;
FP2 wx, wy;
hash256 sh256;

void setup() {
  delay(10000);
  ESP.wdtEnable(WDTO_15MS);
  Serial.begin(115200);

  BIG_fromChar(&x, ch_Z);
  BIG_fromChar(&y, ch_Z + (1 << 6));
  ECP_set(&Z, x, y);
  BIG_rcopy(p, CURVE_Order);
}

void loop() {
  StartTime = micros();
  HASH256_init(&sh256);
  char *bc;
  bc = (char*) malloc((4 * 129) + 257 + 32 + 65);
  ESP.wdtFeed();

  rho_index = random(FLIGHT_TIME);
  v_index = random(FLIGHT_TIME);
  y_index = random(FLIGHT_TIME);
  ESP.wdtFeed();

  // m1
  for (i = 0; i < 129; i++)
  {
    HASH256_process(&sh256, ch_m1[(rho_index * 129) + i]);
    bc[i] = ch_m1[(rho_index * 129) + i];
  }
  ESP.wdtFeed();

  // m2
  for (i = 0; i < 130; i++)
  {
    HASH256_process(&sh256, ch_m2[(rho_index * 129) + i]);
    bc[i + 129] = ch_m2[(rho_index * 129) + i];
  }
  ESP.wdtFeed();

  // Y
  for (i = 0; i < 129; i++)
  {
    HASH256_process(&sh256, i + ch_Y[(y_index * 129) + 1]);
    bc[i + (129 * 2)] = ch_Y[(y_index * 129) + i];
  }
  ESP.wdtFeed();

  // Z
  BIG y_big, rho_big;
  BIG_fromChar(&y_big, ch_y + (rho_index * 129));
  BIG_fromChar(&rho_big, ch_rho + (rho_index * 129));
  BIG_modmul(big, y_big, rho_big, p);
  ECP_copy(&ecp, &Z);
  ECP_clmul(&ecp, big, big);
  ECP_toChar(bc + (3 * 129), &ecp);
  for (i = 0; i < 129; i++) HASH256_process(&sh256, bc[i + (3 * 129)]);
  ESP.wdtFeed();

  // Y_hat
  for (i = 0; i < 257; i++)
  {
    HASH256_process(&sh256, ch_Y_hat[(257 * y_index) + 1]);
    bc[i + (129 * 4)] = ch_Y_hat[(y_index * 257) + i];
  }
  ESP.wdtFeed();

  // N
  for (i = 0; i < 129; i++)  HASH256_process(&sh256, ch_N[i]);
  ESP.wdtFeed();

  // c
  HASH256_hash(&sh256, bc + (4 * 129) + (257));
  BIG_fromBytesLen(big, bc + (4 * 129) + (257), 32);
  ESP.wdtFeed();

  // z1
  BIG v_big;
  BIG_fromChar(&v_big, ch_v + (v_index * 64));
  BIG_modmul(big, big, rho_big, p); // c * rh
  BIG_modadd(big, big, v_big, p); // v + c * rho
  BIG_toBytes(bc + (4 * 129) + (257) + 32, big);
  ESP.wdtFeed();

  // create udp packet
  // Udp.beginPacketMulticast(broadcast, port, WiFi.localIP());
  // Udp.write(bc, (4 * ecp_size) + ecp2_size + 32 + big_size);
  // Udp.endPacket();
  free(bc);
  ESP.wdtFeed();

  Serial.println(micros() - StartTime);
}
