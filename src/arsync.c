#include "arsync.h"

sem_t sem;

int counter = 0;

int max;

char **args;

char *dest;

void *worker(void *x) {
  while (counter < max) {
    sem_wait(&sem);
    int curr = counter++;
    sem_post(&sem);
    pid_t pid = fork();
    if (pid) {
      int status;
      pid_t wpid = waitpid(pid, &status, 0);
      if (wpid == -1)
        perror("waitpid failed");
    } else {
      printf("Copying '%s'...\n", args[curr]);
      execlp("rsync", "rsync", "-a", "--partial", args[curr], dest,
             (char *)NULL);
      _exit(1);
    }
  }
  return x;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: arsync [sources ...] [dest]\n");
    exit(1);
  }
  args = malloc(sizeof(char *) * (argc - 2));
  for (int i = 1; i < argc - 1; i++) {
    args[i - 1] = malloc(strlen(argv[i]) + 1);
    strcpy(args[i - 1], argv[i]);
  }
  max = argc - 2;
  dest = malloc(strlen(argv[argc - 1]) + 1);
  strcpy(dest, argv[argc - 1]);
  sem_init(&sem, 0, 1);
  pthread_t threads[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++) {
    int *ptr = malloc(sizeof(int));
    *ptr = i + 1;
    pthread_create(threads + i, NULL, worker, (void *)ptr);
  }
  for (int i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i], NULL);
  sem_destroy(&sem);
}
