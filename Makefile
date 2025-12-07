# Makefile - Biên dịch server và client
# Sử dụng: make all (hoặc make) để biên dịch cả server và client
# make server: chỉ biên dịch server
# make client: chỉ biên dịch client
# make clean: xóa các file object và executable

CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LDFLAGS = -pthread

# Thư mục
SERVER_DIR = Server
CLIENT_DIR = Client

# Server files
SERVER_SOURCES = $(SERVER_DIR)/server.c \
                 $(SERVER_DIR)/socket_handler.c \
                 $(SERVER_DIR)/auth.c \
                 $(SERVER_DIR)/room.c \
                 $(SERVER_DIR)/item.c \
                 $(SERVER_DIR)/protocol.c \
                 $(SERVER_DIR)/database.c \
                 $(SERVER_DIR)/logger.c \
                 $(SERVER_DIR)/thread_pool.c

SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)
SERVER_TARGET = server

# Client files
CLIENT_SOURCES = $(CLIENT_DIR)/client.c \
                 $(CLIENT_DIR)/connection.c \
                 $(CLIENT_DIR)/commands.c \
                 $(CLIENT_DIR)/ui.c \
                 $(CLIENT_DIR)/response_handler.c \
                 $(CLIENT_DIR)/notification.c

CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)
CLIENT_TARGET = client

# Targets
.PHONY: all server client clean

# Build all
all: server client

# Build server
server: $(SERVER_TARGET)

$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "Server compiled successfully!"

# Build client
client: $(CLIENT_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "Client compiled successfully!"

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo "Cleaned all object files and executables"

# Help
help:
	@echo "Makefile for Online Auction System"
	@echo ""
	@echo "Usage:"
	@echo "  make all     - Compile both server and client"
	@echo "  make server  - Compile server only"
	@echo "  make client  - Compile client only"
	@echo "  make clean   - Remove all compiled files"
	@echo "  make help    - Show this help message"
	@echo ""
	@echo "Run server: ./server <Port_Number>"
	@echo "Run client: ./client <IP_Address> <Port_Number>"
