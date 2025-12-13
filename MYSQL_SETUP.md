# MySQL Setup Guide for Auction System

## 1. Install MySQL (WSL Ubuntu)

```bash
# Update package list
sudo apt update

# Install MySQL Server and development libraries
sudo apt install mysql-server libmysqlclient-dev

# Start MySQL service
sudo service mysql start

# Check status
sudo service mysql status
```

## 2. Configure MySQL

```bash
# Login as root (no password initially)
sudo mysql

# In MySQL prompt:
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'your_password';
FLUSH PRIVILEGES;
EXIT;
```

## 3. Create Database and Tables

```bash
# Run the schema file
mysql -u root -p < schema.sql

# Verify
mysql -u root -p
```

In MySQL prompt:
```sql
USE auction_system;
SHOW TABLES;
SELECT * FROM users;
```

## 4. Update database.h

Edit `Server/database.h` and change:
```c
#define DB_PASS "your_password"  // Your MySQL root password
```

## 5. Compile and Run

```bash
# Clean and rebuild
make clean
make server

# Run server
./run_server 5500
```

## Expected Output:

```
Connecting to MySQL database...
Initializing MySQL connection pool...
MySQL pool initialized with 10 connections
Database connected successfully!
Server started at the port 5500
```

## Troubleshooting:

### Error: "Can't connect to local MySQL server"
```bash
sudo service mysql start
```

### Error: "Access denied for user 'root'"
```bash
sudo mysql
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'newpassword';
FLUSH PRIVILEGES;
```

### Error: "Unknown database 'auction_system'"
```bash
mysql -u root -p < schema.sql
```

### Error: "mysql_config: command not found"
```bash
sudo apt install libmysqlclient-dev
```

## Auto-start MySQL on WSL boot:

Add to `~/.bashrc`:
```bash
if ! service mysql status > /dev/null 2>&1; then
    sudo service mysql start
fi
```

## Sample Queries:

```sql
-- View all users
SELECT * FROM users;

-- View all rooms
SELECT r.room_name, u.username as owner FROM rooms r
JOIN users u ON r.owner_id = u.user_id;

-- View active items
SELECT i.item_name, r.room_name, i.current_price, i.expires_at 
FROM items i 
JOIN rooms r ON i.room_id = r.room_id 
WHERE i.is_sold = 0;

-- View bid history
SELECT b.bid_amount, u.username, b.bid_time, i.item_name
FROM bids b
JOIN users u ON b.user_id = u.user_id
JOIN items i ON b.item_id = i.item_id
ORDER BY b.bid_time DESC;
```
