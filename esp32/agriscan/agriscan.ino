/**
 * Agriscan — RS485 Soil Sensor 7-in-1 + WiFi + WebServer
 * GET /data  →  JSON สำหรับแดชบอร์ด
 */

#include <ModbusMaster.h>
#include <WiFi.h>
#include <WebServer.h>      // ← เพิ่ม
#include <ESPmDNS.h>        // ← เพิ่ม mDNS
#include <HTTPClient.h>     // ← ส่งข้อมูลขึ้นคลาวด์
#include <WiFiClientSecure.h>
#include "dashboard.h"      // ← หน้าเว็บ HTML
#include "secrets.h"        // ← WiFi + API key (ไฟล์นี้ไม่ขึ้น Git)

// ── WiFi ──────────────────────────────────────────────────
// SSID/รหัส WiFi อยู่ใน secrets.h (ไฟล์นี้ไม่ขึ้น Git — กันข้อมูลหลุด)
// ถ้าต้องการแก้ WiFi ให้แก้ที่ secrets.h แล้ว re-flash

// ── Cloud Upload ───────────────────────────────────────────
// URL ของ backend บน Render
const char* CLOUD_URL = "https://agriscan-xynf.onrender.com/api/readings";
// API key อยู่ใน secrets.h (ดูจาก Render Dashboard → Environment → API_KEY)
// ความถี่ส่งข้อมูลขึ้นคลาวด์ (มิลลิวินาที) — 3,000 = ทุก 3 วินาที
#define POST_INTERVAL_MS  3000

// ── RS485 ─────────────────────────────────────────────────
#define RXD2        16
#define TXD2        17
#define DE_RE_PIN   4       // pin ต่อ DE+RE รวมกันของ MAX485 (ไม่มีพินนี้ลบได้)
#define BAUD_RATE   4800
#define MODBUS_ID   1

ModbusMaster node;
WebServer server(80);       // ← Web Server port 80

// ── Register map ──────────────────────────────────────────
#define REG_MOISTURE     0x0000
#define REG_TEMPERATURE  0x0001
#define REG_EC           0x0002
#define REG_PH           0x0003
#define REG_NITROGEN     0x0004
#define REG_PHOSPHORUS   0x0005
#define REG_POTASSIUM    0x0006
#define REG_COUNT        7

// ── Struct ────────────────────────────────────────────────
struct SoilData {
  float moisture;
  float temperature;
  int   ec;
  float ph;
  int   nitrogen;
  int   phosphorus;
  int   potassium;
  bool  valid;
};

SoilData lastData = {0, 0, 0, 0, 0, 0, 0, false};

// ── WiFi Connect ──────────────────────────────────────────
void connectWiFi() {
  Serial.printf("กำลังเชื่อมต่อ WiFi: %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  uint8_t retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.printf("✅ WiFi เชื่อมต่อสำเร็จ! IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println();
    Serial.println("⚠ WiFi เชื่อมต่อไม่ได้ — ทำงานต่อแบบ offline");
  }
}

// ── Read Sensor ───────────────────────────────────────────
SoilData readSensor() {
  SoilData d = {0, 0, 0, 0, 0, 0, 0, false};
  uint8_t result = node.readHoldingRegisters(REG_MOISTURE, REG_COUNT);
  Serial.printf("[Modbus] result=%d\n", result);

  if (result == node.ku8MBSuccess) {
    d.moisture    = node.getResponseBuffer(0) / 10.0;
    d.temperature = (int16_t)node.getResponseBuffer(1) / 10.0;
    d.ec          = node.getResponseBuffer(2);
    d.ph          = node.getResponseBuffer(3) / 10.0;
    d.nitrogen    = node.getResponseBuffer(4);
    d.phosphorus  = node.getResponseBuffer(5);
    d.potassium   = node.getResponseBuffer(6);
    d.valid       = true;
  }
  return d;
}

// ── Upload to Cloud (POST /api/readings) ──────────────────
void uploadReading() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Cloud] WiFi ไม่เชื่อมต่อ — ข้ามการส่ง");
    return;
  }
  if (!lastData.valid) {
    Serial.println("[Cloud] ยังไม่มีค่าจากเซ็นเซอร์ — ข้ามการส่ง");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();   // ไม่ตรวจ certificate (เพียงพอสำหรับการทดลอง/พัฒนา)

  HTTPClient http;
  if (!http.begin(client, CLOUD_URL)) {
    Serial.println("[Cloud] เริ่ม HTTP ไม่สำเร็จ");
    return;
  }

  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", API_KEY);

  String body = "{";
  body += "\"moisture\":"    + String(lastData.moisture, 1)    + ",";
  body += "\"temperature\":" + String(lastData.temperature, 1) + ",";
  body += "\"ec\":"          + String(lastData.ec)             + ",";
  body += "\"ph\":"          + String(lastData.ph, 1)          + ",";
  body += "\"n\":"           + String(lastData.nitrogen)       + ",";
  body += "\"p\":"           + String(lastData.phosphorus)     + ",";
  body += "\"k\":"           + String(lastData.potassium)      + ",";
  body += "\"valid\":"       + String(lastData.valid ? "true" : "false");
  body += "}";

  int code = http.POST(body);
  if (code > 0) {
    Serial.printf("[Cloud] POST %d → %s\n", code, http.getString().c_str());
  } else {
    Serial.printf("[Cloud] ส่งล้มเหลว (%d)\n", code);
  }
  http.end();
}

// ── HTTP Handler: GET /data ───────────────────────────────
void handleData() {
  // CORS headers — สำคัญมาก ให้เบราว์เซอร์ดึงได้
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");

  String json = "{";
  json += "\"moisture\":"    + String(lastData.moisture, 1)    + ",";
  json += "\"temperature\":" + String(lastData.temperature, 1) + ",";
  json += "\"ec\":"          + String(lastData.ec)             + ",";
  json += "\"ph\":"          + String(lastData.ph, 1)          + ",";
  json += "\"n\":"           + String(lastData.nitrogen)       + ",";
  json += "\"p\":"           + String(lastData.phosphorus)     + ",";
  json += "\"k\":"           + String(lastData.potassium)      + ",";
  json += "\"valid\":"       + String(lastData.valid ? "true" : "false");
  json += "}";

  server.send(200, "application/json", json);
  Serial.println("[HTTP] GET /data → ส่ง JSON สำเร็จ");
}

// ── HTTP Handler: OPTIONS (preflight) ────────────────────
void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
}

// ── Print to Serial ───────────────────────────────────────
void printSerial(const SoilData& d) {
  Serial.println("┌────────────────────────────────┐");
  Serial.printf( "│ ความชื้น    : %6.1f %%\n", d.moisture);
  Serial.printf( "│ อุณหภูมิ    : %6.1f °C\n", d.temperature);
  Serial.printf( "│ EC          : %6d µS/cm\n", d.ec);
  Serial.printf( "│ pH          : %6.1f\n", d.ph);
  Serial.printf( "│ N           : %6d mg/kg\n", d.nitrogen);
  Serial.printf( "│ P           : %6d mg/kg\n", d.phosphorus);
  Serial.printf( "│ K           : %6d mg/kg\n", d.potassium);
  Serial.printf( "│ WiFi        : %s\n",
    WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString().c_str() : "ไม่ได้เชื่อมต่อ");
  Serial.println("└────────────────────────────────┘");

  if      (d.moisture < 30) Serial.println(">> [แจ้งเตือน] ดินแห้ง — ควรรดน้ำ");
  else if (d.moisture > 80) Serial.println(">> [แจ้งเตือน] ดินชื้นเกินไป");
  else                      Serial.println(">> [OK] ความชื้นปกติ");
}

// ── Reconnect WiFi ────────────────────────────────────────
void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi หลุด — กำลัง reconnect...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    uint8_t retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 10) {
      delay(500);
      retry++;
    }
    if (WiFi.status() == WL_CONNECTED)
      Serial.printf("✅ Reconnect สำเร็จ! IP: %s\n", WiFi.localIP().toString().c_str());
  }
}

// ── RS485 DE/RE control ────────────────────────────────
void preTransmission()  { digitalWrite(DE_RE_PIN, HIGH); }
void postTransmission() { digitalWrite(DE_RE_PIN, LOW);  }

// ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== Agriscan + WiFi + WebServer ===");

  // เชื่อม WiFi
  WiFi.mode(WIFI_STA);
  connectWiFi();

  // ─── ตั้งชื่อ Local (mDNS) ───────────────────────────
  if (MDNS.begin("agriscan")) {
    Serial.println("✅ mDNS ทำงาน! สามารถเข้าผ่าน http://agriscan.local ได้");
  }

  // ─── ตั้ง Web Server ─────────────────────────────────
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", dashboard_html);
  });
  server.on("/data", HTTP_GET,     handleData);
  server.on("/data", HTTP_OPTIONS, handleOptions);
  server.onNotFound([]() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(404, "text/plain", "Not found");
  });
  server.begin();
  Serial.println("🌐 Web Server เริ่มต้นแล้ว");
  Serial.printf("📡 Dashboard IP: http://%s\n", WiFi.localIP().toString().c_str());
  Serial.println("📡 หรือเข้าผ่านชื่อ: http://agriscan.local\n");

  // เริ่ม RS485
  Serial2.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, LOW);
  node.begin(MODBUS_ID, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  delay(300);
  Serial.println("พร้อมอ่านค่าเซ็นเซอร์...\n");
}

void loop() {
  server.handleClient();   // ← ต้องเรียกทุก loop เพื่อรับ HTTP request

  checkWiFi();

  // อ่านค่า sensor ทุก 3 วินาที
  static unsigned long lastRead = 0;
  if (millis() - lastRead >= 3000) {
    lastRead = millis();

    SoilData d = readSensor();
    if (d.valid) {
      lastData = d;
      printSerial(d);
    } else {
      Serial.println("[ERROR] Modbus fail — ใช้ค่าเดิม");
    }
    Serial.println();
  }

  // ส่งค่าจริงขึ้นคลาวด์ทุก POST_INTERVAL_MS (3 วินาที)
  static unsigned long lastUpload = 0;
  if (millis() - lastUpload >= POST_INTERVAL_MS) {
    lastUpload = millis();
    uploadReading();
  }
}
