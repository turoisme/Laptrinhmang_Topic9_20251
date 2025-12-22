#include "response_handler.h"
void parse_and_display_response(const char *response) {
    int code;
    char message[BUFF_SIZE];
    if (sscanf(response, "%d %[^\n]", &code, message) >= 1) {
        printf("[Response %d] ", code);
        if (sscanf(response, "%d %[^\n]", &code, message) == 2) {
            printf("%s\n", message);
        } else {
            printf("OK\n");
        }
    } else {
        printf("[Unknown response] %s\n", response);
    }
}
