#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fts.h>
#include <errno.h>
#include <locale.h>
#include <ctype.h>

// Function prototypes
void print_help();
void print_version();
int search_in_file(const char *filepath, const char *search_string);
void debug_print(const char *message);

// Global debug flag
int debug_enabled = 0;

int main(int argc, char *argv[]) {
    // Set locale to handle UTF-8 strings
    setlocale(LC_ALL, "en_US.UTF-8");

    // Command-line options
    int opt;
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    // Parse command-line options
    while ((opt = getopt_long(argc, argv, "hv", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return 0;
            case 'v':
                print_version();
                return 0;
            default:
                print_help();
                return 1;
        }
    }

    // Check for required arguments
    if (argc - optind < 2) {
        fprintf(stderr, "Error: Directory and search target are required.\n");
        print_help();
        return 1;
    }

    // Get directory and search target
    char *directory = argv[optind];
    char *search_target = argv[optind + 1];

    // Check if debug mode is enabled
    if (getenv("LAB11DEBUG")) {
        debug_enabled = 1;
        debug_print("Debug mode enabled.");
    }

    // Open directory for traversal
    char *paths[] = {directory, NULL};
    FTS *fts = fts_open(paths, FTS_NOCHDIR | FTS_PHYSICAL, NULL);
    if (!fts) {
        perror("fts_open");
        return 1;
    }

    // Traverse directory
    FTSENT *entry;
    while ((entry = fts_read(fts))) {
        if (entry->fts_info == FTS_F) { // Regular file
            if (search_in_file(entry->fts_path, search_target)) {
                printf("%s\n", entry->fts_path);
            }
        } else if (entry->fts_info == FTS_DNR || entry->fts_info == FTS_ERR) {
            fprintf(stderr, "Error accessing %s: %s\n", entry->fts_path, strerror(entry->fts_errno));
        }
    }

    // Clean up
    fts_close(fts);
    return 0;
}

// Print help information
void print_help() {
    printf("Usage: lab1ofbN3250 [options] directory search_target\n");
    printf("Options:\n");
    printf("  -h, --help\t\tShow this help message and exit\n");
    printf("  -v, --version\t\tShow version information and exit\n");
}

// Print version information
void print_version() {
    printf("lab1ofbN3250 version 5.0\n");
    printf("Author: огом фэйт блессинг, Group N3250\n");
}

// Search for a string in a file
int search_in_file(const char *filepath, const char *search_string) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Error opening %s: %s\n", filepath, strerror(errno));
        return 0;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read file content
    char *buffer = malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(file);
        return 0;
    }
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    fclose(file);

    // Search for the string
    char *found = strstr(buffer, search_string);
    if (found && debug_enabled) {
        debug_print("Found search string in file.");
    }

    free(buffer);
    return found != NULL;
}

// Print debug messages
void debug_print(const char *message) {
    if (debug_enabled) {
        fprintf(stderr, "[DEBUG] %s\n", message);
    }
}
