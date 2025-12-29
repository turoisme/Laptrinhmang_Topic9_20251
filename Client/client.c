#include "client.h"

void print_home_menu() {
    printf("\n===== HOME MENU =====\n");
    printf("1. REGISTER - Register new account\n");
    printf("2. LOGIN - Login to account\n");
    printf("0. EXIT - Disconnect and exit\n");
    printf("=====================\n");
    printf("Enter choice: ");
}

void print_room_menu() {
    printf("\n===== ROOM MENU =====\n");
    printf("1. CREATE_ROOM - Create auction room\n");
    printf("2. JOIN_ROOM - Join auction room\n");
    printf("3. LIST_ROOMS - List all rooms\n");
    printf("4. LOGOUT - Logout from account\n");
    printf("0. EXIT - Disconnect and exit\n");
    printf("=====================\n");
    printf("Enter choice: ");
}

void print_item_menu() {
    printf("\n===== ITEM MENU (IN ROOM) =====\n");
    printf("1. CREATE_ITEM - Create auction item\n");
    printf("2. DELETE_ITEM - Delete item\n");
    printf("3. LIST_ITEMS - List items in room\n");
    printf("4. BID - Place bid on item\n");
    printf("5. BUY - Buy item now\n");
    printf("6. LEAVE_ROOM - Leave current room\n");
    printf("7. LOGOUT - Logout from account\n");
    printf("0. EXIT - Disconnect and exit\n");
    printf("================================\n");
    printf("Enter choice: ");
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(int argc, char *argv[]) {
    // Check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        fprintf(stderr, "Example: %s 127.0.0.1 5500\n", argv[0]);
        return 1;
    }
    
    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Invalid port number: %d\n", port);
        return 1;
    }
    
    // Initialize connection
    ClientConnection conn;
    memset(&conn, 0, sizeof(ClientConnection));
    
    // Connect to server
    printf("Connecting to %s:%d...\n", server_ip, port);
    if (connect_to_server(&conn, server_ip, port) < 0) {
        fprintf(stderr, "Failed to connect to server\n");
        return 1;
    }
    
    // Main loop
    int choice;
    char username[100], password[100], room_name[100], item_name[100];
    int start_price, buy_now_price, bid_amount;
    char *response = NULL;
    int is_logged_in = 0;
    int is_in_room = 0;
    
    while (is_connected(&conn)) {
        if (!is_logged_in) {
            print_home_menu();
        } else if (!is_in_room) {
            print_room_menu();
        } else {
            print_item_menu();
        }
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("Invalid input!\n");
            continue;
        }
        clear_input_buffer();
        
        // Home menu 
        if (!is_logged_in) {
            switch (choice) {
                case 1: //register
                    printf("Username: ");
                    scanf("%99s", username);
                    printf("Password: ");
                    scanf("%99s", password);
                    clear_input_buffer();
                    
                    if (cmd_register(&conn, username, password) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            free(response);
                        }
                    }
                    break;
                    
                case 2: // login
                    printf("Username: ");
                    scanf("%99s", username);
                    printf("Password: ");
                    scanf("%99s", password);
                    clear_input_buffer();
                    
                    if (cmd_login(&conn, username, password) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            int code;
                            char msg[BUFF_SIZE];
                            if (sscanf(response, "%d %[^\n]", &code, msg) >= 1) {
                                parse_and_display_response(response);
                                if (code == LOGIN_SUCCESS) {
                                    is_logged_in = 1;
                                    printf("You are now logged in!\n");
                                }
                            }
                            free(response);
                        }
                    }
                    break;
                    
                case 0: // exit
                    printf("Disconnecting...\n");
                    disconnect_from_server(&conn);
                    break;
                    
                default:
                    printf("Invalid choice!\n");
                    break;
            }
        }
        // Room menu
        else if (!is_in_room) {
            switch (choice) {
                case 1: //create room
                    printf("Room name: ");
                    scanf("%99s", room_name);
                    clear_input_buffer();
                    
                    if (cmd_create_room(&conn, room_name) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            free(response);
                        }
                    }
                    break;
                    
                case 2: // join 
                    printf("Room name: ");
                    scanf("%99s", room_name);
                    clear_input_buffer();
                    
                    if (cmd_join_room(&conn, room_name) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            int code;
                            char msg[BUFF_SIZE];
                            if (sscanf(response, "%d %[^\n]", &code, msg) >= 1) {
                                parse_and_display_response(response);
                                if (code == JOIN_OK) {
                                    is_in_room = 1;
                                    printf("You joined the room!\n");
                                    if (!is_notification_running()) {
                                        start_notification_thread(&conn);
                                        printf("[Notification system started]\n");
                                    }
                                }
                            }
                            free(response);
                        }
                    }
                    break;
                    
                case 3: // list
                    if (cmd_list_rooms(&conn) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            free(response);
                        }
                    }
                    break;
                    
                case 4: 
                     if (cmd_logout(&conn) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            is_logged_in = 0;
                            is_in_room = 0;
                            printf("Logged out!\n");
                            // Stop notification thread
                            stop_notification_thread();
                            free(response);
                        }
                    }
                    break;
                    
                case 0: //
                    printf("Disconnecting...\n");
                    disconnect_from_server(&conn);
                    break;
                    
                default:
                    printf("Invalid choice!\n");
                    break;
            }
        }
        // item menu
        else {
            switch (choice) {
                case 1: // create item
                    printf("Item name: ");
                    scanf("%99s", item_name);
                    printf("Start price: ");
                    scanf("%d", &start_price);
                    printf("Buy now price: ");
                    scanf("%d", &buy_now_price);
                    clear_input_buffer();
                    
                    if (cmd_create_item(&conn, item_name, start_price, buy_now_price) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            free(response);
                        }
                    }
                    break;
                    
                case 2: // delete item
                    printf("Item name: ");
                    scanf("%99s", item_name);
                    clear_input_buffer();
                    
                    if (cmd_delete_item(&conn, item_name) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            free(response);
                        }
                    }
                    break;
                    
                case 3: // list item
                    if (cmd_list_items(&conn) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            free(response);
                        }
                    }
                    break;
                    
                case 4: //bid
                    printf("Item name: ");
                    scanf("%99s", item_name);
                    printf("Bid amount: ");
                    scanf("%d", &bid_amount);
                    clear_input_buffer();
                    
                    if (cmd_bid(&conn, item_name, bid_amount) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            free(response);
                        }
                    }
                    break;
                    
                case 5: // buy
                    printf("Item name: ");
                    scanf("%99s", item_name);
                    clear_input_buffer();
                    
                    if (cmd_buy(&conn, item_name) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            free(response);
                        }
                    }
                    break;
                    
                case 6: // leave room
                    if (cmd_leave_room(&conn) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            is_in_room = 0;
                            printf("You left the room!\n");
                            // Stop notification thread
                            stop_notification_thread();
                            free(response);
                        }
                    }
                    break;
                    
                case 7: // logout
                    if (cmd_logout(&conn) > 0) {
                        if (receive_response(&conn, &response) > 0) {
                            parse_and_display_response(response);
                            is_logged_in = 0;
                            is_in_room = 0;
                            printf("Logged out!\n");
                            // Stop notification thread
                            stop_notification_thread();
                            free(response);
                        }
                    }
                    break;
                    
                case 0: // exit
                    printf("Disconnecting...\n");
                    disconnect_from_server(&conn);
                    break;
                    
                default:
                    printf("Invalid choice!\n");
                    break;
            }
        }
    }
    
    printf("Client terminated.\n");
    return 0;
}
