#pragma once
#include "config.h"

class SmsAlert {
 public:
  explicit SmsAlert(Stream& modem) : modem_(modem) {}
  bool begin() {
    sendCommand("AT");
    sendCommand("AT+CMGF=1");
    return true; // Most SIM800L variants do not provide a dependable immediate acknowledgement.
  }
  void alertTrustedContacts(Severity severity, const String& location, const String& mapsUrl) {
    if (!SafeVelConfig::SMS_ENABLED) return;
    const String body = "SAFEVEL TEST/PROTOTYPE ALERT: " +
        String(severity == Severity::High ? "HIGH" : "LOW") +
        " impact detected. Location: " + location + (mapsUrl.length() ? " " + mapsUrl : "");
    for (size_t i = 0; i < SafeVelConfig::TRUSTED_CONTACT_COUNT; ++i) {
      const char* number = SafeVelConfig::TRUSTED_CONTACTS[i];
      if (isTrustedNumber(number)) sendSms(number, body);
    }
  }
 private:
  bool isTrustedNumber(const char* value) const { return value && value[0] == '+'; }
  void sendCommand(const char* command) { modem_.print(command); modem_.print("\r"); delay(250); }
  void sendSms(const char* number, const String& body) {
    modem_.print("AT+CMGS=\""); modem_.print(number); modem_.print("\"\r"); delay(250);
    modem_.print(body); modem_.write(26); delay(4000);
  }
  Stream& modem_;
};

