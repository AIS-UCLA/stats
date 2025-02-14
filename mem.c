#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rrd.h>
#include <rrd_client.h>

int submit_mem(char *db) {
  FILE *f;
  char buf[1024];
  unsigned long total = 0, free = 0, avail = 0;
  char val[128];
  const char *values[1];

  if((f = fopen("/proc/meminfo", "r")) == NULL) {
    perror("failed to open /proc/meminfo");
    return -1;
  }

  while (fgets(buf, 1024, f) != NULL) {
    sscanf(buf, "MemTotal: %lu kB", &total);
    sscanf(buf, "MemFree: %lu kB", &free);
    sscanf(buf, "MemAvailable: %lu kB", &avail);
    if (total != 0 && free != 0 && avail != 0) break;
  }

  fclose(f);

  if (snprintf(val, 128, "N:%lu:%lu:%lu", total, free, avail) < 0) {
    perror("failed to format update");
    return -1;
  }

  values[0] = val;

  if (rrdc_update(db, 1, (const char * const *)values)) {
    perror("rrdtool update failed");
    return -1;
  }
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("usage: %s <rrdcached socket> <database>\n", argv[0]);
    return 1;
  }

  if (rrdc_connect(argv[1])) {
    printf("failed to connect to rrdcached at %s\n", argv[1]);
    return -1;
  }

  if (submit_mem(argv[2])) return -1;

  return rrdc_disconnect();
}


