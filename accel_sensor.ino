#include <Arduino.h>
#include "accel_sensor.h"

#define CTRL_REG1  (0x20)
#define CTRL_REG2  (0x21)
#define CTRL_REG3  (0x22)
#define CTRL_REG4  (0x23)
#define CTRL_REG5  (0x24)
#define CTRL_REG6  (0x25)
#define OUT_X_L    (0x28)
#define OUT_X_H    (0x29)
#define OUT_Y_L    (0x2A)
#define OUT_Y_H    (0x2B)
#define OUT_Z_L    (0x2C)
#define OUT_Z_H    (0x2D)
#define INT1_SRC   (0x31)
#define CLICK_CFG  (0x38)
#define CLICK_SRC  (0x39)
#define CLICK_THS  (0x3A)
#define TIME_LIMIT (0x3B)

const unsigned char AccelInitPacketClick[6] = { 
                                          0x77,  // enable all axes, set to 400Hz sample rate
                                          0x04,  // enable high-pass filter for click
                                          0x80,  // enable interrupt on click
                                          0x00,  // don't touch default CTRL_REG4
                                          0x08,  // latch interrupt for testing
//                                          0x00,  // no latch
                                          0x00   // int is active high, int2 disabled
                                         };
                                         
const unsigned char AccelInitPacket[6] = { 
                                          0x77,  // enable all axes, set to 400Hz sample rate
                                          0x00,  // enable high-pass filter for click
                                          0x00,  // enable interrupt on click
                                          0x00,  // don't touch default CTRL_REG4
                                          0x00,  // latch interrupt for testing
                                          0x00   // int is active high, int2 disabled
                                         };
                                         
const unsigned char InitClickPacket[4] = {
                                          0x15,
                                          0x10,
                                          200,
                                          0x10
                                         };

                                          
void readFromAccel(unsigned char n_bytes, unsigned char start_addr, unsigned char *dest) {
  unsigned char i;
  Wire.beginTransmission(ACCEL_ADDRESS);
  Wire.write(start_addr | 0x80);
  Wire.endTransmission(false);
  Wire.requestFrom(ACCEL_ADDRESS, n_bytes);
  for (i=0; i<n_bytes; i++) {
    *dest++ = Wire.read();
  }
  return;
}

void writeToAccel(unsigned char n_bytes, unsigned char start_addr, unsigned char *src) {
  unsigned char i;
  unsigned char sub_addr;
  if (n_bytes==1) sub_addr = start_addr;
  else sub_addr = start_addr | 0x80;
  Wire.beginTransmission(ACCEL_ADDRESS);
  Wire.write(sub_addr);
  for (i=0; i<n_bytes; i++) {
    Wire.write(*src++);
  }
  Wire.endTransmission(true);
  return;
}
  
void initAccelSensor(void) {
  Wire.begin();
  writeToAccel(6, CTRL_REG1, (unsigned char*)AccelInitPacketClick);
}

void toggleClick(bool enable) {
  // Set up click...
  writeToAccel(4, CLICK_CFG, (unsigned char*)InitClickPacket);
}  

unsigned char readInt1Src(void) {
  unsigned char result;
  readFromAccel(1, INT1_SRC, &result);
  return result;
}

unsigned char readClickSrc(void) {
  unsigned char result;
  readFromAccel(1, CLICK_SRC, &result);
  return result;
}

void clearInterrupt(void) {
  readInt1Src();
  readClickSrc();
}

void readAccelSensor(accel_data_t * result) {
  unsigned char result_raw[6];
  unsigned char *outPtr = result_raw;
  readFromAccel(6, OUT_X_L, result_raw);
  result->accel_X = (int)(*outPtr++);
  result->accel_X |= ((int)*outPtr++)<<8;
  result->accel_Y = (int)*outPtr++;
  result->accel_Y |= ((int)*outPtr++)<<8;
  result->accel_Z = (int)*outPtr++;
  result->accel_Z |= ((int)*outPtr)<<8;
}
