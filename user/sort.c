#include <kernel/types.h>
#include <user/user.h>
#include <kernel/fcntl.h>
#include <kernel/stat.h>

typedef struct Node {
  char        *data;
  struct Node *next;
} Node;

// Add item at the end of the linked list.
void appendlist(Node **head, Node **tail, char *s) {
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

void freelist(Node *head) {
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

Node *merge(Node *first, Node *second) {
  if (first == 0)
    return second;
  if (second == 0)
    return first;

  Node *result;

  if (strcmp(first->data, second->data) < 0 ||
      strcmp(first->data, second->data) == 0) {
    result = first;
    result->next = merge(first->next, second);
  } else {
    result = second;
    result->next = merge(first, second->next);
  }

  return result;
}

// Split the linked list with slow and fast pointer.
void split(Node *head, Node **firstRef, Node **secondRef) {
  Node *slow = head;
  Node *fast = head->next;

  while (fast != 0) {
    fast = fast->next;
    if (fast != 0) {
      slow = slow->next;
      fast = fast->next;
    }
  }

  *firstRef = head;
  *secondRef = slow->next;
  slow->next = 0;
  return;
}

// Prints the sorted input to output io stream.
// We can sort the strings using
Node *mergesort(Node *head) {
  // Edge case in case the list is empty or has only one element.
  if (head == 0 || head->next == 0) {
    return head;
  }

  Node *first;
  Node *second;

  split(head, &first, &second);

  first = mergesort(first);
  second = mergesort(second);

  return merge(first, second);
}

void readfile_and_sort(char *path) {
  int         fd;
  int         n;
  struct stat st;
  char        buf[512];
  Node       *head = 0;
  Node       *tail = 0;
  // longest english word.
  char wordbuf[45 + 1];
  int  wordlen = 0;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "sort: cannot open %s\n", path);
    exit(1);
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "sort: cannot stat %s\n", path);
    close(fd);
    exit(1);
  }
  switch (st.type) {
  case T_DEVICE:
    fprintf(2, "sort: cannot sort device.\n");
    break;
  case T_FILE:
    // Read all the words/strings from the file and create a linked list.
    while ((n = read(fd, buf, sizeof(buf) - 1) != 0) > 0) {
      // File is now in buffer
      for (int i = 0; i < n; i++) {
        char c = buf[i];
        if (c == ' ' || c == '\n' || c == '\t') {
          if (wordlen > 0) {
            wordbuf[wordlen] = '\0';
            int   len = strlen(wordbuf) + 1;
            char *data = malloc(len);
            if (data == 0) {
              fprintf(2, "Out of memory.\n");
              exit(1);
            }
            strcpy(data, buf);
            appendlist(&head, &tail, data);
            wordlen = 0;
          } else {
            // Multiple whitespaces present.
            continue;
          }
        } else if (wordlen < sizeof(wordbuf) - 1) {
          wordlen++;
          wordbuf[wordlen] = c;
        } else {
          fprintf(2, "String is too long.\n");
        }
      }
    }
    // Sort the linked list and print it out.
    mergesort(head);
    Node *cur = head;
    while (cur->next != 0) {
      printf("%s ", cur->data);
      cur = cur->next;
    }
    freelist(head);
    break;
  case T_DIR:
    fprintf(2, "sort: cannot sort directory.\n");
    break;
  }

  close(fd);
  exit(0);
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    printf("Pipe command.\n");
  else
    for (int i = 0; i < argc; i++) {
      readfile_and_sort(argv[i]);
    };
  exit(0);
}
