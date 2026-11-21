import argparse
import asyncio
import math
import struct
import subprocess
import sys
import time
from bleak import BleakClient, BleakScanner

try:
    import msvcrt
except ImportError:
    msvcrt = None

NUS_RX_CHAR_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
NUS_TX_CHAR_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
MAGIC = b"\xA5\x5A"
FLAG_RAW = 0x00
FLAG_RLE = 0x01
FLAG_DELTA_RLE = 0x02

SCALE_MODES = ["fit", "stretch", "crop"]


def packbits_rle(data: bytes) -> bytes:
    out = bytearray()
    n = len(data)
    i = 0
    while i < n:
        run_len = 1
        while i + run_len < n and data[i + run_len] == data[i] and run_len < 129:
            run_len += 1
        if run_len >= 2:
            out.append(128 + (run_len - 2))
            out.append(data[i])
            i += run_len
        else:
            lit_start = i
            i += 1
            while i < n and (i - lit_start) < 128:
                if i + 1 < n and data[i] == data[i + 1]:
                    break
                i += 1
            lit_len = i - lit_start
            out.append(lit_len - 1)
            out.extend(data[lit_start:i])
    return bytes(out)


def encode_frame_packet(xbm_1024: bytes, prev_xbm: bytes | None = None, force_keyframe: bool = False) -> bytes:
    key_rle = packbits_rle(xbm_1024)
    if prev_xbm is not None and not force_keyframe:
        xor_bytes = bytes(a ^ b for a, b in zip(prev_xbm, xbm_1024))
        delta_rle = packbits_rle(xor_bytes)
        if len(delta_rle) < len(key_rle) and len(delta_rle) < len(xbm_1024):
            return MAGIC + struct.pack("<BH", FLAG_DELTA_RLE, len(delta_rle)) + delta_rle
    if len(key_rle) < len(xbm_1024):
        return MAGIC + struct.pack("<BH", FLAG_RLE, len(key_rle)) + key_rle
    return MAGIC + struct.pack("<BH", FLAG_RAW, len(xbm_1024)) + xbm_1024


def gray_bytes_to_xbm_128x64(gray_bytes: bytes, threshold: int = 128, invert: bool = False) -> bytes:
    out = bytearray(1024)
    if not invert:
        for i in range(1024):
            off = i * 8
            b = 0
            if gray_bytes[off + 0] >= threshold: b |= 1
            if gray_bytes[off + 1] >= threshold: b |= 2
            if gray_bytes[off + 2] >= threshold: b |= 4
            if gray_bytes[off + 3] >= threshold: b |= 8
            if gray_bytes[off + 4] >= threshold: b |= 16
            if gray_bytes[off + 5] >= threshold: b |= 32
            if gray_bytes[off + 6] >= threshold: b |= 64
            if gray_bytes[off + 7] >= threshold: b |= 128
            out[i] = b
    else:
        for i in range(1024):
            off = i * 8
            b = 0
            if gray_bytes[off + 0] < threshold: b |= 1
            if gray_bytes[off + 1] < threshold: b |= 2
            if gray_bytes[off + 2] < threshold: b |= 4
            if gray_bytes[off + 3] < threshold: b |= 8
            if gray_bytes[off + 4] < threshold: b |= 16
            if gray_bytes[off + 5] < threshold: b |= 32
            if gray_bytes[off + 6] < threshold: b |= 64
            if gray_bytes[off + 7] < threshold: b |= 128
            out[i] = b
    return bytes(out)


def generate_demo_frame(frame_idx: int) -> bytes:
    buf = bytearray(1024)
    cx = 64 + int(38 * math.sin(frame_idx * 0.12))
    cy = 32 + int(16 * math.cos(frame_idx * 0.17))
    r2 = 16 * 16
    for y in range(64):
        dy2 = (y - cy) * (y - cy)
        row_off = y * 16
        for x in range(128):
            dx = x - cx
            in_circle = (dx * dx + dy2) <= r2
            in_wave = (y > 32 + int(12 * math.sin((x + frame_idx * 4) * 0.08)))
            if in_circle ^ in_wave:
                buf[row_off + (x >> 3)] |= (1 << (x & 7))
    return bytes(buf)


def build_vf_filter(fps: float, scale_mode: str) -> str:
    if scale_mode == "fit":
        return f"fps={fps},scale=128:64:force_original_aspect_ratio=decrease:flags=area,pad=128:64:(ow-iw)/2:(oh-ih)/2:black,format=gray"
    if scale_mode == "crop":
        return f"fps={fps},scale=128:64:force_original_aspect_ratio=increase:flags=area,crop=128:64,format=gray"
    return f"fps={fps},scale=128:64:flags=area,format=gray"


def open_ffmpeg(video_path: str, fps: float, scale_mode: str, start_sec: float = 0.0):
    cmd = ["ffmpeg", "-v", "error"]
    if start_sec > 0.1:
        cmd += ["-ss", f"{start_sec:.2f}"]
    cmd += [
        "-i", video_path,
        "-vf", build_vf_filter(fps, scale_mode),
        "-f", "rawvideo", "-pix_fmt", "gray", "pipe:1",
    ]
    return subprocess.Popen(cmd, stdout=subprocess.PIPE, bufsize=128 * 64 * 8)


async def find_pixljs(mac_filter: str | None = None):
    print("Scanning for Pixl.js (open '蓝牙投屏' on your OLED Pixl.js)...", flush=True)
    found = await BleakScanner.discover(timeout=6.0, return_adv=True)
    for d, adv in found.values():
        name = (d.name or "").strip()
        if mac_filter and d.address.upper() == mac_filter.upper():
            return d, adv.rssi
        if not mac_filter and name.lower() == "pixl.js":
            return d, adv.rssi
    return None, None


async def stream_video(video_path: str | None, fps: float, threshold: int, invert: bool,
                       scale_mode: str, mac: str | None):
    dev, scan_rssi = await find_pixljs(mac)
    if dev is None:
        print("ERROR: Pixl.js not found! Make sure '蓝牙投屏' is open on your Pixl.js.", flush=True)
        sys.exit(1)

    print(f"Connecting to {dev.name} ({dev.address}) [Scan RSSI: {scan_rssi} dBm]...", flush=True)
    live_rssi = scan_rssi
    async with BleakClient(dev) as client:
        mtu = getattr(client, "mtu_size", 23)
        chunk_size = max(20, min(244, mtu - 3))

        def on_nus_tx(_sender, data: bytearray):
            nonlocal live_rssi
            msg = data.decode("utf-8", errors="ignore").strip()
            if msg.startswith("RSSI:"):
                try:
                    live_rssi = int(msg.split(":", 1)[1])
                except ValueError:
                    pass

        try:
            await client.start_notify(NUS_TX_CHAR_UUID, on_nus_tx)
        except Exception:
            pass

        print(f"Connected! MTU={mtu}, chunk={chunk_size}B | Mode={scale_mode} | Target FPS={fps:.0f} | RSSI={live_rssi} dBm", flush=True)
        print("Hotkeys: '['/']' FPS | '-'/ '=' Threshold | 's' Scale(fit/stretch/crop) | 'i' Invert | 'p' Pause | 'r' Restart | 'q' Quit", flush=True)

        scale_idx = SCALE_MODES.index(scale_mode) if scale_mode in SCALE_MODES else 0
        paused = False
        play_sec = 0.0
        proc = open_ffmpeg(video_path, fps, SCALE_MODES[scale_idx], 0.0) if video_path else None

        frame_idx = 0
        prev_xbm = None
        start_time = time.perf_counter()
        sent_bytes = 0

        try:
            while True:
                # Non-blocking Windows hotkey polling
                if msvcrt is not None and msvcrt.kbhit():
                    ch = msvcrt.getwch().lower()
                    reopen = False
                    if ch == "q":
                        print("Quitting...", flush=True)
                        break
                    elif ch == "p":
                        paused = not paused
                        print(f"[{'PAUSED' if paused else 'RESUMED'}]", flush=True)
                    elif ch == "i":
                        invert = not invert
                        print(f"[Invert={invert}]", flush=True)
                    elif ch == "-":
                        threshold = max(10, threshold - 10)
                        print(f"[Threshold={threshold}]", flush=True)
                    elif ch == "=":
                        threshold = min(245, threshold + 10)
                        print(f"[Threshold={threshold}]", flush=True)
                    elif ch == "[":
                        fps = max(5.0, fps - 2.0)
                        print(f"[Target FPS={fps:.0f}]", flush=True)
                        reopen = video_path is not None
                    elif ch == "]":
                        fps = min(30.0, fps + 2.0)
                        print(f"[Target FPS={fps:.0f}]", flush=True)
                        reopen = video_path is not None
                    elif ch == "s":
                        scale_idx = (scale_idx + 1) % len(SCALE_MODES)
                        print(f"[Scale={SCALE_MODES[scale_idx]}]", flush=True)
                        reopen = video_path is not None
                    elif ch == "r" and video_path is not None:
                        play_sec = 0.0
                        print("[Restarting video]", flush=True)
                        reopen = True

                    if reopen and proc is not None:
                        proc.terminate()
                        proc.wait()
                        proc = open_ffmpeg(video_path, fps, SCALE_MODES[scale_idx], play_sec)

                if paused:
                    await asyncio.sleep(0.05)
                    continue

                t0 = time.perf_counter()
                if proc is not None:
                    raw_gray = proc.stdout.read(128 * 64)
                    if len(raw_gray) < 128 * 64:
                        print("Video finished!", flush=True)
                        break
                    xbm = gray_bytes_to_xbm_128x64(raw_gray, threshold=threshold, invert=invert)
                    play_sec += 1.0 / fps
                else:
                    xbm = generate_demo_frame(frame_idx)

                pkt = encode_frame_packet(xbm, prev_xbm=prev_xbm, force_keyframe=(frame_idx % 150 == 0))
                prev_xbm = xbm
                for off in range(0, len(pkt), chunk_size):
                    if off > 0:
                        await asyncio.sleep(0.003)
                    await client.write_gatt_char(NUS_RX_CHAR_UUID, pkt[off:off + chunk_size], response=False)
                sent_bytes += len(pkt)
                frame_idx += 1

                if frame_idx % 30 == 0:
                    elapsed = time.perf_counter() - start_time
                    print(f"Mode: {SCALE_MODES[scale_idx]} | Frames: {frame_idx} | Avg FPS: {frame_idx / elapsed:.1f} | "
                          f"RSSI: {live_rssi} dBm | Pkt: {sent_bytes / frame_idx:.0f}B | Thr: {threshold}"
                          f"{' | INV' if invert else ''}", flush=True)

                sleep_time = (1.0 / fps) - (time.perf_counter() - t0)
                if sleep_time > 0:
                    await asyncio.sleep(sleep_time)
        finally:
            if proc is not None:
                proc.terminate()
                proc.wait()


def main():
    parser = argparse.ArgumentParser(description="Stream Bad Apple!! or any video to Pixl.js OLED over BLE")
    parser.add_argument("video", nargs="?", default=None, help="Path to video file. Omit for built-in demo.")
    parser.add_argument("--fps", type=float, default=15.0, help="Target FPS (default: 15)")
    parser.add_argument("--threshold", type=int, default=128, help="B/W threshold 0-255 (default: 128)")
    parser.add_argument("--scale", choices=SCALE_MODES, default="fit", help="Scale mode: fit / stretch / crop (default: fit)")
    parser.add_argument("--invert", action="store_true", help="Invert black/white")
    parser.add_argument("--mac", type=str, default=None, help="Optional Pixl.js BLE MAC address")
    args = parser.parse_args()
    asyncio.run(stream_video(args.video, args.fps, args.threshold, args.invert, args.scale, args.mac))


if __name__ == "__main__":
    main()
