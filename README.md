# HỆ THỐNG ĐẤU GIÁ TRỰC TUYẾN

Ứng dụng đấu giá trực tuyến sử dụng TCP socket, multi-threading và MySQL database, được viết bằng C cho hệ điều hành Linux.

## Cài đặt cho người clone repository

### Bước 1: Clone repository
```bash
git clone https://github.com/turoisme/Laptrinhmang_Topic9_20251.git
cd Laptrinhmang_Topic9_20251
```

### Bước 2: Cài đặt MySQL (Ubuntu/WSL)
```bash
# Cài MySQL server và thư viện development
sudo apt update
sudo apt install mysql-server libmysqlclient-dev

# Khởi động MySQL service
sudo service mysql start

# Kiểm tra MySQL đã chạy chưa
sudo service mysql status
```

### Bước 3: Tạo Database và Tables
```bash
# Import database schema
sudo mysql < schema.sql

# Kiểm tra database đã tạo
sudo mysql -e "USE auction_system; SHOW TABLES;"
```


### Bước 4: Tạo MySQL User cho ứng dụng
```bash
sudo mysql -e "
CREATE USER IF NOT EXISTS 'auction_user'@'localhost' IDENTIFIED BY 'auction_pass';
GRANT ALL PRIVILEGES ON auction_system.* TO 'auction_user'@'localhost';
FLUSH PRIVILEGES;
"
```

**Giải thích:**
- Database: `auction_system`
- Username: `auction_user`
- Password: `auction_pass`

### Bước 5: Compile và chạy Server
```bash
# Compile
make clean && make server

# Chạy server trên port 5500
./run_server 5500
```

**Kết quả thành công:**
```
Connecting to MySQL database...
Initializing MySQL connection pool...
MySQL pool initialized with 10 connections
Database connected successfully!
Server started at the port 5500
```

### Bước 6: Test (Terminal khác)
```bash
# Test với sample user đã có sẵn
echo -e "LOGIN alice pass123\r\n" | nc localhost 5500
```

**Response mong đợi:**
```
200 Login successful
```

## 📊 Database Schema (MySQL)

Project sử dụng MySQL local với 6 tables:
3. **items** - Sản phẩm đấu giá
4. **bids** - Lịch sử đấu giá
5. **room_members** - Thành viên phòng
6. **activity_logs** - Logs hoạt động

Chi tiết xem file `schema.sql`

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
│   ├── client.c              # Main client (bao gồm I/O terminal)
│   ├── client.h
│   ├── connection.c          # Quản lý kết nối
│   ├── connection.h
│   ├── commands.c            # Xử lý các lệnh
│   ├── commands.h
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

