/**
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX(a, b) a > b ? a : b

const int DISK_BLOCKS = (12 * 1024 * 1024 / 512) - 1;
const int MIN_BLOCKS = 10;
const int BLOCKSIZE = 512;
const char *BARE_FILE = "iso/bare.img";

class IMG {
public:
  IMG(const char *filename) {
    errno = 0;
    alive = false;
    this->filename = strdup(filename);
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
      error = strdup(strerror(errno));
      perror("open");
      return;
    }
    size = lseek(fd, 0, 2);
    blocks = long(size + (BLOCKSIZE - 1)) / BLOCKSIZE;
    lseek(fd, 0, 0);

    bytes = new uint8_t[blocks * BLOCKSIZE];
    // assure bytes are zeroes out
    bzero(bytes, blocks * BLOCKSIZE);
    read(fd, bytes, size);
    close(fd);
    this->alive = true;
  }
  ~IMG() {
    delete[] bytes;
  }

public:
  char *error;
  bool alive;
  char *filename;
  long size, blocks;
  uint8_t *bytes;
};

int main(int ac, char *av[]) {
  const char *bootfile = ac > 1 ? av[1] : "../boot.img";
  const char *kernelfile = ac > 2 ? av[2] : "../kernel.img";
  const char *fsfile = ac > 3 ? av[3] : "../iso/fs.img";

  IMG boot(bootfile);
  IMG kernel(kernelfile);
  IMG fs(fsfile);

  if (!boot.alive) {
    printf("Can't open %s (%s)\n", bootfile, boot.error);
    exit(1);
  }
  if (!kernel.alive) {
    printf("Can't open %s (%s)\n", kernelfile, kernel.error);
    exit(1);
  }

  printf("\nbuild-img v1.0\n");
  long fsblocks = fs.alive ? fs.blocks : 0;
  long rawblocks = MAX(boot.blocks + kernel.blocks + fsblocks, MIN_BLOCKS);
  long rawsize = rawblocks * BLOCKSIZE;

  printf("  %s is %d bytes, %d blocks\n", boot.filename, boot.size, boot.blocks);
  printf("  %s is %d bytes, %d blocks\n", kernel.filename, kernel.size, kernel.blocks);

  if (fs.alive) {
    printf("  %s is %d bytes, %d blocks\n", fs.filename, fs.size, fs.blocks);
  }
  else {
    printf("*** warning no fs.img filesystem image\n");
  }
  printf("\n  ==> %s is %d bytes, %d blocks (%d minimum)\n", BARE_FILE, rawsize, rawblocks, MIN_BLOCKS);

  uint8_t *raw = new uint8_t[rawsize];

  // assure raw is zeros
  bzero(raw, rawblocks * BLOCKSIZE);

  long bootsize = boot.blocks * BLOCKSIZE;
  memcpy(raw, boot.bytes, bootsize);

  long kernel_size = kernel.blocks * BLOCKSIZE;
  memcpy(&raw[bootsize], kernel.bytes, kernel_size);

  long fs_size = fs.blocks * BLOCKSIZE;
  memcpy(&raw[bootsize + kernel_size], fs.bytes, fs_size);

  // patch boot.img with block # and num blocks for boot sector and kernel
  uint16_t *ptr = (uint16_t *)&raw[12];
  ptr[0] = 2; // boot_sector
  printf("\n  boot_sector: %d\n", 2);

  ptr[1] = boot.blocks - 1; // boot_sectors
  printf("  boot_sectors: %d\n", ptr[1]);

  ptr[2] = boot.blocks + 1; // kernel_sector
  printf("  kernel_sector: %d\n", ptr[2]);

  ptr[3] = kernel.blocks;   // kernel_sectors
  printf("  kernel_sectors: %d\n", ptr[3]);

  ptr[4] = fs.alive ? (ptr[3] + ptr[2] - 1) : 0; // fs_sector
  printf("  root_sector: %d\n\n", ptr[4]);

  int fd = open(BARE_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    printf("*** Can't open %s for output\n", BARE_FILE);
    exit(1);
  }
  write(fd, raw, rawsize);
  close(fd);

  return 0;
}
