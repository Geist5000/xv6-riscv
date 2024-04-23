#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define printErrorExit(fmt, ...) (fprintf(2,fmt, __VA_ARGS__);\
              exit(1))

int
main(int argc, char *argv[]) {


  int fd;
  struct stat last_st;
  struct stat new_st;
  char result[512], *rp;
  rp = result;
  rp += 510;
  *(rp + 1) = 0;
  char buf[512], *p;
  p = buf;
  memmove(p, "./", sizeof(char) * 2);
  p += 2;

  if (stat(".", &last_st) != 0) {
    printErrorExit("pwd: int stat");
  }
  if (chdir("..") != 0) {
    printErrorExit("pwd: init chdir");
  }
  while (1) {
    if ((fd = open(".", 0)) < 0) {
      printErrorExit("pwd: loop open\n");
    }
    if (fstat(fd, &new_st) != 0) {
      close(fd);
      printErrorExit("pwd: loop stat");
    }

    struct dirent dirEntry;
    int count = 0;
    while (read(fd, &dirEntry, sizeof(dirEntry)) == sizeof(dirEntry)) {
      if (++count < 3) {
        continue;
      }
      if (dirEntry.inum == 0) {
        continue;
      }
      struct stat st;
      memmove(p, dirEntry.name, DIRSIZ);
      *(p + DIRSIZ) = 0;

      if (stat(buf, &st) != 0) {
        close(fd);
        printErrorExit("pwd: entry stat of %s; folder ino:%d", buf, new_st.ino);
      }
      if (st.ino == last_st.ino) {
        int length = strlen(dirEntry.name);
        rp -= length;
        if (rp <= result) {
          close(fd);
          printErrorExit("pwd: path to long", buf, new_st.ino);
        }
        memmove(rp, dirEntry.name, sizeof(char) * length);
        rp--;
        memmove(rp, "/", sizeof(char));
        last_st = new_st;
        break;
      }
    }

    close(fd);
    if (new_st.ino == 1) {
      break;
    }

    if (chdir("..") != 0) {
      printErrorExit("pwd: loop chdir");
    }

  }

  printf("%s", rp);


  exit(0);
}
