/*
 * FILE flash_unlock.c
 *
 * This utility unlock all sectors of flash device.
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
  int querymode = 0;

  /*
   * Parse command line options
   */
  if(argc < 2)
  {
    fprintf(stderr, "USAGE: %s <mtd device> [-q(uery): no unlock]\n", argv[0]);
    exit(10/*EXIT_FAILURE*/);
  }
  else if(strncmp(argv[1], "/dev/mtd", 8) != 0)
  {
    fprintf(stderr, "'%s' is not a MTD device.  Must specify mtd device: /dev/mtd?\n", argv[1]);
    exit(11/*EXIT_FAILURE*/);
  }

  if(argc>2 && strncmp(argv[2], "-q", 2) == 0)
    querymode = 1;

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

/*struct mtd_info_user {
	uint8_t type;
	uint32_t flags;
	uint32_t size;	 // Total size of the MTD
	uint32_t erasesize;
	uint32_t writesize;
	uint32_t oobsize;   // Amount of OOB data per block (e.g. 16)
	uint32_t ecctype;
	uint32_t eccsize;
};
*/

  if (querymode) {
    fprintf(stdout, "mtd_info_user: %s\n", argv[1]);
    fprintf(stdout, "  type      0x%X\n", (unsigned int)mtdInfo.type );
    fprintf(stdout, "  flags     0x%X\n", (unsigned int)mtdInfo.flags );
    fprintf(stdout, "  size      0x%X\n", (unsigned int)mtdInfo.size );
    fprintf(stdout, "  erasesize 0x%X\n", (unsigned int)mtdInfo.erasesize );
    fprintf(stdout, "  writesize 0x%X\n", (unsigned int)mtdInfo.writesize );
    fprintf(stdout, "  oobsize   0x%X\n", (unsigned int)mtdInfo.oobsize );
    fprintf(stdout, "  ecctype   0x%X\n", (unsigned int)mtdInfo.ecctype );
    fprintf(stdout, "  eccsize   0x%X\n", (unsigned int)mtdInfo.eccsize );
    return 0;
  }

  mtdLockInfo.start = 0;
  mtdLockInfo.length = mtdInfo.size;
  if(ioctl(fd, MEMUNLOCK, &mtdLockInfo))
  {
    fprintf(stderr, "Could not unlock MTD device: %s\n", argv[1]);
    close(fd);
    exit(14/*EXIT_FAILURE*/);
  }

  return 0;
}

