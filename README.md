# GPS Vehicle Tracker — NodeMCU + NEO-6M

A real-time GPS vehicle tracking system built with a **NodeMCU ESP8266**, a **NEO-6M GPS module**, a **Python Flask server**, and a **Leaflet.js live map dashboard** — no paid API keys required.

---

## Features

- Real-time latitude, longitude, and speed tracking
- Live map dashboard with auto-refresh every 5 seconds
- Data stored in SQLite (zero setup)
- LED blinks on NodeMCU when GPS fix is acquired
- Works on any local network

---

## Components Needed

| Component | Quantity |
|---|---|
| NodeMCU ESP8266 (v1.0 / v2) | 1 |
| NEO-6M GPS Module (with antenna) | 1 |
| Male-to-female jumper wires | 4 |
| USB Mini-B cable (for NodeMCU) | 1 |
| Computer running Python 3.8+ | 1 |

---

## Circuit Wiring

| NEO-6M Pin | NodeMCU Pin | Description |
|---|---|---|
| VCC | 3V3 | 3.3 V power supply |
| GND | GND | Ground |
| TX  | D6 (GPIO 12) | GPS transmit → NodeMCU receive |
| RX  | D5 (GPIO 14) | GPS receive ← NodeMCU transmit |

> **Note:** The NEO-6M TX → NodeMCU D6 is the critical data line. RX is optional for most tracking use-cases.

See [`circuit.md`](circuit.md) for the ASCII wiring diagram.

---

## Arduino Libraries Required

Install these via **Arduino IDE → Sketch → Include Library → Manage Libraries**:

| Library | Author |
|---|---|
| TinyGPS++ | Mikal Hart |
| ESP8266WiFi | ESP8266 Community |
| ESP8266HTTPClient | ESP8266 Community |
| SoftwareSerial | Arduino / ESP8266 Community |

**Board:** Add the ESP8266 board package:  
Tools → Board Manager → search `esp8266` → install **ESP8266 by ESP8266 Community**

---

## Server Setup

### 1. Install Python dependencies

```bash
pip install -r requirements.txt
```

### 2. Run the Flask server

```bash
cd server
python server.py
```

The server starts on `http://0.0.0.0:5000`.

- `POST /update` — NodeMCU sends GPS data here
- `GET  /location` — Returns latest coordinates as JSON
- `GET  /history?limit=50` — Returns last N coordinates

### 3. Open the dashboard

Open `dashboard/index.html` in your browser.  
The dashboard fetches `/location` every 5 seconds — make sure the server is running.

> If the server is on a different machine, change `fetch('/location')` in `index.html`  
> to `fetch('http://YOUR_SERVER_IP:5000/location')`.

---

## NodeMCU Setup

### 1. Edit `main.ino`

Update these three lines:

```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://YOUR_SERVER_IP:5000/update";
```

`YOUR_SERVER_IP` is the local IP of the machine running `server.py`  
(find it with `ipconfig` on Windows or `ifconfig` / `ip a` on Linux/Mac).

### 2. Upload to NodeMCU

1. Open `main.ino` in Arduino IDE
2. Select **Tools → Board → NodeMCU 1.0 (ESP-12E Module)**
3. Select the correct **COM port**
4. Click **Upload**
5. Open Serial Monitor at **115200 baud** to see GPS output

---

## Expected Serial Monitor Output

```
Connecting to WiFi: MyWiFi
....
WiFi connected!
IP address: 192.168.1.105
Waiting for GPS fix...
Waiting for GPS fix...
LAT: 12.971599
LNG: 77.594566
SPEED: 0.12 km/h
HTTP POST response: 200
Data sent successfully.
```

---

## Project Structure

```
gps-vehicle-tracker-nodemcu/
├── main.ino               # NodeMCU Arduino sketch
├── server/
│   └── server.py          # Flask API + SQLite backend
├── dashboard/
│   └── index.html         # Leaflet.js live map
├── circuit.md             # Wiring diagram
├── requirements.txt       # Python dependencies
├── .gitignore
└── README.md
```

---

## Troubleshooting

| Problem | Fix |
|---|---|
| GPS stuck on "Waiting for fix" | Take the module outdoors or near a window; cold start can take 1–2 minutes |
| HTTP POST fails | Check `serverUrl` IP, ensure server is running and firewall allows port 5000 |
| Board not detected | Install CH340 / CP2102 driver for your OS |
| `gps.location.isValid()` always false | Check TX/RX wiring; confirm NEO-6M baud is 9600 |

---

## License

MIT
