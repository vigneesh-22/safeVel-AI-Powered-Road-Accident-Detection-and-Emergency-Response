#pragma once
#include <TinyGPS++.h>

class GpsService {
 public:
  void poll(Stream& serial) { while (serial.available()) gps_.encode(serial.read()); }
  bool hasRecentFix(unsigned long nowMs, unsigned long staleMs) const {
    return gps_.location.isValid() && gps_.location.age() <= staleMs && nowMs > 0;
  }
  String locationText(unsigned long nowMs, unsigned long staleMs) const {
    if (!hasRecentFix(nowMs, staleMs)) return "GPS unavailable";
    return String(gps_.location.lat(), 6) + "," + String(gps_.location.lng(), 6);
  }
  String mapsUrl(unsigned long nowMs, unsigned long staleMs) const {
    if (!hasRecentFix(nowMs, staleMs)) return "";
    return "https://maps.google.com/?q=" + locationText(nowMs, staleMs);
  }
 private:
  TinyGPSPlus gps_;
};

