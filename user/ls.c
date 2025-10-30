#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

char *fmtname(char *path) {
  static char buf[DIRSIZ + 1];
  char       *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if (strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  buf[sizeof(buf) - 1] = '\0';
  return buf;
}

// Linked list for strings as values
typedef struct node {
  char *dir;
  struct node *next;
} node_t;

// Add arg to ls
// Use a queue (linked list) to add the dir files that need to get listed
// Work them through one after one with the original ls command.
void ls(char *path, char* arg) {
  char          buf[512], *p;
  int           fd;
  struct dirent de;
  struct stat   st;
  node_t queue;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }
  if (strcmp(arg, "-R") == 0) {
    switch (st.type) {
    case T_DEVICE:
      break;
    case T_FILE:
      printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (int)st.size);
      break;
    case T_DIR:
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("ls: path too long\n");
      break;
      }
    }
  }
  else
{
  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (int)st.size);
    break;

  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      if(st.type == T_DIR) printf("IS A DIR ");
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (int)st.size);
    }
    break;
    }
  }
  close(fd);
}


int main(int argc, char *argv[]) {
  int i;
  if (argc < 2) {
    ls(".", " ");
    exit(0);
  }
  if (strcmp(argv[1], "-R") == 0) {
    if (argc < 3) {
      ls(".", " ");
      exit(0);
    }
    for (i = 2; i < argc; i++) {
      ls(argv[i], "-R");
      exit(0);
    }
  }
  for (i = 1; i < argc; i++)
    ls(argv[i], " ");
  exit(0);
}
