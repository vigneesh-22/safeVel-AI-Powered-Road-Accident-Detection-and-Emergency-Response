#pragma once
#include <SD.h>
#include <SPI.h>
#include "sensor_types.h"

class SdLogger {
 public:
  bool begin(int chipSelect) { ready_ = SD.begin(chipSelect); return ready_; }
  bool logEvent(const DetectionResult& event, const String& location, const String& cameraStatus) {
    if (!ready_) return false;
    File file = SD.open("/safevel_events.csv", FILE_APPEND);
    if (!file) return false;
    file.printf("%lu,%s,%.2f,%.2f,%s,%s\n", millis(),
                event.severity == Severity::High ? "high" : "low", event.impactG,
                event.peakGyroDps, location.c_str(), cameraStatus.c_str());
    file.close();
    return true;
  }
 private:
  bool ready_ = false;
};

