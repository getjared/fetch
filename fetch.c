#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define MAX_CMD_LEN 256
#define MAX_OUTPUT_LEN 1024
#define BOX_WIDTH 23

#define CLEAR_SCREEN "\033[2J\033[H"
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define ITALIC "\033[3m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"

#define ASCII_ART_HEIGHT 3
const char *ASCII_ART[] = {
    "　(　  .∧_∧",
    "　 )　(｡・ω・)",
    "　旦 ι''o,,_）～"
};

char* get_command_output(const char* cmd);
char* get_wm();
char* get_shell();
char* get_linux_version();
char* get_resolution();
int get_visible_length(const char* str);
void print_padded(const char* str, int width);
void print_ascii_art(const char* str);

int main() {
    char wm_info[MAX_OUTPUT_LEN], shell_info[MAX_OUTPUT_LEN], linux_info[MAX_OUTPUT_LEN], res_info[MAX_OUTPUT_LEN];
    snprintf(wm_info, MAX_OUTPUT_LEN, "%s%swm:%s%s %s", BOLD, RED, RESET, ITALIC, get_wm());
    snprintf(shell_info, MAX_OUTPUT_LEN, "%s%ssh:%s%s %s", BOLD, GREEN, RESET, ITALIC, get_shell());
    snprintf(linux_info, MAX_OUTPUT_LEN, "%s%sver:%s%s %s", BOLD, YELLOW, RESET, ITALIC, get_linux_version());
    snprintf(res_info, MAX_OUTPUT_LEN, "%s%sres:%s%s %s", BOLD, BLUE, RESET, ITALIC, get_resolution());

    char *lines[] = {wm_info, shell_info, linux_info, res_info};

    printf(CLEAR_SCREEN);

    for (int i = 0; i < ASCII_ART_HEIGHT; i++) {
        print_ascii_art(ASCII_ART[i]);
        printf("\n");
    }

    printf(CYAN "╭");
    for (int i = 0; i < BOX_WIDTH - 2; i++) printf("─");
    printf("╮\n" RESET);

    for (int i = 0; i < 4; i++) {
        printf(CYAN "│ " RESET);
        print_padded(lines[i], BOX_WIDTH - 4);
        printf(CYAN " │\n" RESET);
    }

    printf(CYAN "╰");
    for (int i = 0; i < BOX_WIDTH - 2; i++) printf("─");
    printf("╯\n" RESET);

    return 0;
}

char* get_command_output(const char* cmd) {
    static char output[MAX_OUTPUT_LEN];
    FILE* fp = popen(cmd, "r");
    if (fp == NULL) {
        return "Unknown";
    }
    if (fgets(output, sizeof(output), fp) != NULL) {
        output[strcspn(output, "\n")] = 0;
    } else {
        strcpy(output, "Unknown");
    }
    pclose(fp);
    return output;
}

char* get_wm() {
    Display* d = XOpenDisplay(NULL);
    if (d == NULL) {
        return "Unknown";
    }
    
    Window root = DefaultRootWindow(d);
    Atom net_supporting_wm_check = XInternAtom(d, "_NET_SUPPORTING_WM_CHECK", False);
    Atom net_wm_name = XInternAtom(d, "_NET_WM_NAME", False);
    Atom utf8_string = XInternAtom(d, "UTF8_STRING", False);

    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;

    static char wm_name[256] = "Unknown";

    if (XGetWindowProperty(d, root, net_supporting_wm_check, 0, 1, False, XA_WINDOW, &actual_type, &actual_format, &nitems, &bytes_after, &data) == Success) {
        if (data) {
            Window child = *(Window*)data;
            XFree(data);
            
            if (XGetWindowProperty(d, child, net_wm_name, 0, 256, False, utf8_string, &actual_type, &actual_format, &nitems, &bytes_after, &data) == Success) {
                if (data) {
                    strncpy(wm_name, (char*)data, sizeof(wm_name) - 1);
                    wm_name[sizeof(wm_name) - 1] = '\0';
                    XFree(data);
                }
            }
        }
    }

    XCloseDisplay(d);
    return wm_name;
}

char* get_shell() {
    return getenv("SHELL");
}

char* get_linux_version() {
    return get_command_output("uname -r");
}

char* get_resolution() {
    Display* d = XOpenDisplay(NULL);
    if (d == NULL) {
        return "Unknown";
    }
    int screen = DefaultScreen(d);
    static char resolution[MAX_OUTPUT_LEN];
    snprintf(resolution, sizeof(resolution), "%dx%d", 
             DisplayWidth(d, screen), DisplayHeight(d, screen));
    XCloseDisplay(d);
    return resolution;
}

int get_visible_length(const char* str) {
    int len = 0;
    int in_escape = 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] == '\033') {
            in_escape = 1;
        } else if (in_escape && str[i] == 'm') {
            in_escape = 0;
        } else if (!in_escape) {
            len++;
        }
    }
    return len;
}

void print_padded(const char* str, int width) {
    int visible_len = get_visible_length(str);
    printf("%s", str);
    for (int i = visible_len; i < width; i++) {
        printf(" ");
    }
}

void print_ascii_art(const char* str) {
    printf("%s", str);
}