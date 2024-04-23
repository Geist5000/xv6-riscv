#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

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
    fprintf(2, "pwd: int stat");
    exit(1);
  }
  if (chdir("..") != 0) {
    fprintf(2, "pwd: init chdir");
    exit(1);
  }
  while (1) {
    if ((fd = open(".", 0)) < 0) {
      fprintf(2, "pwd: loop open\n");
      exit(1);
    }
    if (fstat(fd, &new_st) != 0) {
      close(fd);
      fprintf(2, "pwd: loop stat");
      exit(1);
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
        fprintf(2, "pwd: entry stat of %s; folder ino:%d", buf, new_st.ino);
        exit(1);
      }
      if (st.ino == last_st.ino) {
        int length = strlen(dirEntry.name);
        rp -= length;
        if (rp <= result) {
          close(fd);
          fprintf(2, "pwd: path to long", buf, new_st.ino);
          exit(1);
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
      fprintf(2, "pwd: loop chdir");
      exit(1);
    }

  }

  printf("%s", rp);


  exit(0);
}
