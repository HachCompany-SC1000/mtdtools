/*
 * FILE flash_lock.c
 *
 * This utility locks one or more sectors of flash device.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <string.h>

#include <mtd/mtd-user.h>

/* Note: In order to understand why updates fail all EXIT_FAILURE are replaced by posizive numbers 10, 11, 12, ..., 16 */
#define EXIT_FAILURE 1


int main(int argc, char *argv[])
{
  int fd;
  struct mtd_info_user mtdInfo;
  struct erase_info_user mtdLockInfo;
  int num_sectors;
  int ofs;

  /*
   * Parse command line options
   */
  if(argc != 4)
  {
    fprintf(stderr, "USAGE: %s <mtd device> <ofs in hex> <num of sectors in decimal or -1 for all sectors>\n", argv[0]);
    exit(10/*EXIT_FAILURE*/);
  }
  else if(strncmp(argv[1], "/dev/mtd", 8) != 0)
  {
    fprintf(stderr, "'%s' is not a MTD device.  Must specify mtd device: /dev/mtd?\n", argv[1]);
    exit(11/*EXIT_FAILURE*/);
  }

  fd = open(argv[1], O_RDWR);
  if(fd < 0)
  {
    fprintf(stderr, "Could not open mtd device: %s\n", argv[1]);
    exit(12/*EXIT_FAILURE*/);
  }

  if(ioctl(fd, MEMGETINFO, &mtdInfo))
  {
    fprintf(stderr, "Could not get MTD device info from %s\n", argv[1]);
    close(fd);
    exit(13/*EXIT_FAILURE*/);
  }
  sscanf(argv[2], "%x",&ofs);
  sscanf(argv[3], "%d",&num_sectors);
  if(ofs > mtdInfo.size - mtdInfo.erasesize)
  {
    fprintf(stderr, "%x is beyond device size %x\n",ofs,(unsigned int)(mtdInfo.size - mtdInfo.erasesize));
    exit(14/*EXIT_FAILURE*/);
  }

  if (num_sectors == -1) {
	  num_sectors = mtdInfo.size/mtdInfo.erasesize;
  }
  else {
	  if(num_sectors > mtdInfo.size/mtdInfo.erasesize)
	  {
	    fprintf(stderr, "%d are too many sectors, device only has %d\n",num_sectors,(int)(mtdInfo.size/mtdInfo.erasesize));
	    exit(15/*EXIT_FAILURE*/);
	  }
  }

  mtdLockInfo.start = ofs;
  mtdLockInfo.length = num_sectors * mtdInfo.erasesize;
  if(ioctl(fd, MEMLOCK, &mtdLockInfo))
  {
    fprintf(stderr, "Could not lock MTD device: %s\n", argv[1]);
    close(fd);
    exit(16/*EXIT_FAILURE*/);
  }

  return 0;
}

