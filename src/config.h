#pragma once
#include <stddef.h>

// Safe defaults: SMS and camera triggering are disabled until intentionally configured.
// Do not add emergency-service numbers. Use only trusted contacts who have agreed to testing.
namespace SafeVelConfig {
constexpr bool SMS_ENABLED = false;
constexpr const char* TRUSTED_CONTACTS[] = {"", "", ""};
constexpr size_t TRUSTED_CONTACT_COUNT = 3;

constexpr bool CAMERA_HTTP_ENABLED = false;
constexpr const char* WIFI_SSID = "";
constexpr const char* WIFI_PASSWORD = "";
constexpr const char* CAMERA_CAPTURE_URL = "http://192.168.4.1/capture";

constexpr float IMPACT_G_THRESHOLD = 3.0f;
constexpr float HIGH_SEVERITY_G_THRESHOLD = 5.5f;
constexpr float GYRO_THRESHOLD_DPS = 280.0f;
constexpr unsigned long ALERT_COOLDOWN_MS = 60000UL;
constexpr unsigned long GPS_FIX_STALE_MS = 15000UL;
}

