#define _GNU_SOURCE
#include <dlfcn.h> // for dlsym
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int hellp() {
  printf("help called");
  return 0;
}
int get_base_address();
int main() {
  char buffer[1024];
  int i = get_base_address();

  void *handle;
  // void (*rust_function)();
  // char *error;
  handle = dlopen(NULL, RTLD_LAZY);
  // printf("test %d\n", 2);
  // if (!handle) {
  //   fprintf(stderr, "%s\n", dlerror());
  //   exit(1);
  // }

  // hellp();
  // printf("does this work?\n");
  // dlerror(); /* Clear any existing error */
  // rust_function = dlsym(handle, "rust_function");
  // if ((error = dlerror()) != NULL) {
  //   fprintf(stderr, "%s\n", error);
  //   exit(1);
  // }
  // (*rust_function)();
  // dlclose(handle);

  printf("Press any key to exit\n");
  scanf("%d", NULL);
  return 0;
}
int get_base_address() {

  void *handle = dlopen(NULL, RTLD_NOW);
  if (!handle) {
    fprintf(stderr, "Error: %s\n", dlerror());
    return -1;
  }

  if (handle) {

    printf("Sucessfully got base address: %p\n", handle);
  } else {
    printf("Failed to get base address\n");
    return -1;
  }

  // Close the handle
  dlclose(handle);
  return 0;
}
