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

typedef struct Node {
  char        *data;
  struct Node *next;
} Node;

void appendList(Node **head, Node **tail, char *s) {
  Node *n = malloc(sizeof(Node));
  if (n == 0) {
    fprintf(2, "malloc: cannot allocate memory for Node n.");
    return;
  }

  n->data = s;
  n->next = 0;
  if (*head == 0) {
    // Init the head of the list. Head = tail at the beginning.
    *head = n;
    *tail = n;
  } else {
    // Set n to next of current tail and set tail to n after.
    (*tail)->next = n;
    *tail = n;
  }
  return;
}

void freeList(Node *head) {
  Node *cur = head;
  while (cur) {
    // printf("Freeing...\n");
    Node *next = cur->next;
    free(cur->data);
    free(cur);
    cur = next;
  }
  // printf("Done freeing...\n");
  return;
}

void ls(char *path, char *arg) {
  char          buf[512], *p;
  int           fd;
  struct dirent de;
  struct stat   st;
  Node         *head = 0;
  Node         *tail = 0;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

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
    printf("%s:\n", path);
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;

      // printf("Reading the directory...\n");
      // Setting the char pointer p to the end of buff and adding the null
      // terminator.
      // memmove copies the name into the buffer after the "/" where p points to
      // It does not change the positon where p points to
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (int)st.size);
      if (st.type == T_DIR && strcmp(arg, "-R") == 0) {
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
          continue; // Skip current and parent directory
        int i = strlen(buf) + 1;
        // Set the size of memory to the same size of the buffer + 1 (null
        // terminator)
        char *cp = malloc(sizeof(char) * i);
        if (cp == 0)
          continue; // malloc failed
        strcpy(cp, buf);
        appendList(&head, &tail, cp);
      }
    }
    printf("\n");
    break;
  }
  if (head != 0 && strcmp(arg, "-R") == 0) {
    // Iterate over the list and free the nodes after
    Node *cur = head;
    while (cur != 0) {
      // printf("Going through the list...\n");
      Node *next = cur->next;
      ls(cur->data, "-R");
      cur = next;
    }
    freeList(head);
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  int i;
  if (argc < 2) {
    ls(".", "");
    exit(0);
  }
  if (strcmp(argv[1], "-R") == 0) {
    if (argc < 3) {
      ls(".", "-R");
      exit(0);
    }
    for (i = 2; i < argc; i++)
      ls(argv[i], "-R");
  }
  for (i = 1; i < argc; i++)
    ls(argv[i], "");
  exit(0);
}
