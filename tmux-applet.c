/* tmux-applet
 * Copyright 2011-2013 Vianney le Clément de Saint-Marcq <vleclement@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with This program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/param.h>
#include <sys/statvfs.h>

#ifdef BSD
#include <unistd.h>
#include <sys/sysctl.h>
#include <vm/vm_param.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// Utility functions

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifdef BSD
/**
 * Get a value from the BSD sysctl interface.
 * @param name the name of the value to query
 * @param buffer output buffer
 * @param len length of the parameter
 * @return TRUE if successful, FALSE otherwise
 */
int getsysctl(const char* name, void* buffer, size_t len) {
    size_t newlen = len;
    if(sysctlbyname(name, buffer, &newlen, NULL, 0) != 0) {
        perror("sysctl");
        return FALSE;
    }
    if(newlen != len) {
        fprintf(stderr, "sysctl: expected size %lu, got %lu\n", len, newlen);
        return FALSE;
    }
    return TRUE;
}
#endif

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
void print_size(unsigned long long size, int magnitude) {
    assert(magnitude >= 0 && magnitude <= 4);
    if(magnitude == TERRABYTES || size < 1024) {
        printf("#[bright]%llu#[nobright]%c", size, SIZE_SUFFIXES[magnitude]);
    } else if(size < 10*1024) {
        printf("#[bright]%llu.%llu#[nobright]%c", size >> 10, (size & 1023) / 103,
                              SIZE_SUFFIXES[magnitude+1]);
    } else {
        print_size(size >> 10, magnitude + 1);
    }
}

/**
 * Number of applets (really) printed so far
 */
int nbApplets = 0;

/**
 * Begin printing of an applet.
 *
 * @param attr the attributes specified in the configuration file, or "" if none
 * @param defattr the default attributes if attr == ""
 */
void begin_applet(const char* attr, const char* defattr) {
    if(attr[0] == '\0')
        attr = defattr;
    if(nbApplets != 0)
        printf("  ");
    printf("#[bright,%s]", attr);
}

/**
 * End printing of an applet.
 */
void end_applet() {
    printf("#[default]");
    nbApplets++;
}

////////////////////////////////////////////////////////////////////////////////
// Applets

/**
 * Name: load
 * Parameters: (none)
 * Default attributes: fg=yellow
 * Description: print current load
 */
void applet_load(FILE* fconf, const char* attributes) {
    double load;

    if(getloadavg(&load, 1) != 1)
        return;

    begin_applet(attributes, "fg=yellow");
    printf("%.2f", load);
    end_applet();
}

/**
 * Name: memory
 * Parameters: (none)
 * Default attributes: fg=green
 * Description: print used RAM percentage and free RAM amount
 */
void applet_memory(FILE* fconf, const char* attributes) {
    unsigned long memTotal = 0, memFree = 0;

#ifdef BSD

    unsigned inactiveCount, cacheCount, freeCount;

    if(!getsysctl("hw.physmem", &memTotal, sizeof(memTotal)))
        return;
    if(!getsysctl("vm.stats.vm.v_inactive_count",
                  &inactiveCount, sizeof(inactiveCount)))
        return;
    if(!getsysctl("vm.stats.vm.v_cache_count",
                  &cacheCount, sizeof(cacheCount)))
        return;
    if(!getsysctl("vm.stats.vm.v_free_count",
                  &freeCount, sizeof(freeCount)))
        return;

    memTotal /= 1024;
    memFree = inactiveCount + cacheCount + freeCount;
    memFree = memFree * getpagesize() / 1024;

#else

    FILE* f;
    char key[31];
    int n, toRead;
    unsigned long value;
    unsigned long memBuffers = 0, memCached = 0;

    f = fopen("/proc/meminfo", "r");
    if(f == NULL)
        return;

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

#endif

    if(memTotal == 0)
        return;

    begin_applet(attributes, "fg=green");
    printf("%lu#[nobright]%%,", (memTotal - memFree) * 100 / memTotal);
    print_size(memFree, KILOBYTES);
    end_applet();
}

/**
 * Name: disk
 * Parameters: path
 * Default attributes: fg=magenta
 * Description: print used percentage of the filesystem mounted at path and
 *     the free disk amount
 */
void applet_disk(FILE* fconf, const char* attributes) {
    char path[128];
    struct statvfs s;

    path[0] = '\0';
    if(fscanf(fconf, "%127s", path) != 1 || statvfs(path, &s)) {
        fprintf(stderr, "Unable to statvfs path '%s'\n", path);
        return;
    }

    begin_applet(attributes, "fg=magenta");
    printf("%llu#[nobright]%%,",
           (s.f_blocks - s.f_bfree) * 100ULL / s.f_blocks);
    print_size((unsigned long long) s.f_bavail * s.f_frsize, BYTES);
    end_applet();
}

/**
 * Name: raid
 * Parameters: (none)
 * Default attributes: bg=red
 * Description: show RAID when a raid array is degraded
 */
void applet_raid(FILE* fconf, const char* attributes) {
    FILE* f;
    char line[1024], *match;
    int degraded;

    f = fopen("/proc/mdstat", "r");
    if(f == NULL)
        return;

    degraded = FALSE;
    while(fgets(line, 1024, f) != NULL) {
        if(strstr(line, " blocks ") != NULL) {
            match = strrchr(line, '[');
            if(match == NULL)
                continue;
            if(strchr(match + 1, '_') != NULL) {
                degraded = TRUE;
                break;
            }
        }
    }
    fclose(f);

    if(degraded) {
        begin_applet(attributes, "bg=red");
        printf("RAID");
        end_applet();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Main program

int main(int argc, char *argv[]) {
    FILE *fconf;
    char applet[128], attributes[128];
    int c, len;
#ifndef DEBUG
    char filename[MAXPATHLEN];
#endif

#ifdef DEBUG
    fconf = fopen("tmux-applet.conf", "r");
#else
    strcpy(filename, getenv("HOME"));
    strcat(filename, "/.tmux-applet.conf");
    fconf = fopen(filename, "r");
    if(fconf == NULL)
        fconf = fopen("/etc/tmux-applet.conf", "r");
#endif

    if(fconf == NULL) {
        fprintf(stderr, "Unable to open configuration file.\n");
        exit(1);
    }

    attributes[0] = '\0';
    while(fscanf(fconf, "%127s", applet) == 1) {
        switch(applet[0]) {
        case '#': // comment
            do {
                c = getc(fconf);
            } while(c != EOF && c != '\n');
            break;
        case '[': // specify attributes
            len = strlen(applet);
            if(applet[len-1] != ']') {
                fprintf(stderr, "Invalid attributes specification.\n");
                exit(2);
            }
            memcpy(attributes, applet + 1, (len - 2)*sizeof(char));
            attributes[len-2] = '\0';
            break;
        default: // applet
            if(strcmp(applet, "load") == 0) {
                applet_load(fconf, attributes);
            } else if(strcmp(applet, "memory") == 0) {
                applet_memory(fconf, attributes);
            } else if(strcmp(applet, "disk") == 0) {
                applet_disk(fconf, attributes);
            } else if(strcmp(applet, "raid") == 0) {
                applet_raid(fconf, attributes);
            } else {
                fprintf(stderr, "Unknown applet '%s'.\n", applet);
            }
            attributes[0] = '\0'; // reset attributes
        }
    }

    fclose(fconf);
    printf("\n");
    return 0;
}

/* vim:set sw=4 ts=4 et: */
