-- Auction System Database Schema

CREATE DATABASE IF NOT EXISTS auction_system;
USE auction_system;

-- Users table
CREATE TABLE IF NOT EXISTS users (
    user_id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    INDEX idx_username (username)
) ENGINE=InnoDB;

-- Rooms table
CREATE TABLE IF NOT EXISTS rooms (
    room_id INT AUTO_INCREMENT PRIMARY KEY,
    room_name VARCHAR(100) UNIQUE NOT NULL,
    owner_id INT NOT NULL,
    is_private BOOLEAN DEFAULT FALSE,
    password VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    FOREIGN KEY (owner_id) REFERENCES users(user_id),
    INDEX idx_room_name (room_name),
    INDEX idx_owner (owner_id)
) ENGINE=InnoDB;

-- Room members (many-to-many)
CREATE TABLE IF NOT EXISTS room_members (
    room_id INT NOT NULL,
    user_id INT NOT NULL,
    joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (room_id, user_id),
    FOREIGN KEY (room_id) REFERENCES rooms(room_id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
) ENGINE=InnoDB;

-- Items table
CREATE TABLE IF NOT EXISTS items (
    item_id INT AUTO_INCREMENT PRIMARY KEY,
    item_name VARCHAR(100) NOT NULL,
    room_id INT NOT NULL,
    owner_id INT NOT NULL,
    start_price DECIMAL(10,2) NOT NULL,
    current_price DECIMAL(10,2) NOT NULL,
    buy_now_price DECIMAL(10,2) NOT NULL,
    current_bidder_id INT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expires_at TIMESTAMP NOT NULL,
    is_sold BOOLEAN DEFAULT FALSE,
    FOREIGN KEY (room_id) REFERENCES rooms(room_id) ON DELETE CASCADE,
    FOREIGN KEY (owner_id) REFERENCES users(user_id),
    FOREIGN KEY (current_bidder_id) REFERENCES users(user_id),
    INDEX idx_room (room_id),
    INDEX idx_owner (owner_id),
    INDEX idx_expires (expires_at)
) ENGINE=InnoDB;

-- Bids table (history)
CREATE TABLE IF NOT EXISTS bids (
    bid_id INT AUTO_INCREMENT PRIMARY KEY,
    item_id INT NOT NULL,
    user_id INT NOT NULL,
    bid_amount DECIMAL(10,2) NOT NULL,
    bid_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (item_id) REFERENCES items(item_id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    INDEX idx_item (item_id),
    INDEX idx_user (user_id)
) ENGINE=InnoDB;

-- Activity logs
CREATE TABLE IF NOT EXISTS activity_logs (
    log_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT,
    action VARCHAR(50) NOT NULL,
    details TEXT,
    ip_address VARCHAR(45),
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE SET NULL,
    INDEX idx_user (user_id),
    INDEX idx_timestamp (timestamp)
) ENGINE=InnoDB;

-- Insert sample data
INSERT INTO users (username, password) VALUES 
    ('admin', 'admin123'),
    ('alice', 'pass123'),
    ('bob', 'pass456');

INSERT INTO rooms (room_name, owner_id) VALUES
    ('VIP_ROOM', 1),
    ('PUBLIC_ROOM', 2);
