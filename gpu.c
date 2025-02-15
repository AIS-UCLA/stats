#include <stdio.h>

#include <rrd.h>
#include <rrd_client.h>

#include <nvml.h>

#define MAX_GPUS 16

int submit_gpu(char *db) {
  nvmlReturn_t res;
  unsigned int devcnt, i;

  char val[32 * MAX_GPUS] = "N";
  int idx = 1;
  const char *values[1];

  if ((res = nvmlDeviceGetCount(&devcnt)) != NVML_SUCCESS) goto nvml_err;
  if (devcnt > MAX_GPUS) {
    printf("you say you have %d GPUs, but I don't believe you\n", devcnt);
    return -1;
  }

  for (i = 0; i < devcnt; i++) {
    nvmlDevice_t dev;
    nvmlUtilization_t util;
    nvmlMemory_t mem;
    unsigned int pow;
    int rc;

    if ((res = nvmlDeviceGetHandleByIndex_v2(i, &dev)) != NVML_SUCCESS) goto nvml_err;
    if ((res = nvmlDeviceGetUtilizationRates(dev, &util)) != NVML_SUCCESS) goto nvml_err;
    if ((res = nvmlDeviceGetMemoryInfo(dev, &mem)) != NVML_SUCCESS) goto nvml_err;
    if ((res = nvmlDeviceGetPowerUsage(dev, &pow)) != NVML_SUCCESS) goto nvml_err;

    if ((rc = snprintf(&val[idx], 128, ":%d:%llu:%u", util.gpu, mem.used, pow)) < 0) {
      perror("failed to format update");
      return -1;
    }
    idx += rc;
  }

  values[0] = val;

  if (rrdc_update(db, 1, (const char * const *)values)) {
    perror("rrdtool update failed");
    return -1;
  }
  return 0;

nvml_err:
  printf("error in NVML: %s\n", nvmlErrorString(res));
  if ((res = nvmlShutdown()) != NVML_SUCCESS) printf("failed to shutdown NVML: %s\n", nvmlErrorString(res));
  return -1;
}

int main(int argc, char **argv) {
  nvmlReturn_t res;

  if (argc != 3) {
    printf("usage: %s <rrdcached socket> <database>\n", argv[0]);
    return 1;
  }

  if (rrdc_connect(argv[1])) {
    printf("failed to connect to rrdcached at %s\n", argv[1]);
    return -1;
  }

  if ((res = nvmlInit_v2()) != NVML_SUCCESS) {
    printf("failed to initialize NVML: %s\n", nvmlErrorString(res));
    return -1;
  }


  if (submit_gpu(argv[2])) return -1;

  return rrdc_disconnect();
}


