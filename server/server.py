from flask import Flask, request, jsonify
import sqlite3
import os
from datetime import datetime

app = Flask(__name__)

DB_PATH = os.path.join(os.path.dirname(__file__), "gps_data.db")


def get_db():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn


def init_db():
    with get_db() as conn:
        conn.execute("""
            CREATE TABLE IF NOT EXISTS locations (
                id        INTEGER PRIMARY KEY AUTOINCREMENT,
                latitude  REAL    NOT NULL,
                longitude REAL    NOT NULL,
                speed     REAL    NOT NULL DEFAULT 0,
                timestamp TEXT    NOT NULL
            )
        """)
        conn.commit()
    print("[DB] Database initialised at:", DB_PATH)


@app.route("/update", methods=["POST"])
def update_location():
    data = request.get_json(silent=True)
    if not data:
        return jsonify({"error": "Invalid or missing JSON body"}), 400

    lat   = data.get("latitude")
    lng   = data.get("longitude")
    speed = data.get("speed", 0)

    if lat is None or lng is None:
        return jsonify({"error": "latitude and longitude are required"}), 400

    timestamp = datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S")

    with get_db() as conn:
        conn.execute(
            "INSERT INTO locations (latitude, longitude, speed, timestamp) VALUES (?, ?, ?, ?)",
            (lat, lng, speed, timestamp)
        )
        conn.commit()

    print(f"[GPS] lat={lat:.6f}  lng={lng:.6f}  speed={speed:.2f} km/h  @ {timestamp}")
    return jsonify({"status": "ok", "timestamp": timestamp}), 200


@app.route("/location", methods=["GET"])
def get_latest_location():
    with get_db() as conn:
        row = conn.execute(
            "SELECT latitude, longitude, speed, timestamp FROM locations ORDER BY id DESC LIMIT 1"
        ).fetchone()

    if row is None:
        return jsonify({"error": "No data yet"}), 404

    return jsonify({
        "latitude":  row["latitude"],
        "longitude": row["longitude"],
        "speed":     row["speed"],
        "timestamp": row["timestamp"]
    })


@app.route("/history", methods=["GET"])
def get_history():
    limit = request.args.get("limit", 50, type=int)
    with get_db() as conn:
        rows = conn.execute(
            "SELECT latitude, longitude, speed, timestamp FROM locations ORDER BY id DESC LIMIT ?",
            (limit,)
        ).fetchall()

    return jsonify([dict(r) for r in rows])


if __name__ == "__main__":
    init_db()
    print("[Server] Starting Flask GPS server on http://0.0.0.0:5000")
    app.run(host="0.0.0.0", port=5000, debug=True)
