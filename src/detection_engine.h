#pragma once
#include <math.h>
#include "sensor_types.h"

// Deterministic, Arduino-independent detection core. Thresholds must be tuned in real vehicles.
class DetectionEngine {
 public:
  DetectionEngine(float impactThresholdG, float highSeverityThresholdG,
                  float gyroThresholdDps, unsigned long cooldownMs)
      : impactThresholdG_(impactThresholdG), highSeverityThresholdG_(highSeverityThresholdG),
        gyroThresholdDps_(gyroThresholdDps), cooldownMs_(cooldownMs), lastTriggerMs_(0),
        hasTriggered_(false) {}

  DetectionResult evaluate(const ImuSample& sample) {
    const float magnitude = sqrtf(sample.axG * sample.axG + sample.ayG * sample.ayG + sample.azG * sample.azG);
    const float impact = fabsf(magnitude - 1.0f);
    const float gyro = max3(fabsf(sample.gxDps), fabsf(sample.gyDps), fabsf(sample.gzDps));
    const bool cooldownActive = hasTriggered_ && (sample.capturedAtMs - lastTriggerMs_ < cooldownMs_);
    const bool trigger = !cooldownActive &&
        (impact >= impactThresholdG_ || (impact >= impactThresholdG_ * 0.65f && gyro >= gyroThresholdDps_));
    if (trigger) {
      lastTriggerMs_ = sample.capturedAtMs;
      hasTriggered_ = true;
    }
    return {trigger, trigger ? (impact >= highSeverityThresholdG_ ? Severity::High : Severity::Low) : Severity::None,
            impact, gyro};
  }

 private:
  static float max3(float a, float b, float c) { return a > b ? (a > c ? a : c) : (b > c ? b : c); }
  float impactThresholdG_, highSeverityThresholdG_, gyroThresholdDps_;
  unsigned long cooldownMs_, lastTriggerMs_;
  bool hasTriggered_;
};

