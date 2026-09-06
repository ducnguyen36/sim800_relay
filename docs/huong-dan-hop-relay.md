# HƯỚNG DẪN SỬ DỤNG HỘP 4 RELAY (A7680C — RL-1.0)

Thiết bị điều khiển **4 relay độc lập** (bơm, đèn, thiết bị điện). Mỗi relay
**giữ trạng thái** (bật tới khi tắt), điều khiển bằng **nút bấm trên máy**,
**remote RF**, hoặc **SMS**. Có menu/PIN và đăng ký số điện thoại như dòng cũ.

> Firmware này KHÔNG phải điều khiển cửa cuốn — không có logic mở/đóng/khóa cửa.

## 1. Cài đặt lần đầu

Máy mới chưa có số nào, PIN mặc định **0000**.

1. **Vào menu:** nhấn **M**, nhập PIN **0000** (nút **+** tăng số, **M** sang ô kế). Màn hình hiện **"MASTER:"**.
2. **Học remote (nếu có):** khi đang ở "MASTER:", vào menu **CHÍNH** rồi bấm nút trên remote để học (xem mục 4).
3. **Lưu số chính:** khi đang ở "MASTER:", dùng điện thoại muốn làm **số chính** gọi vào SIM. Máy tự cúp máy và lưu làm **số chính (Super/M)**.

## 2. Điều khiển bằng nút bấm trên máy

Màn hình chính hiển thị trạng thái 4 relay (hiện **số** nếu relay ON, dấu **-** nếu OFF) và phiên bản; dòng dưới là giờ/ngày.

- **Nút +** (màn hình chính): nếu cả 4 relay đang ON → **tắt tất cả**; ngược lại → **bật tất cả**.
- **Nút B** (màn hình chính): mở **màn hình panel** để điều khiển từng relay:
  - **B**: chuyển chọn relay kế tiếp (R1 → R2 → R3 → R4 → R1), relay đang chọn nhấp nháy.
  - **+**: bật/tắt relay đang chọn.
  - **M**: thoát về màn hình chính. Không bấm gì ~15 giây cũng tự thoát.
- **Nút M** (màn hình chính): nhập PIN vào menu cấu hình.

Trạng thái relay được **lưu vào bộ nhớ**; sau khi mất điện có điện lại, máy **khôi phục đúng trạng thái trước đó**.

## 3. Menu cấu hình (nhấn M → nhập PIN)

- **CHÍNH** — lưu **số chính (Master)** mới (gọi vào máy để lưu); cũng là nơi **học remote** (bấm nút remote).
- **PHỤ** — lưu **số phụ (User)** (gọi vào để lưu).
- **ĐỔI PIN** — nhập PIN cũ (4 số) → PIN mới (4 số).
- **EXIT** — thoát menu.
- **Giữ nút B lâu** khi ở CHÍNH/PHỤ → **xóa toàn bộ số, remote và đặt lại PIN 0000**.

## 4. Remote RF

- **Học remote:** vào menu **CHÍNH** → bấm nút cần học trên remote → máy báo "remote dc hoc".
- **Điều khiển:** mỗi nút trên remote **bật/tắt (toggle)** một relay — bấm 1 lần bật, bấm lần nữa tắt:
  - Nút **lên → Relay 1**
  - Nút **xuống → Relay 2**
  - Nút **khóa → Relay 3**
  - Nút **stop → Relay 4**

> Lưu ý: quy ước nút ↔ relay ở trên nên **kiểm tra thực tế với remote của bạn**; nếu lệch, đối chiếu mã nút máy in ra (dòng debug 'P') để chỉnh.

## 5. Điều khiển & cấu hình bằng SMS

Cú pháp: `<PIN>,<lệnh>` — ví dụ PIN `0000`. Người gửi phải là số đã lưu. Không dấu tiếng Việt.

| Lệnh SMS | Chức năng | Quyền |
|---|---|---|
| `0000,R1,ON` … `0000,R4,ON` | Bật relay 1..4 | Số đã lưu |
| `0000,R1,OFF` … `0000,R4,OFF` | Tắt relay 1..4 | Số đã lưu |
| `0000,ALL,ON` | Bật cả 4 relay | Số đã lưu |
| `0000,ALL,OFF` | Tắt cả 4 relay | Số đã lưu |
| `0000,B` | Nhắn lại **trạng thái 4 relay** | Số đã lưu |
| `0000,Luu,0912345678` | Lưu số làm **User** | Master |
| `0000,Luu,0912345678,m` | Lưu số làm **Master** | Master |
| `0000,Xoa,5` / `0000,Xoa,0` | Xóa số thứ 5 / xóa tất cả | Master |
| `0000,D` | Nhắn lại danh sách số đã lưu | Master |
| `0000,Pin,xxxx` | Đổi PIN thành `xxxx` | Master |
| `0000,Kp,ON` / `0000,Kp,OFF` | Khóa / mở khóa **bàn phím** trên máy | Master |
| `0000,BC,Bat` / `0000,BC,Tat` | Bật / tắt báo cáo qua SMS | Master |
| `0000,g` | Máy gọi lại cho người vừa gửi | Master |
| `0000,Cg,hhmmddMMyy` | Chỉnh giờ thủ công (vd `0000,Cg,1430260826` = 14:30 26/08/26) | Master |
| `0000,R` | Khởi động lại module SIM (10 giây) | Master |
| `0000,Rr,r` | Khôi phục cài đặt gốc (xóa số/remote, PIN 0000, tắt hết relay) | Super (M) |

- Cuộc gọi đến chỉ dùng để **đăng ký số** (khi đang ở màn hình lưu số); **không** điều khiển relay.

## 6. Tự động lấy lại giờ

Máy tự lấy giờ từ mạng khi khởi động và làm mới mỗi giờ. Nếu lúc đó chưa lấy được (một số SIM), máy **tự thử lại mỗi vài phút cho tới khi có giờ**. Nếu vẫn không có, dùng lệnh `Cg` để chỉnh tay.

## 7. Bảo mật

- Nhập sai PIN trên bàn phím **5 lần** → tự khóa bàn phím; mở lại bằng SMS `Kp,OFF` từ Master.
- Xóa toàn bộ số (`Xoa,0`, `Rr,r`, hoặc giữ nút B) làm mất quyền Master hiện tại — máy quay lại trạng thái "chưa có Master".

---

*Kiểm chứng theo firmware phiên bản RL-1.0. Nên thử lại từng chức năng trên phần cứng trước khi giao khách (đặc biệt: relay giữ ổn định, quy ước nút remote, bố cục LCD).*
