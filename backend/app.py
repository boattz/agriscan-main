"""
Agriscan — Backend API (ข้อมูลจริงจาก ESP32)

รับค่าเซ็นเซอร์จริงจาก ESP32 เก็บลง PostgreSQL (หรือ SQLite เมื่อรันในเครื่อง)
และเสิร์ฟ dashboard + ค่าล่าสุดให้เว็บ

Endpoints:
  POST /api/readings  ← ESP32 ส่งค่า (ต้องมี header X-API-Key)
  GET  /api/latest    → ค่าล่าสุดจากฐานข้อมูล
  GET  /data          → alias ของ /api/latest (dashboard เดิมใช้เส้นนี้)
  GET  /health        → ตรวจสถานะ service + ฐานข้อมูล
  GET  /              → เสิร์ฟหน้า dashboard
"""

import os
import sqlite3
from datetime import datetime

from flask import Flask, jsonify, request, send_from_directory
from flask_cors import CORS

# ── Config ──────────────────────────────────────────────────
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DASHBOARD_DIR = os.path.join(BASE_DIR, "..", "dashboard")

# API key สำหรับกันคนอื่นส่งข้อมูลปลอม — ตั้งผ่าน env บน Render
API_KEY = os.environ.get("API_KEY", "agriscan-dev-key")

# ถ้าตั้ง DATABASE_URL (บน Render) จะใช้ PostgreSQL
# ถ้าไม่ตั้ง (รันในเครื่อง) จะใช้ SQLite file อัตโนมัติ
DATABASE_URL = os.environ.get("DATABASE_URL", "")

# เก็บข้อมูลกี่วันแล้วลบทิ้งอัตโนมัติ (กัน database เต็ม) — ตั้งผ่าน env RETAIN_DAYS
RETAIN_DAYS = int(os.environ.get("RETAIN_DAYS", "7"))

# ทำงาน cleanup ทุกกี่ครั้งที่รับค่า (ไม่ต้องลบทุก insert — ประหยัด resource)
CLEANUP_EVERY = 50
_cleanup_counter = 0

app = Flask(__name__)
CORS(app)

# ── Database ────────────────────────────────────────────────
if DATABASE_URL:
    import psycopg
    from psycopg.rows import dict_row

    SCHEMA = """
    CREATE TABLE IF NOT EXISTS readings (
        id         SERIAL PRIMARY KEY,
        moisture   REAL,
        temperature REAL,
        ec         INTEGER,
        ph         REAL,
        n          INTEGER,
        p          INTEGER,
        k          INTEGER,
        valid      BOOLEAN DEFAULT TRUE,
        created_at TIMESTAMPTZ DEFAULT NOW()
    );
    """
    PARAM = "%s"
else:
    SCHEMA = """
    CREATE TABLE IF NOT EXISTS readings (
        id         INTEGER PRIMARY KEY AUTOINCREMENT,
        moisture   REAL,
        temperature REAL,
        ec         INTEGER,
        ph         REAL,
        n          INTEGER,
        p          INTEGER,
        k          INTEGER,
        valid      INTEGER DEFAULT 1,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );
    """
    PARAM = "?"


def get_conn():
    """คืน connection — PostgreSQL เมื่อมี DATABASE_URL, ไม่เช่นนั้น SQLite"""
    if DATABASE_URL:
        return psycopg.connect(DATABASE_URL, row_factory=dict_row)
    conn = sqlite3.connect(os.path.join(BASE_DIR, "agriscan.db"))
    conn.row_factory = sqlite3.Row
    return conn


def init_db():
    try:
        with get_conn() as conn:
            conn.execute(SCHEMA)
            conn.commit()
        print("[OK] Database พร้อมใช้งาน" + (" (PostgreSQL)" if DATABASE_URL else " (SQLite local)"))
    except Exception as e:
        print("[WARN] Database init ล้มเหลว:", e)


def cleanup_old_readings():
    """ลบข้อมูลที่เก่ากว่า RETAIN_DAYS วันออก — กัน database เต็ม"""
    try:
        with get_conn() as conn:
            if DATABASE_URL:
                conn.execute(
                    "DELETE FROM readings WHERE created_at < NOW() - INTERVAL '%s days'"
                    % RETAIN_DAYS
                )
            else:
                conn.execute(
                    "DELETE FROM readings WHERE created_at < datetime('now', '-%d days')"
                    % RETAIN_DAYS
                )
            conn.commit()
        print(f"[OK] Cleanup: ลบข้อมูลเก่ากว่า {RETAIN_DAYS} วันแล้ว")
    except Exception as e:
        print("[WARN] Cleanup ล้มเหลว:", e)


def row_to_json(row):
    ts = row["created_at"]
    return {
        "moisture":    float(row["moisture"]) if row["moisture"] is not None else None,
        "temperature": float(row["temperature"]) if row["temperature"] is not None else None,
        "ec":          row["ec"],
        "ph":          float(row["ph"]) if row["ph"] is not None else None,
        "n":           row["n"],
        "p":           row["p"],
        "k":           row["k"],
        "valid":       bool(row["valid"]),
        # PostgreSQL คืน datetime ส่วน SQLite คืน string
        "timestamp":   ts.isoformat() if isinstance(ts, datetime) else ts,
    }


# ── Routes ──────────────────────────────────────────────────
@app.route("/", methods=["GET"])
def index():
    # เสิร์ฟ dashboard (poll /data บน origin เดียวกัน)
    return send_from_directory(DASHBOARD_DIR, "index.html")


@app.route("/<path:path>", methods=["GET"])
def static_files(path):
    # assets: style.css, script.js ฯลฯ
    return send_from_directory(DASHBOARD_DIR, path)


@app.route("/health", methods=["GET"])
def health():
    db_ok = False
    try:
        with get_conn() as conn:
            conn.execute("SELECT 1")
        db_ok = True
    except Exception:
        pass
    return jsonify({"status": "healthy", "db": "connected" if db_ok else "unavailable"})


@app.route("/api/readings", methods=["POST"])
def add_reading():
    """ESP32 ส่งค่าเซ็นเซอร์จริงมาบันทึกลงฐานข้อมูล"""
    if request.headers.get("X-API-Key") != API_KEY:
        return jsonify({"error": "unauthorized — X-API-Key ไม่ถูกต้อง"}), 401

    data = request.get_json(silent=True)
    if not isinstance(data, dict):
        return jsonify({"error": "ต้องส่ง JSON body"}), 400

    fields = ["moisture", "temperature", "ec", "ph", "n", "p", "k"]
    row = {f: data.get(f) for f in fields}

    # ตรวจสอบขั้นต่ำ — ต้องมี moisture เสมอ
    if row["moisture"] is None:
        return jsonify({"error": "missing required field: moisture"}), 400

    valid = bool(data.get("valid", True))
    placeholders = ", ".join([PARAM] * (len(fields) + 1))
    sql = (
        "INSERT INTO readings (moisture, temperature, ec, ph, n, p, k, valid) "
        f"VALUES ({placeholders})"
    )

    try:
        with get_conn() as conn:
            conn.execute(sql, (*[row[f] for f in fields], valid))
            conn.commit()
    except Exception as e:
        print("⚠ Insert ล้มเหลว:", e)
        return jsonify({"error": "database error"}), 500

    # ลบข้อมูลเก่าเป็นระยะ ๆ (ทุก CLEANUP_EVERY ครั้งที่รับค่า) — กัน DB เต็ม
    global _cleanup_counter
    _cleanup_counter += 1
    if _cleanup_counter % CLEANUP_EVERY == 0:
        cleanup_old_readings()

    return jsonify({"success": True}), 201


@app.route("/api/latest", methods=["GET"])
def latest():
    """ค่าล่าสุดจากฐานข้อมูล (ข้อมูลจริงจาก ESP32)"""
    try:
        with get_conn() as conn:
            cur = conn.execute(
                "SELECT moisture, temperature, ec, ph, n, p, k, valid, created_at "
                "FROM readings ORDER BY id DESC LIMIT 1"
            )
            row = cur.fetchone()
    except Exception as e:
        print("⚠ Query ล้มเหลว:", e)
        return jsonify({"error": "database unavailable"}), 503

    if row is None:
        return jsonify({"error": "ยังไม่มีข้อมูล — รอ ESP32 ส่งค่ามาก่อน"}), 404

    return jsonify(row_to_json(row))


@app.route("/data", methods=["GET"])
def data():
    """alias ของ /api/latest — ใช้กับ dashboard เดิมที่ไม่ต้องแก้เส้นทาง"""
    return latest()


init_db()
cleanup_old_readings()  # ลบทิ้งข้อมูลเก่าครั้งแรกตอน service เริ่ม

if __name__ == "__main__":
    port = int(os.environ.get("PORT", 5000))
    app.run(host="0.0.0.0", port=port, debug=True)
