#include "exfunc.h"

void setup() {
  delay(10000);
  ESP.wdtEnable(WDTO_15MS);
  Serial.begin(115200);
  Serial.println(F("Start atomic test"));

  char *raw;
  csprng RNG;
  raw = (char*) malloc(100 * sizeof(char));
  RAND_seed(&RNG, 100, raw);
  free(raw);

  hash256 sh256;
  byte i;
  BIG big, big1, big2, p;
  unsigned long StartTime;
  ECP2 ecp2, ecp2_mul;
  ECP ecp, ecp_mul;
  char ch[256];

  ECP2_generator(&ecp2);
  ECP_generator(&ecp);

  for (i = 0; i < NLEN_B256_28; i++) p[i] = CURVE_Order[i];

  // Serial.println(F("NUMBER GEN"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    BIG_randomnum(big, p, &RNG);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("ECP copy"));
  for (i = 0; i < rounds; i++) // only one operation per cycle gives 0 for time
  {
    StartTime = micros();
    ECP_copy(&ecp_mul, &ecp);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("ECP2 copy"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    ECP2_copy(&ecp2_mul, &ecp2);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("ECP MUL"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    ECP_clmul(&ecp_mul, big, big);
    Serial.println(micros() - StartTime);
    ECP_copy(&ecp_mul, &ecp);
    ESP.wdtFeed();
  }

  // Serial.println(F("ECP2 MUL BIG"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    ECP2_mul(&ecp2_mul, big);
    Serial.println(micros() - StartTime);
    ECP2_copy(&ecp2_mul, &ecp2);
    ESP.wdtFeed();
  }

  BIG_randomnum(big1, p, &RNG);
  // Serial.println(F("BIG mod add"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    BIG_modadd(big2, big1, big, p);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("BIG mod mul"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    BIG_modmul(big2, big1, big, p);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("BIG inv mod "));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    BIG_invmodp(big2, big, p);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("BIG to Bytes"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    BIG_toBytes(ch, p);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("BIG from Bytes"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    BIG_fromBytesLen(big2, ch, 32);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("ECP_toChar"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    ECP_toChar(ch, &ecp);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("ECP2_toChar"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    ECP2_toChar(ch, &ecp2);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("HASH256_init"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    HASH256_init(&sh256);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("HASH message"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    for (byte j = 0; j < 35; j++) HASH256_process(&sh256, ch[j]);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("HASH ECP"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    for (byte j = 0; j < 65; j++) HASH256_process(&sh256, ch[j]);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("HASH ECP2"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    for (byte j = 0; j < 129; j++) HASH256_process(&sh256, ch[j]);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }

  // Serial.println(F("HASH finalize"));
  for (i = 0; i < rounds; i++)
  {
    StartTime = micros();
    HASH256_hash(&sh256, ch);
    Serial.println(micros() - StartTime);
    ESP.wdtFeed();
  }
  Serial.println(F("DONE"));
  ESP.wdtFeed();
}

void loop() {
  // put your main code here, to run repeatedly:
  ESP.wdtFeed();
}
