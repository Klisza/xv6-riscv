#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"

typedef struct Node {
  char        *data;
  struct Node *next;
} Node;

// Add item at the end of the linked list.
void appendlist(Node **head, Node **tail, char *s) {
  Node *n = malloc(sizeof(Node));
  if (n == 0) {
    fprintf(2, "malloc: cannot allocate memory for Node n.\n");
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

void sort(int fd) {
  int   n;
  char  buf[512];
  Node *head = 0;
  Node *tail = 0;
  // longest english word.
  char wordbuf[45 + 1];
  int  wordlen = 0;
  // Read all the words/strings from the file and create a linked list.
  while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
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
          strcpy(data, wordbuf);
          appendlist(&head, &tail, data);
          wordlen = 0;
        } else {
          // Multiple whitespaces present.
          continue;
        }
      } else if (wordlen < sizeof(wordbuf) - 1) {
        wordbuf[wordlen] = c;
        wordlen++;
      } else {
        fprintf(2, "String is too long.\n");
      }
    }
  }
  if (wordlen > 0) {
    wordbuf[wordlen] = '\0';
    int   len = strlen(wordbuf) + 1;
    char *data = malloc(len);
    if (data == 0) {
      fprintf(2, "Out of memory.\n");
      exit(1);
    }
    strcpy(data, wordbuf);
    appendlist(&head, &tail, data);
    wordlen = 0;
  }
  // Sort the linked list and print it out.
  head = mergesort(head);
  freelist(head);
  Node *cur = head;
  while (cur != 0) {
    printf("%s ", cur->data);
    cur = cur->next;
  }
  printf("\n");
  freelist(head);

  close(fd);
  return;
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    sort(0);
  else {
    for (int i = 1; i < argc; i++) {
      int fd = open(argv[i], O_RDONLY);
      if (fd < 0) {
        fprintf(2, "Open: cannot open %s\n", argv[i]);
        exit(1);
      }

      struct stat st;
      if (fstat(fd, &st) < 0) {
        fprintf(2, "sort: cannot stat %s\n", argv[i]);
        close(fd);
        exit(0);
      }

      if (st.type == T_DIR || st.type == T_DEVICE) {
        fprintf(2, "sort: cannot sort %s\n", argv[i]);
        close(fd);
        exit(1);
      }
      sort(fd);
      close(fd);
    }
  }
  exit(0);
}
