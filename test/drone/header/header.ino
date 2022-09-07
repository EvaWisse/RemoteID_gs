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

  // m1
  BIG x, y;
  BIG_rcopy(x, ECP0_x);
  BIG_rcopy(y, ECP0_y);
  ECP_set(&m1, x, y);
}

void loop() { 
  ESP.wdtFeed();
}
