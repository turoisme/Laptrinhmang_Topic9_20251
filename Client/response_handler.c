#include "response_handler.h"
void parse_and_display_response(const char *response) {
    int code;
    char message[BUFF_SIZE];
    if (sscanf(response, "%d", &code) >= 1) {
        printf("[Response %d] ", code);
        
        // Find the start of the message (after code and space)
        const char *msg_start = strchr(response, ' ');
        if (msg_start) {
            msg_start++; // Skip the space
            // Print the entire message including newlines
            printf("%s", msg_start);
            // Add newline if message doesn't end with one
            if (msg_start[strlen(msg_start)-1] != '\n') {
                printf("\n");
            }
        } else {
            printf("OK\n");
        }
    } else {
        printf("[Unknown response] %s\n", response);
    }
}
