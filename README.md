# 🌱 Agriscan

ระบบ IoT สำหรับตรวจสอบค่าดินแบบ Real-time อ่านค่าจากเซ็นเซอร์ 7-in-1 (RS485 Modbus) ผ่าน ESP32 และแสดงผลบนแดชบอร์ดเว็บ — **ESP32 ส่งค่าจริงขึ้นคลาวด์ (Render.com) เก็บลง PostgreSQL** ดูได้จากทุกที่ ไม่มี mock data

## ค่าที่วัดได้

| ค่า | หน่วย | เกณฑ์อ้างอิง | แหล่งที่มา |
|-----|--------|--------------|-----------|
| ความชื้น (Moisture) | % | 30–80 (แนวทางรดน้ำ) | ไม่ใช่เกณฑ์กรมฯ — ขึ้นกับชนิดดิน |
| อุณหภูมิ (Temperature) | °C | 15–35 | ไม่มีเกณฑ์มาตรฐาน (ข้อมูลประกอบ) |
| ค่าการนำไฟฟ้า (EC) | µS/cm | < 2,000 = ไม่เค็ม | เกณฑ์ความเค็มของกรมพัฒนาที่ดิน (2 dS/m = เริ่มเค็มเล็กน้อย) |
| pH ดิน | — | 5.5–7.5 (เหมาะสม 6.0–7.0) | การจัดชั้น pH ของกรมพัฒนาที่ดิน |
| ไนโตรเจน (N) | mg/kg | ≥ 50 (ค่าบ่งชี้) | กรมฯ วัด N เป็น % (OM/Total N) — ค่าเซ็นเซอร์เป็นค่าประมาณ |
| ฟอสฟอรัส (P) | mg/kg | ต่ำ <10 · ปานกลาง 10–25 · สูง >25 | กรมพัฒนาที่ดิน (ตารางที่ 15, กองสำรวจดิน 2523) |
| โพแทสเซียม (K) | mg/kg | ต่ำ <60 · ปานกลาง 60–90 · สูง >90 | กรมพัฒนาที่ดิน (ตารางที่ 15, กองสำรวจดิน 2523) |

**แหล่งอ้างอิง:** กรมพัฒนาที่ดิน, *คู่มือการวิเคราะห์ดินทางเคมีเพื่อประเมินความอุดมสมบูรณ์ของดิน*, ตารางที่ 15 (กองสำรวจดิน, 2523) — ลิงก์ตรงของหน้าที่มีตาราง: https://e-library.ldd.go.th/library/flip/bib10134f/files/basic-html/page83.html (ถ้า root ของ flipbook เปิดไม่ได้ ให้ใช้ลิงก์ตรงนี้ หรือค้นใน Google: "bib10134f page83")

## สถาปัตยกรรม

```
esp32/agriscan/
  agriscan.ino            ← firmware (RS485 + WiFi + WebServer + ส่งข้อมูลขึ้นคลาวด์)
  dashboard.h             ← dashboard ฝังใน C++ raw string literal

dashboard/
  index.html              ← dashboard สำหรับเปิดในเบราว์เซอร์
  script.js               ← logic ดึงข้อมูล, fallback chain, คำแนะนำ
  style.css               ← ธีม dark green glassmorphism

backend/
  app.py                  ← Flask API จริง (รับค่า + เก็บ DB + เสิร์ฟ dashboard)
  requirements.txt        ← Python dependencies
```

**Data flow (ข้อมูลจริง ไม่มี mock):**

```
เซ็นเซอร์ 7-in-1 → RS485 Modbus → ESP32 → POST /api/readings (HTTPS, ทุก 3 วินาที)
                                          ↓
                              PostgreSQL (Render)
                                          ↓
              Dashboard บนเว็บ ← GET /api/latest (poll ทุก 3 วินาที)
```

## วิธี Deploy (Render.com) — ดูบนเว็บได้จริง

1. **Push โค้ดนี้ขึ้น GitHub** แล้วนำเข้าใน [Render.com](https://render.com) (New → Blueprint)
2. Render จะอ่าน `render.yaml` อัตโนมัติ สร้างให้ครบ: **Web Service + PostgreSQL (free tier)** + สุ่มค่า `API_KEY`
3. เปิด URL ที่ได้ เช่น `https://agriscan-xynf.onrender.com/` — จะเห็นหน้า dashboard
4. คัดลอกค่า **API_KEY** จาก Render Dashboard → Environment

### ตั้งค่า ESP32 ให้ส่งค่าขึ้นคลาวด์

เปิด `esp32/agriscan/agriscan.ino` แล้วแก้:

```cpp
const char* CLOUD_URL = "https://agriscan-xynf.onrender.com/api/readings";
const char* API_KEY   = "<ค่า API_KEY จาก Render — ดูที่ Environment>";
```

- ESP32 จะ POST ค่าจริงขึ้นคลาวด์ทุก 3 วินาที (`POST_INTERVAL_MS`)
- Backend ลบข้อมูลเก่ากว่า 7 วันอัตโนมัติ (ตั้งได้ผ่าน env `RETAIN_DAYS`) — กัน database เต็ม
- ยังเสิร์ฟ dashboard ให้เครือข่ายท้องถิ่นผ่าน `http://agriscan.local` ตามเดิม

## วิธีใช้งาน

### Backend (รันในเครื่อง — ใช้ SQLite อัตโนมัติ)

```bash
pip install -r backend/requirements.txt
cd backend && python app.py          # http://localhost:5000
```

- ไม่ต้องตั้งค่าใด ๆ — ไม่มี `DATABASE_URL` จะใช้ไฟล์ `backend/agriscan.db` (SQLite)
- ทดสอบส่งค่าเหมือน ESP32:
  ```bash
  curl -X POST http://localhost:5000/api/readings \
       -H "Content-Type: application/json" \
       -H "X-API-Key: agriscan-dev-key" \
       -d '{"moisture":45.2,"temperature":28.5,"ec":320,"ph":6.8,"n":45,"p":32,"k":89}'
  ```

### Dashboard (เบราว์เซอร์)

- เปิด `dashboard/index.html` ตรงๆ หรือเข้าผ่าน URL ของ Render
- Fallback chain: IP ตั้งเอง (localStorage) → URL ที่ใช้ได้ล่าสุด → page origin → **คลาวด์ Render** → agriscan.local → 192.168.1.1 → localhost:5000

### ESP32 (ของจริง)

1. เปิดไฟล์ `esp32/agriscan/agriscan.ino` ใน Arduino IDE
2. ติดตั้งไลบรารี ModbusMaster
3. แก้ไข SSID/PASSWORD และ CLOUD_URL/API_KEY
4. Flash ไปยัง ESP32

## API Endpoints

| เส้นทาง | วิธี | คำอธิบาย |
|---------|------|-----------|
| `POST /api/readings` | ESP32 | รับค่าจากเซ็นเซอร์ (ต้องมี header `X-API-Key`) |
| `GET /api/latest` | Dashboard | ค่าล่าสุดจากฐานข้อมูล |
| `GET /data` | Dashboard | alias ของ `/api/latest` |
| `GET /health` | — | ตรวจสถานะ service + ฐานข้อมูล |
| `GET /` | — | เสิร์ฟหน้า dashboard |

## เอกสารประกอบ

- `agriscan-presentation.html` — เอกสารนำเสนอโปรเจกต์ฉบับสมบูรณ์ (เปิดในเบราว์เซอร์)
- `agriscan-code-guide.html` — คู่มืออธิบายโค้ดทุกส่วน: ภาษา, สแตก, workflow, อธิบายทีละไฟล์ (เปิดในเบราว์เซอร์)

## หมายเหตุ

- **ไม่มี mock data แล้ว** — ถ้ายังไม่มีข้อมูล หน้าเว็บจะแสดงสถานะ offline จนกว่า ESP32 จะส่งค่ามา
- บน Render free tier พื้นที่ Postgres มีจำกัด (ส่งทุก 3 วินาที ≈ 28,800 แถว/วัน + ลบข้อมูลเก่าอัตโนมัติ) เหมาะสำหรับการสาธิต
- HTTPS ของ ESP32 ใช้ `setInsecure()` — เพียงพอสำหรับการพัฒนา ถ้าต้องการความปลอดภัยสูงขึ้นควรใช้ certificate pinning
