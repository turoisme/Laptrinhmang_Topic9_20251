===============================================
CLIENT FOLDER - DOCUMENTATION
===============================================

Hệ thống client cho ứng dụng đấu giá online với TCP socket và multi-threading.
Client kết nối đến server, gửi lệnh và nhận response/notification real-time.

===============================================
1. client.c / client.h
===============================================
Chức năng: Entry point của client, quản lý main loop và menu system

Hàm chính:
- main(argc, argv): Khởi động client
  + Parse IP và port từ command line arguments
  + Gọi connect_to_server() để kết nối
  + Quản lý 3 menu states: HOME_MENU, ROOM_MENU, ITEM_MENU
  + Main loop nhận input từ user và gọi cmd_* functions
  
- print_home_menu(): Hiển thị menu khi chưa login
  + Options: REGISTER, LOGIN, EXIT
  
- print_room_menu(): Hiển thị menu khi đã login nhưng chưa vào phòng
  + Options: CREATE_ROOM, JOIN_ROOM, LIST_ROOMS, LOGOUT, EXIT
  
- print_item_menu(): Hiển thị menu khi đã vào phòng
  + Options: CREATE_ITEM, DELETE_ITEM, LIST_ITEMS, BID, BUY, LEAVE_ROOM, LOGOUT, EXIT
  
- clear_input_buffer(): Xóa buffer stdin sau scanf

Biến quan trọng:
- is_logged_in: Flag kiểm tra trạng thái đăng nhập (0/1)
- is_in_room: Flag kiểm tra đã vào phòng chưa (0/1)

Logic chuyển đổi menu:
- LOGIN/REGISTER thành công (code 200/100) → is_logged_in = 1 → ROOM_MENU
- JOIN_ROOM thành công (code 300) → is_in_room = 1 → ITEM_MENU
- LEAVE_ROOM → is_in_room = 0 → quay lại ROOM_MENU
- LOGOUT → reset cả 2 flags → quay lại HOME_MENU

===============================================
2. connection.c / connection.h
===============================================
Chức năng: Quản lý kết nối TCP socket và stream processing

Structures:
- ConnectionBuffer: Buffer để xử lý message streaming
  + buffer[BUFF_SIZE*2]: Lưu dữ liệu chưa xử lý xong
  + buffer_len: Độ dài dữ liệu trong buffer
  
- ClientConnection: Thông tin kết nối
  + sockfd: Socket file descriptor
  + server_addr: Địa chỉ server
  + conn_buf: ConnectionBuffer cho stream processing
  + is_connected: Trạng thái kết nối

Hàm chính:
- connect_to_server(conn, ip, port): Kết nối đến server
  + Tạo socket với socket(AF_INET, SOCK_STREAM, 0)
  + Convert IP bằng inet_pton()
  + Gọi connect() để thiết lập kết nối
  + Khởi tạo ConnectionBuffer
  + Return 0 nếu thành công, -1 nếu lỗi
  
- disconnect_from_server(conn): Ngắt kết nối
  + close(sockfd)
  + Set is_connected = 0
  
- send_command(conn, command): Gửi lệnh đến server
  + Tự động thêm delimiter "\r\n" vào cuối message
  + Xử lý partial send bằng loop
  + Return số bytes đã gửi
  
- receive_response(conn, **response): Nhận response từ server
  + Stream processing với delimiter "\r\n"
  + Kiểm tra buffer trước, tìm delimiter
  + Nếu chưa có message hoàn chỉnh, gọi recv() để nhận thêm
  + Lưu phần dư vào buffer cho lần sau
  + Xử lý partial messages và multiple messages in one buffer
  + Allocate memory động cho response, caller phải free()
  + Return số bytes nhận được, -1 nếu lỗi
  
- is_connected(conn): Kiểm tra trạng thái kết nối

Đặc điểm quan trọng:
- Blocking I/O: recv() sẽ block cho đến khi có data
- Stream processing: Xử lý TCP stream với delimiter "\r\n"
- Buffer management: Lưu partial messages để xử lý tiếp

===============================================
3. commands.c / commands.h
===============================================
Chức năng: Format protocol commands và gửi đến server

Hàm authentication:
- cmd_register(conn, username, password): Đăng ký tài khoản
  + Format: "REGISTER username password"
  + Gọi send_command()
  
- cmd_login(conn, username, password): Đăng nhập
  + Format: "LOGIN username password"

Hàm quản lý phòng:
- cmd_create_room(conn, room_name): Tạo phòng đấu giá
  + Format: "CREATE_ROOM room_name"
  
- cmd_join_room(conn, room_name): Tham gia phòng
  + Format: "JOIN_ROOM room_name"
  
- cmd_leave_room(conn): Rời phòng hiện tại
  + Format: "LEAVE_ROOM"
  
- cmd_list_rooms(conn): Liệt kê tất cả phòng
  + Format: "LIST_ROOMS"

Hàm quản lý vật phẩm:
- cmd_create_item(conn, item_name, start_price, buy_now_price): Tạo vật phẩm đấu giá
  + Format: "CREATE_ITEM item_name start_price buy_now_price"
  
- cmd_delete_item(conn, item_name): Xóa vật phẩm
  + Format: "DELETE_ITEM item_name"
  
- cmd_list_items(conn): Liệt kê vật phẩm trong phòng
  + Format: "LIST_ITEMS"

Hàm đấu giá:
- cmd_bid(conn, item_name, amount): Đặt giá
  + Format: "BID item_name amount"
  
- cmd_buy(conn, item_name): Mua ngay với giá buy-now
  + Format: "BUY item_name"

Đặc điểm:
- Tất cả hàm tự động format command theo protocol
- Gọi send_command() để gửi (tự động thêm "\r\n")
- Return số bytes đã gửi

===============================================
4. response_handler.c / response_handler.h
===============================================
Chức năng: Parse và hiển thị server responses

Response codes:
- 0: DATABASE_ERROR
- 100: REGISTER_SUCCESS
- 111: USERNAME_EXISTS
- 112: INVALID_INPUT_PARAMETER
- 200: LOGIN_SUCCESS
- 211: USER_NOT_FOUND
- 213: ALREADY_LOGGED_IN
- 300: JOIN_OK
- 313: ROOM_CLOSED
- 400: ROOM_CREATED
- 600: ITEM_CREATED
- 620: ITEM_DELETED
- 621: ITEM_ALREADY_SOLD
- 622: ACCESS_DENIED
- 721: WENT_INTO_ANOTHER_ROOM
- 800: BID_OK
- 811: BID_TOO_LOW
- 900: BUY_OK
- 912: ITEM_ALREADY_SOLD_BUY
- 921: FORMAT_ERROR

Hàm chính:
- parse_and_display_response(response): Parse và hiển thị response
  + Parse format: "code message"
  + Sử dụng sscanf() để tách code và message
  + Hiển thị: "[Response code] message"
  + Nếu chỉ có code không có message → hiển thị "OK"

Đặc điểm:
- Chỉ parse response từ command (codes 0-999)
- KHÔNG xử lý notification (codes 1000-1999) - việc đó do notification.c làm

===============================================
5. notification.c / notification.h
===============================================
Chức năng: Background thread nhận broadcast notifications từ server

Notification codes:
- 1000: NOTIFY_BID - Có người đặt giá mới
- 1001: NOTIFY_BUY - Vật phẩm được mua
- 1002: NOTIFY_ITEM_CREATED - Có vật phẩm mới
- 1003: NOTIFY_ITEM_DELETED - Vật phẩm bị xóa
- 1004: NOTIFY_COUNTDOWN - Đếm ngược thời gian (3 phút, 30s, 10s...)
- 1005: NOTIFY_AUCTION_END - Đấu giá kết thúc
- 1006: NOTIFY_USER_JOIN - User tham gia phòng
- 1007: NOTIFY_USER_LEAVE - User rời phòng

Hàm chính:
- start_notification_thread(conn): Bật notification thread
  + Tạo pthread với pthread_create()
  + Thread chạy song song với main loop
  + pthread_detach() để tự động cleanup
  
- stop_notification_thread(): Tắt notification thread
  + Set flag thread_running = 0
  + Thread sẽ tự thoát ở lần receive_response() tiếp theo
  
- is_notification_running(): Kiểm tra trạng thái thread
  
- notification_receiver(arg): Thread function (chạy trong background)
  + While loop liên tục gọi receive_response()
  + Parse notification code
  + Nếu code >= 1000 && < 2000 → là notification
  + In lại prompt "Enter choice: " để user biết vẫn có thể nhập
  + fflush(stdout) để hiển thị ngay

Kịch bản sử dụng:
- Khi JOIN_ROOM thành công → gọi start_notification_thread()
- Thread lắng nghe notifications trong khi user nhập lệnh bình thường
- Khi LEAVE_ROOM → gọi stop_notification_thread()

Đặc điểm quan trọng:
- Non-blocking cho user: Thread chạy background, không block main menu
- Real-time notifications: Nhận ngay khi server gửi
- Tự động re-print prompt sau mỗi notification
- Thread safe: Mỗi connection có một thread riêng

===============================================
LUỒNG HOẠT ĐỘNG TỔNG QUÁT
===============================================

1. Khởi động:
   client.c main() → parse args → connect_to_server()

2. Menu loop (client.c):
   print_menu() → scanf() → cmd_*() → receive_response() → parse_and_display_response()

3. Gửi command:
   cmd_*() → format command string → send_command() → append "\r\n" → send()

4. Nhận response (đồng bộ):
   receive_response() → recv() → stream processing → tìm "\r\n" → return message
   → parse_and_display_response() → hiển thị kết quả

5. Nhận notification (bất đồng bộ):
   notification_receiver() thread → receive_response() → parse code >= 1000
   → hiển thị với icon → re-print prompt

6. Chuyển đổi state:
   - HOME_MENU → LOGIN/REGISTER → ROOM_MENU
   - ROOM_MENU → JOIN_ROOM + start_notification_thread() → ITEM_MENU
   - ITEM_MENU → LEAVE_ROOM + stop_notification_thread() → ROOM_MENU

===============================================
COMPILATION & USAGE
===============================================

Build:
  make

Run:
  ./run_client <server_ip> <port>
  Example: ./run_client 127.0.0.1 5500

Cleanup:
  make clean

===============================================
DEPENDENCIES
===============================================

- pthread: Multi-threading cho notification
- socket: TCP communication
- stdio, stdlib, string, unistd: Standard C libraries

=============================================== 