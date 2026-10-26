#pragma once
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "sensor_types.h"

class ImuService {
 public:
  bool begin() {
    if (!imu_.begin()) return false;
    imu_.setAccelerometerRange(MPU6050_RANGE_16_G);
    imu_.setGyroRange(MPU6050_RANGE_500_DEG);
    imu_.setFilterBandwidth(MPU6050_BAND_21_HZ);
    return true;
  }
  ImuSample read(unsigned long nowMs) {
    sensors_event_t acceleration, gyro, temperature;
    imu_.getEvent(&acceleration, &gyro, &temperature);
    constexpr float MS2_TO_G = 1.0f / 9.80665f;
    constexpr float RAD_TO_DPS = 57.2957795f;
    return {acceleration.acceleration.x * MS2_TO_G, acceleration.acceleration.y * MS2_TO_G,
            acceleration.acceleration.z * MS2_TO_G, gyro.gyro.x * RAD_TO_DPS,
            gyro.gyro.y * RAD_TO_DPS, gyro.gyro.z * RAD_TO_DPS, nowMs};
  }
 private:
  Adafruit_MPU6050 imu_;
};

