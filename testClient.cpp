#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#define PORT     8080
#define MAXLINE  1024

typedef struct input_s {
    uint8_t up : 1;
    uint8_t down : 1;
    uint8_t left : 1;
    uint8_t right : 1;
    uint8_t shoot : 1;
} input_t;

uint8_t toByte(input_t input)
{
    uint8_t byte = 0;
    byte |= (input.up    & 0x01) << 0;
    byte |= (input.down  & 0x01) << 1;
    byte |= (input.left  & 0x01) << 2;
    byte |= (input.right & 0x01) << 3;
    byte |= (input.shoot & 0x01) << 4;
    return byte;
}

/************ terminal helpers ************/

static struct termios orig_termios;

void disable_raw_mode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO); // no canonical mode, no echo
    raw.c_cc[VMIN] = 0;  // non-blocking read
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    // also set stdin non-blocking
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

/************ key handling ************/

void update_input_from_keyboard(input_t *input)
{
    unsigned char c;
    // read all available bytes (non-blocking)
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == 0x1B) { // ESC sequence: arrow keys are ESC [ A/B/C/D
            unsigned char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) return;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) return;

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': // Up arrow
                        input->up = 1;
                        input->down = 0;
                        input->left = 0;
                        input->right = 0;
                        break;
                    case 'B': // Down arrow
                        input->down = 1;
                        input->up = 0;
                        input->left = 0;
                        input->right = 0;
                        break;
                    case 'C': // Right arrow
                        input->right = 1;
                        input->down = 0;
                        input->up = 0;
                        input->left = 0;
                        break;
                    case 'D': // Left arrow
                        input->left = 1;
                        input->down = 0;
                        input->up = 0;
                        input->right = 0;
                        break;
                    default:
                        input->up = 0;
                        input->down = 0;
                        input->left = 0;
                        input->right = 0;
                        break;
                }
            }
        } else if (c == ' ') {
            // Spacebar as shoot toggle
            input->shoot = 1;
        } else if (c == 'q') {
            // optional: 'q' to quit
            disable_raw_mode();
            exit(0);
        } else {
            // other keys reset movement
            input->up = 0;
            input->down = 0;
            input->left = 0;
            input->right = 0;
            input->shoot = 0;
        }
    }

    // Here you could also "decay" inputs, e.g. reset shoot after one frame:
    // input->shoot = 0;
}

// Driver code
int main() {
    int sockfd;
    uint8_t payload;
    input_t input = {0, 0, 0, 0, 0};
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // send to localhost

    enable_raw_mode();

    while (1) {
        // update input according to pressed keys
        update_input_from_keyboard(&input);

        // convert struct to byte
        payload = toByte(input);

        // send packet
        sendto(sockfd, &payload, sizeof(payload),
               MSG_CONFIRM, (const struct sockaddr *) &servaddr,
               sizeof(servaddr));

        // simple "frame" delay so you don't spam at max speed
        usleep(16000); // ~60 FPS
    }

    disable_raw_mode();
    close(sockfd);
    return 0;
}
