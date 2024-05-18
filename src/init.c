#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

extern void rusty_entry();

void *wait_for_init(void *arg) {
  sleep(5);
  while (1) {
    sleep(1);
  }
  return NULL;
}

/* Dynamic library initializer since its more complex to do in Rust - It gets
 * called when loaded and then calls rust function */
__attribute__((constructor)) void my_init(void) {
  printf("c constructor init...\n");
  rusty_entry();
  // pthread_t tid;
  // pthread_create(&tid, NULL, wait_for_init, NULL);
  // rusty_entry();
}

int change_permissions(unsigned long start, unsigned long end, int pid) {
  printf("start: %lu end: %lu\n", start, end);
  size_t length = end - start;
  if (mprotect((void *)start, length, PROT_READ | PROT_WRITE | PROT_EXEC) ==
      -1) {
    perror("mprotect failed");
    return -1;
  }

  printf("perms been changed for mapping start: %lu end: %lu length: %lu\n",
         start, end, length);
  return 0;
}

int read_mappings() {

  int pid = getpid();
  // printf("PID: %d\n", pid);

  char mappings_path[256] = {0};
  sprintf(mappings_path, "/proc/%d/maps", pid);

  FILE *file = fopen(mappings_path, "r");

  if (file == NULL)
    return -1;

  char line[256];
  int line_count = 0;

  while (fgets(line, sizeof(line), file) != NULL && line_count < 7) {
    // fclose(file);
    printf("trying line: %s\n", line);

    line_count++;

    char *addr_range = strtok(line, " ");
    if (!addr_range)
      continue;

    unsigned long start, end;
    char *dash;
    dash = strchr(addr_range, '-');

    if (!dash)
      continue;

    *dash = '\0';

    start = strtoul(addr_range, NULL, 16);
    end = strtoul(dash + 1, NULL, 16);
    size_t length = end - start;
    change_permissions(start, end, pid);
    char dump_path[256] = {0};
    sprintf(dump_path, "./dump%d.bin", line_count);

    int fd = open(dump_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1)
      continue;

    write(fd, (void *)start, length);
    close(fd);
    printf("Mapping: %d start: %lu has been dumped\n", line_count, start);
  }
  return 0;
}
