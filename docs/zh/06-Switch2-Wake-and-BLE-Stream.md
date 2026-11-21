# Switch 2 唤醒与蓝牙投屏指南

本固件为 Pixl.js（兼容 **OLED** 与 **LCD** 128×64 机型）新增了两项基于 BLE 的实用应用：**Switch2 唤醒** 与 **蓝牙投屏**。

---

## 一、Switch2 唤醒（Switch2 Wake）

### 功能简介
Nintendo Switch 2 在休眠状态下会监听来自**已配对 Joy-Con 2 / Pro Controller 2** 的 BLE 广播报文（Nintendo Company ID `0x0553`，唤醒标志 `0x81`）。本应用在 nRF52832（SoftDevice S112）上实现了：
- **一键唤醒 Switch 2**：通过 `sd_ble_gap_addr_set()` 临时将源 MAC 伪装为已配对的手柄 Public MAC，以最大发射功率（+4 dBm）广播 31 字节 Switch 2 唤醒报文，完成后自动恢复 Pixl.js 原生 MAC。
- **本机无线扫描抓包**：无需手机或电脑，利用 SoftDevice S112 Radio Timeslot API 直接驱动 `NRF_RADIO` 监听 BLE 广播信道 37/38/39，自动捕获 Joy-Con 2 / Pro 2 按下 Home 键时发出的 `0x0553` + `0x81` 唤醒报文并保存到本地。
- **开屏自动唤醒（Boot Wake）**：开启后，每次按拨轮点亮休眠中的 Pixl.js 时会自动后台广播 2 秒唤醒包（NFC 静默刷卡唤醒不会误触发）。
- **双模式兼容**：支持 `NonConn`（默认，不可连接单向广播）与 `Conn`（可连接广播，连接后自动断开）两种广播模式。

### 快速上手
1. 在桌面打开 **Switch2 唤醒**。
2. 选择 **扫描唤醒包**（屏幕提示 `请按手柄Home键`，持续 15 秒）。
3. 将已与你的 Switch 2 配对过的 **Joy-Con 2** 或 **Pro Controller 2** 贴近 Pixl.js，短按一下 **Home 键**。
4. 捕获成功后会提示 **`捕获成功!`**，并自动保存到根目录 `/switch2_wake.txt`。
5. Switch 2 休眠后，点击 **唤醒 Switch 2** 即可唤醒（也可打开 **开屏自动唤醒** 实现亮屏即唤醒）。

### 手动配置文件 `/switch2_wake.txt`
也可以通过 **蓝牙传输** 连接 Web 工具，在根目录创建或编辑 `/switch2_wake.txt`：

```ini
# 方式 1：填写手柄 MAC 与 Switch 2 主机 MAC（固件自动拼包并反序主机 MAC）
mac=AA:BB:CC:DD:EE:FF
host=11:22:33:44:55:66
pid=2066          # 2066=JC2-L, 2067=JC2-R, 2069=Pro2
mode=nonconn      # nonconn 或 conn
auto_wake=0       # 1=开屏自动唤醒, 0=关闭

# 方式 2：或直接填写抓包得到的 31 字节原始十六进制广播数据（优先级高于 host）
#data=0201061BFF53050100037E0566200001816655443322110F00000000000000
```

---

## 二、蓝牙投屏（BLE Stream）

### 功能简介
利用 Pixl.js 现有的 BLE Nordic UART Service（NUS，250B 大 MTU 与 10–20 ms 低延迟连接间隔），实现将电脑端的视频流实时压缩并推送到 Pixl.js 的 128×64 屏幕（OLED / LCD 均支持）。

### 传输协议（帧头 5 字节 + 载荷）
| 字节偏移 | 含义 | 说明 |
| :--- | :--- | :--- |
| `0..1` | Magic | 固定 `0xA5 0x5A` |
| `2` | Flags | `0x00`=原始 1024B XBM，`0x01`=PackBits RLE 帧，`0x02`=帧间 XOR 差分 + RLE |
| `3..4` | Length | 载荷字节长度（uint16 小端，≤ 1024） |
| `5..` | Payload | 图像数据（128×64，LSB-first 水平 XBM 格式） |

### 设备端操作
在桌面打开 **蓝牙投屏** 后：
- **左键**：开关顶部实时调试 HUD（显示 `实测FPS / 实时RSSI(dBm) / 单帧字节数`）
- **右键**：一键切换画面黑白反色
- **中键**：退出投屏并返回桌面
- **自动重连**：电脑端停止推流或断开蓝牙时，设备自动清空最后一帧并退回等待连接界面继续广播。

### 电脑端推流脚本
仓库自带跨平台推流脚本 [`fw/scripts/stream_video_to_pixl.py`](../../fw/scripts/stream_video_to_pixl.py)（依赖 Python `bleak` 与 `ffmpeg`）：

```bash
pip install bleak
# 运行内置演示动画
python fw/scripts/stream_video_to_pixl.py

# 推流本地视频文件（如 Bad Apple!!）
python fw/scripts/stream_video_to_pixl.py path/to/video.mp4 --fps 15 --scale stretch
```

播放过程中支持终端热键实时调节：
- `[` / `]`：降低 / 提高目标帧率（5–30 FPS）
- `s`：切换缩放模式（`fit` 等比留黑边 / `stretch` 拉伸铺满 / `crop` 居中裁剪）
- `-` / `=`：调节二值化阈值（10–245）
- `i`：反色 | `p`：暂停/继续 | `r`：重播 | `q`：退出
