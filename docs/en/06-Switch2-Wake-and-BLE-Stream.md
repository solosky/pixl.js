# Switch 2 Wake & BLE Screen Streaming Guide

This firmware adds two BLE-based apps to Pixl.js (compatible with both **OLED** and **LCD** 128×64 boards): **Switch2 Wake** and **BLE Stream**.

---

## 1. Switch2 Wake

### Overview
Nintendo Switch 2 listens in sleep mode for BLE advertising packets from a **previously paired Joy-Con 2 / Pro Controller 2** (Nintendo Company ID `0x0553`, wake flag `0x81`). On nRF52832 (SoftDevice S112), this app provides:
- **One-click Switch 2 Wake**: Temporarily spoofs the paired controller's Public MAC via `sd_ble_gap_addr_set()`, broadcasts the 31-byte Switch 2 wake packet at +4 dBm TX power, and automatically restores Pixl.js's native MAC afterward.
- **On-device BLE Wake Packet Scanner**: Uses the Nordic SoftDevice S112 Radio Timeslot API to passively sniff BLE advertising channels 37/38/39 directly on `NRF_RADIO`, automatically capturing the `0x0553` + `0x81` packet when you press Home on a paired Joy-Con 2 / Pro 2—no phone or PC required.
- **Boot Wake (`auto_wake`)**: Optionally broadcasts a 2-second wake packet automatically whenever Pixl.js wakes up from sleep via button press (skipped on silent NFC-field wakeups).
- **Broadcast Modes**: Supports both `NonConn` (default, non-connectable undirected) and `Conn` (connectable undirected, auto-disconnects on peer connect).

### Quick Start
1. Open **Switch2 Wake** from the desktop.
2. Select **Scan Wake Pkt** (prompts `Press JC2 Home` for 15 seconds).
3. Bring a **Joy-Con 2** or **Pro Controller 2** paired with your Switch 2 close to Pixl.js and press its **Home** button.
4. Once `Captured!` appears, the controller MAC, Switch 2 host MAC, PID, and raw 31-byte payload are saved automatically to `/switch2_wake.txt`.
5. Put Switch 2 to sleep and select **Wake Switch 2** (or enable **Boot Wake** to wake Switch 2 whenever you wake Pixl.js).

### Configuration File (`/switch2_wake.txt`)
You can also create or edit `/switch2_wake.txt` in the root directory via BLE File Transfer:

```ini
# Option 1: Controller MAC + Switch 2 Host MAC (auto-builds 31B packet)
mac=AA:BB:CC:DD:EE:FF
host=11:22:33:44:55:66
# Controller PID: 2066=JC2-L, 2067=JC2-R, 2069=Pro2
pid=2066
# Advertising mode: nonconn or conn
mode=nonconn
# Boot wake: 1=wake on boot, 0=disabled
auto_wake=0

# Option 2: Raw 31-byte hex payload (overrides host/pid if present)
#data=0201061BFF53050100037E0566200001816655443322110F00000000000000
```

---

## 2. BLE Stream (128×64 Screen Streaming)

### Overview
Streams real-time monochrome 128×64 video frames from a PC to Pixl.js (OLED or LCD) over BLE Nordic UART Service (NUS, 250B MTU, 10–20 ms connection interval) with PackBits RLE and inter-frame XOR Delta + RLE compression.

### Frame Protocol (5-byte header + payload)
| Offset | Field | Description |
| :--- | :--- | :--- |
| `0..1` | Magic | `0xA5 0x5A` |
| `2` | Flags | `0x00`=Raw 1024B XBM, `0x01`=PackBits RLE, `0x02`=XOR Delta + RLE |
| `3..4` | Length | Payload length (uint16 little-endian, ≤ 1024) |
| `5..` | Payload | 128×64 monochrome frame (LSB-first horizontal XBM) |

### On-device Controls
- **Left**: Toggle top debug HUD (`FPS / RSSI(dBm) / Frame Bytes`)
- **Right**: Toggle black/white inversion
- **Center**: Exit to desktop
- **Auto-reconnect**: When the PC disconnects or finishes playback, Pixl.js clears the last frame and returns to the waiting screen while advertising for the next connection.

### PC Companion Script
Use [`fw/scripts/stream_video_to_pixl.py`](../../fw/scripts/stream_video_to_pixl.py) (requires Python `bleak` and `ffmpeg`):

```bash
pip install bleak
# Built-in animation demo
python fw/scripts/stream_video_to_pixl.py

# Stream a video file (e.g. Bad Apple!!)
python fw/scripts/stream_video_to_pixl.py path/to/video.mp4 --fps 15 --scale stretch
```

Live terminal hotkeys while streaming (interactive hotkeys are supported on Windows; press `Ctrl+C` to quit on macOS/Linux):
- `[` / `]`: Decrease / increase target FPS (5–30)
- `s`: Cycle scale mode (`fit` / `stretch` / `crop`)
- `-` / `=`: Adjust B/W threshold (10–245)
- `i`: Invert | `p`: Pause/Resume | `r`: Restart | `q`: Quit
