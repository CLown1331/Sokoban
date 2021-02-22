#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int started = 0;

#define print(format, args...) \
    fprintf(output, format, ## args); \ 
    fprintf(stdout, format, ## args)

void process(FILE *input, FILE *output) {
    char *line;
    size_t len = 0;
    ssize_t nread;
    char row[25];
    memset(row, 0, sizeof row);
    while ((nread = getline(&line, &len, input)) != -1 && len > 0 && line[0] != '~') {
        for (int i = 0; line[i]; i++) {
            switch (line[i])
            {
            case '#':
                row[i] = 1;
                break;
            case '$':
                row[i] = 2;
                break;
            case '.':
                row[i] = 3;
                break;
            case '@':
                row[i] = 4;
                break;
            default:
                row[i] = 0;
                break;
            }
        }
        for (int i = 0; i < 20; i++) {
            if (started) {
                print(",");
            }
            started = 1;
            print(" 0x%02hhX", row[i]);
        }
    }
    free(line);
}

int main(int argc, char** argv) {
    int limit = atoi(argv[1]);
    FILE *input = fopen("levels", "r+");
    FILE *output = fopen("levels.c", "w+");
    char *line;
    size_t len = 0;
    ssize_t nread;
    int level_count = 0;
    print("unsigned char levels[] = {");
    while ((nread = getline(&line, &len, input)) != -1 && level_count < limit) {
        process(input, output);
        level_count++;
    }
    print("};\n");
    print("unsigned char level_index[] = {");
    for (int i = 0; i < level_count; i++) {
        if (i != 0) {
            print(",");
        }
        print(" %d", i * 360);
    }
    print("};\n");
    free(line);
    fclose(input);
    fclose(output);
    return 0;
}