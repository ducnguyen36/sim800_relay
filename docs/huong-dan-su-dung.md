# HƯỚNG DẪN SỬ DỤNG BỘ ĐIỀU KHIỂN CỬA CUỐN (A7680C)

## 1. Cài đặt lần đầu (rất quan trọng — làm đúng thứ tự)

Máy mới (hoặc sau khi khôi phục cài đặt gốc) chưa có số điện thoại nào, mã PIN mặc định là **0000**.

**Bước 1 — Vào menu cấu hình:**
Nhấn nút **M**, nhập PIN **0000** bằng nút **+** (tăng số) và **M** (chuyển sang ô tiếp theo), màn hình sẽ hiện **"MASTER:"**.

**Bước 2 — Học remote khẩn cấp (nếu có, làm TRƯỚC khi lưu số chính):**
Trong lúc màn hình vẫn hiện "MASTER:", bấm 1 nút bất kỳ trên remote RF muốn dùng làm **remote khẩn cấp**. Remote này sẽ luôn mở được cửa/mở khóa bất kể trạng thái khóa. *Lưu ý: chỉ học được remote khẩn cấp ở bước này — sau khi đã lưu số chính (Bước 3), phải khôi phục cài đặt gốc mới học lại được.*

**Bước 3 — Lưu số điện thoại chính (Master):**
Vẫn đang ở màn hình "MASTER:", dùng điện thoại muốn làm **số chính** gọi vào SIM trên máy. Máy sẽ tự cúp máy và lưu số này làm **số chính (Master)**, gửi tin nhắn xác nhận "Luu Master".

Từ đây máy đã có Master, sẵn sàng sử dụng.

## 2. Ba loại tài khoản (vai trò)

| Vai trò | Ký hiệu | Quyền hạn |
|---|---|---|
| **Super** | `M` | Toàn quyền — kể cả khôi phục cài đặt gốc (`Rr,r`). Chỉ số chính đầu tiên (Bước 3 ở trên) mới nhận vai trò này. |
| **Master (Chính)** | `m` | Mở/đóng/dừng cửa, quản lý số điện thoại, đổi PIN, cài đặt (UPS, báo cáo, hướng motor, khóa...). |
| **User (Phụ)** | `u` | Chỉ mở/đóng/dừng cửa qua SMS hoặc cuộc gọi. Không quản lý được cài đặt. |

## 3. Điều khiển bằng cuộc gọi điện thoại

- Gọi vào SIM từ một số **đã lưu** (Master hoặc User) → cửa tự **mở**, máy tự cúp máy ngay (không tốn cước, không cần nghe máy).
- Cửa đang khóa (`Khoa cua cuon`) → gọi vào sẽ không mở, máy nhắn lại "Cua dang khoa".

## 4. Điều khiển bằng bàn phím trên máy

- **Nút M**: mở/xác nhận menu, chuyển mục.
- **Nút +**: tăng số (khi nhập PIN), điều khiển lên/dừng cửa (ở màn hình chính).
- **Nút B**: quay lại / xuống — lùi một bước hoặc điều khiển hướng còn lại.
- Ở màn hình chính (chưa nhập PIN): nút **+** và **B** điều khiển cửa lên/dừng/xuống (bấm luân phiên: lần 1 chạy 1 hướng, lần 2 dừng...).
- Nhấn **M** → nhập đúng PIN → vào menu chính:
  - **CHÍNH** — nhấn **B** để lưu **số chính (Master)** mới: màn hình yêu cầu gọi vào máy.
  - **PHỤ** — tương tự, lưu **số phụ (User)**.
  - **ĐỔI PIN** — nhập PIN cũ (4 số) → nhập PIN mới (4 số) → xác nhận.
  - **HƯỚNG MOTOR** — chọn TRÁI/PHẢI tùy motor lắp bên nào (xem mục 7).
  - **EXIT** — thoát menu.
- **Giữ nút B lâu** khi đang ở màn hình CHÍNH/PHỤ → **xóa toàn bộ số điện thoại đã lưu, remote đã học, và đặt lại PIN về 0000** (khôi phục một phần, không xóa cài đặt khóa/UPS/hướng — dùng lệnh `Rr,r` qua SMS nếu muốn khôi phục toàn bộ).

## 5. Remote điều khiển từ xa (RF)

- **Học remote thường:** vào menu CHÍNH (nếu đã có Master) → nhấn nút cần học trên remote → máy báo "remote dc hoc". Học được tối đa ~18 remote thường (ngoài remote khẩn cấp và module báo động).
- **Học module báo động:** vào menu, chuyển sang mục **PHỤ**, nhấn nút trên module báo động cần học.
- Mỗi remote học được có **2 nút**: một nút mở/lên, một nút đóng/xuống (bấm nút thứ 3 lần lượt sẽ dừng).
- **Khóa RF** (không cho remote nào điều khiển): giữ nút khóa trên remote (nút đầu tiên/nút số 0). Mở lại bằng cách giữ nút đó lần nữa.

## 6. Toàn bộ lệnh SMS

**Cú pháp chung:** `<PIN 4 số>,<lệnh>` — ví dụ PIN là `0000`.

Người gửi phải là số đã lưu (Master hoặc User tùy lệnh). Không nhập dấu tiếng Việt.

| Lệnh SMS (gửi tới máy) | Chức năng | Quyền |
|---|---|---|
| `0000,Le` (hoặc `Len`) | **Mở cửa** | Master hoặc User |
| `0000,Xu` (hoặc `Xuong`) | **Đóng cửa** | Master hoặc User |
| `0000,S` | **Dừng cửa** (cũng dùng để mở khóa nếu đã khóa bằng lệnh `K`) | Master hoặc User (Master bắt buộc nếu đang khóa toàn bộ) |
| `0000,K` | **Khóa cửa** (không cho mở, kể cả gọi/SMS/remote thường — chỉ remote khẩn cấp mở được) | Master |
| `0000,Kp,ON` | Khóa **bàn phím** trên máy | Master |
| `0000,Kp,OFF` | Mở khóa bàn phím | Master |
| `0000,Pin,xxxx` | Đổi mã PIN thành `xxxx` (4 số) | Master |
| `0000,Luu,0912345678` | Lưu số `0912345678` làm **User** | Master |
| `0000,Luu,0912345678,m` | Lưu số `0912345678` làm **Master** | Master |
| `0000,Xoa,5` | Xóa số thứ **5** trong danh sách | Master |
| `0000,Xoa,0` | Xóa **toàn bộ** số đã lưu | Master |
| `0000,D` | Nhắn lại **danh sách số** đã lưu | Master |
| `0000,g` | **Gọi lại** cho người vừa gửi lệnh này | Master |
| `0000,UPS,Bat` (hoặc `,ON`) | Bật lưu điện UPS | Master |
| `0000,UPS,Tat` (hoặc `,OFF`) | Tắt UPS | Master |
| `0000,BC,Bat` (hoặc `,ON`) | Bật gửi báo cáo mỗi lần thao tác | Master |
| `0000,BC,Tat` (hoặc `,OFF`) | Tắt báo cáo | Master |
| `0000,B` | Kiểm tra máy còn hoạt động (máy nhắn lại "Bao cao thanh cong") | Master |
| `0000,R` | Khởi động lại **module SIM** (10 giây) | Master |
| `0000,Rr,r` | **Khôi phục cài đặt gốc** — xóa hết số, remote, đặt lại PIN 0000, mở tất cả khóa | Chỉ **Super (M)** |
| `<pin>,?` | Nhận hướng dẫn lệnh (bản cũ, có thể chưa cập nhật đầy đủ) | Bất kỳ số đã lưu |

## 7. Hướng motor (TRÁI / PHẢI)

Motor cửa có thể lắp bên trái hoặc bên phải, khiến chiều "mở/đóng" bị đảo ngược. Vào menu → **HƯỚNG MOTOR** → chọn đúng bên motor thực tế lắp đặt. Nếu chọn sai, lệnh mở sẽ làm cửa chạy ngược (đóng thay vì mở).

## 8. UPS (lưu điện dự phòng)

Bật UPS (`0000,UPS,Bat`) để relay 4 kích hoạt nguồn dự phòng khi cần; UPS tự tắt sau một khoảng thời gian đếm ngược cố định hoặc tắt thủ công bằng `0000,UPS,Tat`.

## 9. Lưu ý bảo mật

- Nhập sai PIN trên bàn phím **5 lần liên tiếp** → tự động khóa bàn phím, cần Master gửi SMS `Kp,OFF` hoặc dùng số điện thoại để mở lại.
- Lệnh `K` (khóa cửa) chỉ có thể mở bằng: lệnh `S`, remote khẩn cấp, hoặc khi máy tự đếm thời gian khóa vượt quá giới hạn an toàn.
- Xóa toàn bộ số (`Xoa,0` hoặc giữ nút B) sẽ làm mất luôn quyền Master hiện tại — máy quay lại trạng thái "chưa có Master", lần gọi vào tiếp theo (ở màn hình CHÍNH) sẽ tự thành Master mới.

---

*Tài liệu này được kiểm chứng lại từng lệnh so với mã nguồn firmware (phiên bản A7-1.2), nhưng chưa được thử nghiệm trực tiếp trên phần cứng thật — nên kiểm tra lại từng mục trước khi phát hành cho khách hàng.*
