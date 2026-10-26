/* Optional, separate ESP32-CAM (AI Thinker) evidence node.
 * It is a network peer, not a device wired into the main ESP32's camera bus.
 * /capture saves a JPEG to this board's microSD and returns a JSON-like status.
 */
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SD_MMC.h>

const char* WIFI_SSID = "CHANGE_ME";
const char* WIFI_PASSWORD = "CHANGE_ME";
WebServer server(80);
static unsigned long imageNumber = 0;

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

void capture() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) { server.send(500, "application/json", "{\"ok\":false,\"error\":\"camera_capture\"}"); return; }
  const String filename = "/safevel_" + String(++imageNumber) + ".jpg";
  File file = SD_MMC.open(filename, FILE_WRITE);
  bool ok = file && file.write(fb->buf, fb->len) == fb->len;
  if (file) file.close();
  esp_camera_fb_return(fb);
  server.send(ok ? 200 : 500, "application/json", ok ? "{\"ok\":true,\"file\":\"" + filename + "\"}" : "{\"ok\":false,\"error\":\"sd_write\"}");
}

void setup() {
  Serial.begin(115200);
  camera_config_t c{};
  c.ledc_channel = LEDC_CHANNEL_0; c.ledc_timer = LEDC_TIMER_0;
  c.pin_d0=Y2_GPIO_NUM; c.pin_d1=Y3_GPIO_NUM; c.pin_d2=Y4_GPIO_NUM; c.pin_d3=Y5_GPIO_NUM;
  c.pin_d4=Y6_GPIO_NUM; c.pin_d5=Y7_GPIO_NUM; c.pin_d6=Y8_GPIO_NUM; c.pin_d7=Y9_GPIO_NUM;
  c.pin_xclk=XCLK_GPIO_NUM; c.pin_pclk=PCLK_GPIO_NUM; c.pin_vsync=VSYNC_GPIO_NUM; c.pin_href=HREF_GPIO_NUM;
  c.pin_sccb_sda=SIOD_GPIO_NUM; c.pin_sccb_scl=SIOC_GPIO_NUM; c.pin_pwdn=PWDN_GPIO_NUM; c.pin_reset=RESET_GPIO_NUM;
  c.xclk_freq_hz=20000000; c.pixel_format=PIXFORMAT_JPEG; c.frame_size=FRAMESIZE_VGA; c.jpeg_quality=12; c.fb_count=1;
  if (esp_camera_init(&c) != ESP_OK) { Serial.println("Camera init failed"); return; }
  if (!SD_MMC.begin("/sdcard", true)) Serial.println("SD_MMC unavailable; capture will fail");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  server.on("/capture", HTTP_GET, capture);
  server.on("/health", HTTP_GET, [](){ server.send(200, "text/plain", "safevel-camera-ok"); });
  server.begin();
  Serial.println(WiFi.localIP());
}
void loop() { server.handleClient(); }

