#ifndef _ACCEL_SENSOR_H_
#define _ACCEL_SENSOR_H_

#define ACCEL_ADDRESS 0x18

typedef struct {
  int accel_X;
  int accel_Y;
  int accel_Z;
} accel_data_t;

void readFromAccel(unsigned char n_bytes, unsigned char start_addr, unsigned char *dest);
void writeToAccel(unsigned char n_bytes, unsigned char start_addr, unsigned char *src);
void initAccelSensor(void);
void toggleClick(bool enable);
void clearInterrupt(void);
unsigned char readInt1Src(void);
void readAccelSensor(accel_data_t * result);

#endif

