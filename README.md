# HỆ THỐNG ĐẤU GIÁ TRỰC TUYẾN

Ứng dụng đấu giá trực tuyến sử dụng TCP socket và đa luồng, được viết bằng C cho hệ điều hành Linux.

## Cấu trúc thư mục

```
Laptrinhmang/
├── Server/                    # Mã nguồn Server
│   ├── server.c              # Main server
│   ├── server.h
│   ├── socket_handler.c      # Xử lý socket và kết nối
│   ├── socket_handler.h
│   ├── auth.c                # Xử lý đăng ký/đăng nhập
│   ├── auth.h
│   ├── room.c                # Quản lý phòng đấu giá
│   ├── room.h
│   ├── item.c                # Quản lý vật phẩm và đấu giá
│   ├── item.h
│   ├── protocol.c            # Xử lý giao thức thông điệp
│   ├── protocol.h
│   ├── database.c            # Lưu trữ dữ liệu
│   ├── database.h
│   ├── logger.c              # Ghi log hoạt động
│   ├── logger.h
│   ├── thread_pool.c         # Quản lý đa luồng
│   └── thread_pool.h
│
├── Client/                    # Mã nguồn Client
│   ├── client.c              # Main client
│   ├── client.h
│   ├── connection.c          # Quản lý kết nối
│   ├── connection.h
│   ├── commands.c            # Xử lý các lệnh
│   ├── commands.h
│   ├── ui.c                  # Giao diện người dùng
│   ├── ui.h
│   ├── response_handler.c    # Xử lý response từ server
│   ├── response_handler.h
│   ├── notification.c        # Xử lý thông báo real-time
│   └── notification.h
│
├── Makefile                   # File biên dịch
└── README.md                  # File này

```

## Tính năng

### Người dùng có thể:
- ✅ Đăng ký và đăng nhập tài khoản
- ✅ Tạo phòng đấu giá
- ✅ Tham gia phòng đấu giá (mỗi lúc chỉ 1 phòng)
- ✅ Bán vật phẩm với giá khởi điểm và giá mua ngay
- ✅ Rút vật phẩm chưa được đấu giá
- ✅ Đấu giá vật phẩm (tố giá tối thiểu +10,000đ)
- ✅ Mua ngay vật phẩm
- ✅ Nhận thông báo real-time về đấu giá
- ✅ Hệ thống đếm ngược: 3 phút → 3 thông báo (mỗi 30s) → Kết thúc

## Giao thức thông điệp

### 1. Đăng ký
```
REGISTER <username> <password>

Response:
- 100 REGISTER_SUCCESS
- 111 USERNAME_EXISTS
- 112 INVALID_INPUT_PARAMETER
- 921 FORMAT_ERROR
```

### 2. Đăng nhập
```
LOGIN <username> <password>

Response:
- 200 LOGIN_SUCCESS
- 211 USER_NOT_FOUND
- 112 INVALID_INPUT_PARAMETER
- 213 ALREADY_LOGGED_IN
- 921 FORMAT_ERROR
```

### 3. Tạo phòng
```
CREATE_ROOM <room_name>

Response:
- 400 ROOM_CREATED <room_id>
- 112 INVALID_INPUT_PARAMETER
- 921 FORMAT_ERROR
```

### 4. Liệt kê phòng
```
LIST_ROOMS

Response:
- 500
  ROOM 1 <id> <name> <status>
  ROOM 2 <id> <name> <status>
  ...
- 921 FORMAT_ERROR
```

### 5. Tham gia phòng
```
JOIN_ROOM <room_id>

Response:
- 300 JOIN_OK
- 112 INVALID_INPUT_PARAMETER
- 313 ROOM_CLOSED
- 721 WENT_INTO_ANOTHER_ROOM
```

### 6. Tạo vật phẩm
```
CREATE_ITEM <room_id> "<item_name>" <start_price>

Response:
- 600 ITEM_CREATED <item_id>
- 112 INVALID_INPUT_PARAMETER
- 921 FORMAT_ERROR
```

### 7. Xóa vật phẩm
```
DELETE_ITEM <item_id>

Response:
- 620 ITEM_DELETED
- 112 INVALID_INPUT_PARAMETER
- 621 ITEM_ALREADY_SOLD
- 622 ACCESS_DENIED
- 921 FORMAT_ERROR
```

### 8. Đấu giá
```
BID <item_id> <price>

Response:
- 800 BID_OK <new_price>
- 811 BID_TOO_LOW
- 112 INVALID_INPUT_PARAMETER
- 921 FORMAT_ERROR
```

### 9. Mua ngay
```
BUY <item_id>

Response:
- 900 BUY_OK
- 112 INVALID_INPUT_PARAMETER
- 912 ITEM_ALREADY_SOLD
- 921 FORMAT_ERROR
```

## Biên dịch

### Biên dịch tất cả (server + client):
```bash
make
# hoặc
make all
```

### Biên dịch riêng lẻ:
```bash
make server    # Chỉ biên dịch server
make client    # Chỉ biên dịch client
```

### Xóa file đã biên dịch:
```bash
make clean
```

## Chạy chương trình

### Khởi động Server:
```bash
./server <Port_Number>

# Ví dụ:
./server 5500
```

### Khởi động Client:
```bash
./client <IP_Address> <Port_Number>

# Ví dụ:
./client 127.0.0.1 5500
./client 10.0.0.1 5500
```

## Yêu cầu hệ thống

- **Hệ điều hành**: Linux
- **Compiler**: GCC với hỗ trợ pthread
- **Thư viện**: 
  - pthread (đa luồng)
  - socket (TCP networking)

## Kiến trúc hệ thống

### Server
- **Đa luồng**: Mỗi client được xử lý bởi một thread riêng
- **TCP Socket**: Lắng nghe và chấp nhận kết nối
- **Database**: Lưu trữ users, rooms, items, bids
- **Logging**: Ghi lại tất cả hoạt động
- **Timer**: Đếm ngược 3 phút cho mỗi phiên đấu giá

### Client
- **Thread chính**: Xử lý input từ người dùng
- **Thread phụ**: Lắng nghe thông báo từ server
- **UI**: Giao diện dòng lệnh thân thiện
- **Auto-reconnect**: Tự động kết nối lại khi mất kết nối

## Phân điểm (20 điểm)

| Chức năng | Điểm |
|-----------|------|
| Xử lý truyền dòng | 1 |
| Cài đặt cơ chế vào/ra socket trên server | 2 |
| Đăng ký và quản lý tài khoản | 2 |
| Đăng nhập và quản lý phiên | 2 |
| Kiểm soát quyền truy cập phòng đấu giá | 1 |
| Tạo phòng đấu giá | 1 |
| Liệt kê phòng đấu giá | 1 |
| Tạo vật phẩm đấu giá | 3 |
| Xóa vật phẩm trong phòng đấu giá | 1 |
| Tham gia phòng đấu giá | 2 |
| Tố giá | 2 |
| Mua trực tiếp | 1 |
| Ghi log hoạt động | 1 |
| **Tổng** | **20** |

## Ghi chú

- Giá đấu phải cao hơn giá hiện tại ít nhất **10,000 đồng**
- Mỗi người chỉ tham gia **1 phòng** tại một thời điểm
- Thời gian đấu giá: **3 phút** → 3 thông báo countdown (mỗi **30 giây**)
- Tất cả thông điệp kết thúc bằng `\r\n`

## Tác giả

Đồ án môn Lập trình mạng - 2025

## License

Dự án học tập - Không sử dụng cho mục đích thương mại
