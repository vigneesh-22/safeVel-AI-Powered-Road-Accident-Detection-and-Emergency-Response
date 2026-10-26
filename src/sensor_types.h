#pragma once

struct ImuSample {
  float axG;
  float ayG;
  float azG;
  float gxDps;
  float gyDps;
  float gzDps;
  unsigned long capturedAtMs;
};

enum class Severity : unsigned char { None, Low, High };

struct DetectionResult {
  bool triggered;
  Severity severity;
  float impactG;
  float peakGyroDps;
};

