#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

void set_color_red()   { fprintf(stdout, "\033[0;31m"); }
void set_color_green() { fprintf(stdout, "\033[0;32m"); }
void set_color_white() { fprintf(stdout, "\033[0;37m"); }
void set_color_dim()   { fprintf(stdout, "\033[2;37m"); }
void reset_color()     { fprintf(stdout, "\033[0m");    }

static struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

typedef enum { UNTYPED, CORRECT, WRONG } Result;

static const char *word_pool[] = {
    "the", "be", "to", "of", "and", "a", "in", "that", "have", "it",
    "for", "not", "on", "with", "he", "as", "you", "do", "at", "this",
    "but", "his", "by", "from", "they", "we", "say", "her", "she", "or",
    "an", "will", "my", "one", "all", "would", "there", "their", "what",
    "so", "up", "out", "if", "about", "who", "get", "which", "go", "me",
    "when", "make", "can", "like", "time", "no", "just", "him", "know",
    "take", "people", "into", "year", "your", "good", "some", "could",
    "them", "see", "other", "than", "then", "now", "look", "only", "come",
    "its", "over", "think", "also", "back", "after", "use", "two", "how",
    "our", "work", "first", "well", "way", "even", "new", "want", "any",
    "these", "give", "day", "most", "us", "great", "between", "need",
    "large", "often", "hand", "high", "place", "hold", "real", "life",
    "few", "north", "open", "seem", "together", "next", "white", "children",
    "begin", "got", "walk", "example", "ease", "paper", "group", "always",
    "music", "those", "both", "mark", "book", "letter", "until", "mile",
    "river", "car", "feet", "care", "second", "enough", "plain", "girl",
    "usual", "young", "ready", "above", "ever", "red", "list", "though",
    "feel", "talk", "bird", "soon", "body", "dog", "family", "direct",
    "pose", "leave", "song", "measure", "door", "product", "black", "short",
};
static const int pool_size = sizeof(word_pool) / sizeof(word_pool[0]);

#define NUM_LINES    3
#define LINE_MAX_LEN 80
#define WORDS_PER_LINE 8

void build_random_line(char *buf) {
    buf[0] = '\0';
    int remaining = WORDS_PER_LINE;
    while (remaining > 0) {
        const char *word = word_pool[rand() % pool_size];
        /* +1 for the space (or null on first word) */
        if (strlen(buf) + strlen(word) + 1 >= LINE_MAX_LEN) continue;
        if (buf[0] != '\0') strcat(buf, " ");
        strcat(buf, word);
        remaining--;
    }
}

void render_line(const char *line, Result *results, int len, int cursor) {
    printf("\r\033[K");
    for (int i = 0; i < len; i++) {
        switch (results[i]) {
            case CORRECT: set_color_green(); break;
            case WRONG:   set_color_red();   break;
            case UNTYPED:
                if (i == cursor) set_color_white();
                else             set_color_dim();
                break;
        }
        putchar(line[i]);
    }
    reset_color();
    fflush(stdout);
}

int main() {
    srand((unsigned)time(NULL));
    enable_raw_mode();

    int total_correct = 0;
    int total_wrong   = 0;

    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    for (int l = 0; l < NUM_LINES; l++) {
        char line[LINE_MAX_LEN];
        build_random_line(line);
        int len = strlen(line);

        Result results[LINE_MAX_LEN];
        memset(results, UNTYPED, sizeof(results));
        int cursor = 0;

        render_line(line, results, len, cursor);

        while (1) {
            int c = getchar();
            if (c == EOF) goto done;
            if (c == 3) { printf("\n"); goto done; }

            if (c == '\r' || c == '\n') {
                for (int i = cursor; i < len; i++) total_wrong++;
                break;
            }

            if (c == 127 || c == '\b') {
                if (cursor > 0) {
                    cursor--;
                    results[cursor] = UNTYPED;
                    render_line(line, results, len, cursor);
                }
                continue;
            }

            if (cursor < len) {
                if (c == line[cursor]) {
                    results[cursor] = CORRECT;
                    total_correct++;
                } else {
                    results[cursor] = WRONG;
                    total_wrong++;
                }
                cursor++;
                render_line(line, results, len, cursor);
            }
        }

        render_line(line, results, len, -1);
        printf("\n");
    }

done:
    clock_gettime(CLOCK_MONOTONIC, &t_end);

    double elapsed = (t_end.tv_sec  - t_start.tv_sec)
                   + (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    int    total    = total_correct + total_wrong;
    double accuracy = total > 0 ? (100.0 * total_correct / total) : 0.0;
    double wpm      = (total_correct / 5.0) / (elapsed / 60.0);

    printf("\n");
    set_color_white();  printf("--- results ---\n"); reset_color();
    set_color_green();  printf("correct   : %d\n",   total_correct);
    set_color_red();    printf("incorrect : %d\n",   total_wrong);
    reset_color();      printf("accuracy  : %.1f%%\n", accuracy);
                        printf("time      : %.2fs\n",  elapsed);
    set_color_green();  printf("wpm       : %.1f\n",  wpm);
    reset_color();

    return 0;
}