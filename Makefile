CC = gcc
CFLAGS = -Wall -Wextra -pthread -g `mysql_config --cflags`
LDFLAGS = -pthread `mysql_config --libs`

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
SERVER_TARGET = run_server

# Client files
CLIENT_SOURCES = $(CLIENT_DIR)/client.c \
                 $(CLIENT_DIR)/connection.c \
                 $(CLIENT_DIR)/commands.c \
                 $(CLIENT_DIR)/response_handler.c \
                 $(CLIENT_DIR)/notification.c

CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)
CLIENT_TARGET = run_client

# Targets
.PHONY: all server client clean

# Build all
all: server client

# Build server
server: $(SERVER_TARGET)

$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Server compiled successfully!"

# Build client
client: $(CLIENT_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Client compiled successfully!"

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(SERVER_OBJECTS) $(CLIENT_OBJECTS)
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo "Cleaned all object files and executables"
