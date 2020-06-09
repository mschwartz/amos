#include <FileSystems/BFileSystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

class RawDisk {
public:
  RawDisk(const char *filename, TUint64 size);
public:
  TBool Format();
  TBool MakeDirectory(const char *path);
  TBool CopyFile(const char *dst, const char *src);
};

void Format() {
  int fd = open("/tmp/barefs.img", O_WRONLY|O_CREAT|O_TRUNC, 0666);
  if (fd < 0) {
    printf("Can't open %s for output\n", "/tmp/barefs.img");
    exit(1);
  }
}

int main(int ac, char *av[]) {
  if (ac < 2) {
    printf("Usage:\n\t %s command [args...]\n", av[0]);
    exit(1);
  }

  if (strcasecmp(av[1], "FORMAT") == 0) {
    printf("FORMAT\n");
  }
  else {
    printf("*** Invalid command '%s'\n", av[1]);
    exit(1);
  }
  printf("hello, world\n");
  
}
