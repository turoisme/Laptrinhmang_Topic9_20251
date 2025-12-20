Tạm thời chỉ cần import auth.h và room.h
Gọi takeAuthCommand(int sockfd, char *cmd) khi có lệnh LOGIN,REGISTER,LOGOUT với tham số:
   sockfd là socket gửi thông tin
   cmd là toàn bộ câu lệnh (kết thúc bằng '\0')
Gọi takeRoomCommand(int sockfd, char *cmd) khi có lệnh CREATE_ROOM,JOIN_ROOM,LEAVE_ROOM với tham số:
   sockfd là socket gửi thông tin
   cmd là toàn bộ câu lệnh (kết thúc bằng '\0')
Hiện tại server chưa kết nối vào database (lưu bằng danh sách liên kết)
Xóa file này trước khi push, hoặc để lại nó để đưa hướng dẫn