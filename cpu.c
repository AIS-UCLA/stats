#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rrd.h>
#include <rrd_client.h>

void append_tok(char **val, int *len, int *pos, const char *tok) {
  int tok_len;
  if (tok == NULL) { exit(-1); }
  tok_len = strlen(tok);
  if (*len < *pos + tok_len + 1) {
    *val = realloc(*val, *len + 1024);
    *len += 1024;
  }
  memcpy(&(*val)[*pos], tok, tok_len);
  *pos += tok_len;
  (*val)[(*pos)++] = ':';
}

int submit_cpu(char *db) {
  FILE *f;
  char buf[1024];
  char *tok;
  int tok_len;
  int val_pos;
  int val_len = 1024;
  char *val = malloc(val_len);

  val[0] = 'N';
  val[1] = ':';
  val_pos = 2;


  if((f = fopen("/proc/stat", "r")) == NULL) {
    perror("failed to open /proc/stat");
    return -1;
  }

  while (fgets(buf, 1024, f) != NULL) {
    if (strncmp(buf, "cpu", 3)) continue;
    strtok(buf, " ");                                        /* cpuN */
    append_tok(&val, &val_len, &val_pos, strtok(NULL, " ")); /* user */
    strtok(NULL , " ");                                      /* nice */
    append_tok(&val, &val_len, &val_pos, strtok(NULL, " ")); /* system */
    append_tok(&val, &val_len, &val_pos, strtok(NULL, " ")); /* idle */
  }
  val[val_pos - 1] = '\0';

  if (rrdc_update(db, 1, (const char * const *)&val)) {
    perror("rrdtool update failed");
    return -1;
  }
  free(val);
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

  if (submit_cpu(argv[2])) return -1;

  return rrdc_disconnect();
}


