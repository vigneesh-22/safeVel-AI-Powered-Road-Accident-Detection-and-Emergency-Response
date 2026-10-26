# Test plan

Perform tests away from traffic, with consent from every SMS recipient. Keep `SMS_ENABLED = false` until the final controlled SMS check.

| Test | Method | Expected result |
|---|---|---|
| Build | Run `pio run` | Main ESP32 firmware compiles. |
| Logic unit test | Run `powershell -ExecutionPolicy Bypass -File tests/run_host_tests.ps1` | `detection tests passed`. |
| IMU presence | Open 115200 serial monitor after boot | IMU initializes; gentle motion changes debug when instrumented. |
| GPS | Give NEO-6M outdoor sky view | A fresh coordinate appears in logged event. |
| SD logging | Trigger SOS once | `/safevel_events.csv` is created on main SD card. |
| Buzzer/SOS | Press SOS for one loop cycle | One low-severity local event, then cooldown. |
| GSM dry run | Keep SMS disabled; inspect serial/power | No outbound message; SIM800L stays stable during bursts. |
| Trusted SMS | Put only a consenting test number in config, enable SMS, press SOS | Exactly that contact receives a message beginning `SAFEVEL TEST/PROTOTYPE ALERT`. |
| Camera node | Browse `http://CAMERA_IP/health`, then `/capture` | `health` returns OK and camera SD receives a JPEG. |
| Camera bridge | Enable Wi-Fi/camera with the correct IP, then SOS | Main event CSV records the camera HTTP response. |

Do not simulate an impact by throwing the assembled unit or test during driving. Use a fixture, low-energy motion, or temporarily lower thresholds for bench testing. Tune thresholds with recorded data; this heuristic is not a certified crash detector.

