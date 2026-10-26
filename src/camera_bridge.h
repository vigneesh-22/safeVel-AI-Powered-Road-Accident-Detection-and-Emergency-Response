#pragma once
#include <HTTPClient.h>
#include "config.h"

class CameraBridge {
 public:
  String capture() {
    if (!SafeVelConfig::CAMERA_HTTP_ENABLED) return "disabled";
    HTTPClient request;
    if (!request.begin(SafeVelConfig::CAMERA_CAPTURE_URL)) return "camera_url_invalid";
    const int code = request.GET();
    const String result = code == 200 ? request.getString() : "camera_http_" + String(code);
    request.end();
    return result;
  }
};

