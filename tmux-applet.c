#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/statvfs.h>

////////////////////////////////////////////////////////////////////////////////
// Utility functions

char SIZE_SUFFIXES[] = {'B', 'k', 'M', 'G', 'T'};
#define BYTES      0
#define KILOBYTES  1
#define MEGABYTES  2
#define GIGABYTES  3
#define TERRABYTES 4

/**
 * Print a size in human-readable form
 * @param size the size
 * @param magnitude order of magnitude of size (0 for bytes, 1 for kilobytes,
 *        etc., maximum 4 for terabytes)
 */
void print_size(unsigned long size, int magnitude) {
    assert(magnitude >= 0 && magnitude <= 4);
    if(magnitude == 4 || size < 1024L) {
        printf("%lu%c", size, SIZE_SUFFIXES[magnitude]);
    } else if(size < 10*1024L) {
        printf("%lu.%lu%c", size / 1024L, (size % 1024L) / 103L,
                            SIZE_SUFFIXES[magnitude+1]);
    } else {
        print_size(size / 1024L, magnitude + 1);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Applets

/**
 * Print current load
 */
void print_load() {
    FILE *f;
    char load[11];
    int n;
    
    f = fopen("/proc/loadavg", "r");
    n = fscanf(f, "%10s", load);
    if(n == EOF || n != 1)
        return;
    
    printf("%s", load);
}

/**
 * Print available RAM
 */
void print_meminfo() {
    FILE* f;
    char key[31];
    int n, toRead;
    unsigned long value, memTotal, memFree, memBuffers, memCached;
    
    f = fopen("/proc/meminfo", "r");
    toRead = 4;
    while((n = fscanf(f, "%30s %lu kB", key, &value)) != EOF) {
        if(n != 2)
            continue;
        if(strcmp(key, "MemTotal:") == 0)
            memTotal = value;
        else if(strcmp(key, "MemFree:") == 0)
            memFree = value;
        else if(strcmp(key, "Buffers:") == 0)
            memBuffers = value;
        else if(strcmp(key, "Cached:") == 0)
            memCached = value;
        else
            continue;
        toRead--;
        if(toRead == 0)
            break;
    }
    fclose(f);
    
    memFree += memBuffers + memCached;
    printf("%lu%%,", (memTotal - memFree) * 100 / memTotal);
    print_size(memFree, KILOBYTES);
}

void print_diskfree(const char* path) {
    struct statvfs s;
    unsigned long diskTotal, diskFree;
    
    if(statvfs(path, &s))
        return;
    diskTotal = s.f_blocks * s.f_frsize;
    diskFree = s.f_bavail * s.f_bsize;
    
    printf("%lu%%,", (diskTotal - diskFree) * 100 / diskTotal);
    print_size(diskFree, BYTES);
}

////////////////////////////////////////////////////////////////////////////////
// Main program

/**
 * Print usage notice
 * @param progname name of this program
 */
int print_usage(char* progname) {
    fprintf(stderr, "Usage: %s {l,m,d}\n", progname);
    return 1;
}

int main(int argc, char *argv[]) {
    if(argc != 2)
        return print_usage(argv[0]);

    switch(argv[1][0]) {
    case 'l':  print_load();             break;
    case 'm':  print_meminfo();          break;
    case 'd':  print_diskfree("/home");  break;
    default:
        return print_usage(argv[0]);
    }
    printf("\n");
    return 0;
}
