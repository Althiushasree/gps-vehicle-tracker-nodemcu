# Circuit Diagram — NEO-6M to NodeMCU

## Pin Connection Table

| NEO-6M Pin | Wire Color (suggested) | NodeMCU Pin | Description              |
|------------|------------------------|-------------|--------------------------|
| VCC        | Red                    | 3V3         | 3.3 V power (NOT 5V!)    |
| GND        | Black                  | GND         | Common ground            |
| TX         | Green                  | D6 (GPIO12) | GPS data → NodeMCU RX    |
| RX         | Yellow                 | D5 (GPIO14) | NodeMCU TX → GPS (opt.)  |

> **Warning:** Do NOT connect VCC to the 5V/VIN pin — the NEO-6M logic is 3.3 V.

---

## ASCII Wiring Diagram

```
                        ┌─────────────────────┐
  NEO-6M GPS Module     │      NodeMCU        │
  ┌──────────────┐      │   ESP8266 v1.0      │
  │              │      │                     │
  │  VCC ────────┼──────┼── 3V3               │
  │              │      │                     │
  │  GND ────────┼──────┼── GND               │
  │              │      │                     │
  │  TX  ────────┼──────┼── D6 (GPIO 12)      │  ← GPS data line
  │              │      │                     │
  │  RX  ────────┼──────┼── D5 (GPIO 14)      │  (optional)
  │              │      │                     │
  │  [Antenna]   │      │            USB ─────┼── PC / Power
  └──────────────┘      └─────────────────────┘
```

---

## NodeMCU Pin Reference

```
                    ┌──────────────┐
              RST ──┤              ├── TX (GPIO 1)
               A0 ──┤              ├── RX (GPIO 3)
               D0 ──┤              ├── D1 (GPIO 5)   SCL
               D5 ──┤  NodeMCU    ├── D2 (GPIO 4)   SDA
               D6 ──┤  ESP8266    ├── D3 (GPIO 0)   FLASH
               D7 ──┤             ├── D4 (GPIO 2)   LED
               D8 ──┤             ├── GND
              3V3 ──┤             ├── 5V
              GND ──┤             ├── VIN
                    └──────────────┘
```

- **D6 (GPIO 12)** → connected to NEO-6M **TX** (SoftwareSerial RX in code)  
- **D5 (GPIO 14)** → connected to NEO-6M **RX** (SoftwareSerial TX in code)

---

## Notes

1. The NEO-6M comes with a ceramic patch antenna soldered on. For best signal,  
   point the antenna **skyward** and operate outdoors or near a window.
2. A cold GPS fix (first lock after power-on) can take **60–120 seconds**.  
   The built-in LED on the NodeMCU blinks 3× when a valid fix is received.
3. The NEO-6M module outputs NMEA sentences at **9600 baud** by default.  
   The code uses `SoftwareSerial` so the hardware UART stays free for debugging.
