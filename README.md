# Solar-Garden-Light-Controller
Bộ điều khiển đèn sân vườn năng lượng mặt trời tự động

![Hardware - Altium Designer](https://img.shields.io/badge/Hardware-Altium%20Designer%2019-blue.svg)(#)
![MCU - PIC16F877A](https://img.shields.io/badge/MCU-PIC16F877A-orange.svg)(#)
![License - MIT](https://img.shields.io/badge/License-MIT-green.svg)(#)

Hệ thống điều khiển sạc pin mặt trời và tự động bật/tắt đèn sân vườn ban đêm, tích hợp bộ hạ áp Buck Converter, cảm biến dòng/áp I2C và tầng lái công suất MOSFET.

---

## 📌 Tổng Quan Dự Án

Dự án thiết kế mạch điều khiển nguồn năng lượng mặt trời thông minh cho hệ thống chiếu sáng sân vườn gia đình. Mạch xử lý toàn bộ chu trình từ quản lý sạc pin/ắc-quy ban ngày đến tự động bật đèn chiếu sáng ban đêm với độ tin cậy cao.

### Các Tính Năng Chính:
* **Tự động Bật/Tắt (Auto Day/Night Switch):** Cảm biến điện áp tấm pin qua mạch định thời NE555 và Rơ-le giúp nhận biết chính xác trạng thái Ngày/Đêm.
* **Bộ Sạc Buck Converter:** Hạ áp thông minh từ tấm pin mặt trời ($V_{solar}$) sạc cho Pin/Ắc-quy thông qua IC XL4016.
* **Đo Lường & Giám Sát:** Cảm biến INA219 giao tiếp I2C giúp vi điều khiển theo dõi chính xác dòng điện và điện áp sạc.
* **Khối Điều Control & Lái Tải:** 
  * Vi điều khiển **PIC16F877A** điều khiển luồng hoạt động và phát xung PWM.
  * Tầng lái Gate Driver dạng **Totem-pole** kích mở hoàn toàn MOSFET IRFZ44 đóng cắt đèn LED 15W ban đêm không lo bị nóng.

---

## 📐 Sơ Đồ Nguyên Lý (Schematic Architecture)

Sơ đồ nguyên lý được phân chia thành **5 khối chức năng chính**:

![Schematic Overview](./docs/schematic.png)

1. **AUTO DAY/NIGHT RELAY SWITCH:** Cảm biến điện áp Solar, mạch NE555 và các khâu hạ áp LM7812/LM7805.
2. **BUCK CONVERTER:** Mạch hạ áp XL4016 và hồi tiếp TL431 điều khiển dòng sạc.
3. **SOLAR INPUT & VOLTAGE / CURRENT SENSOR:** Đầu vào Solar và cảm biến dòng/áp INA219.
4. **MAIN CONTROLLER:** Khối vi điều khiển PIC16F877A, thạch anh 20MHz, nút nhấn Reset và mạch nạp ICSP.
5. **GATE DRIVER & LOAD SWITCH:** Tầng lái Totem-pole và MOSFET IRFZ44 điều khiển đèn LED 15W.

---

## 🛠️ Cấu Trúc Thư Mục Repository

```text
├── Documentation/          # Tài liệu thiết kế & File Smart PDF
│   └── Solar_Garden_Light_Controller.pdf
├── Hardware/               # File thiết kế Altium Designer
│   ├── Schematic/          # Sơ đồ nguyên lý (.SchDoc)
│   └── Outputs/            # File sản xuất (Gerber, BOM, Assembly)
├── Firmware/               # Mã nguồn C/C++ cho PIC16F877A
├── docs/                   # Hình ảnh minh họa cho README
└── README.md               # Tài liệu hướng dẫn dự án
