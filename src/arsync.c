#include "arsync.h"

sem_t *sem;

int counter = 0;

int max;

char **args;

char *dest;

Status statuses[NUM_THREADS];

int thread_done[NUM_THREADS];

void *worker(void *x) {
  while (counter < max) {
    sem_wait(sem);
    int curr = counter++;
    sem_post(sem);
    snprintf(statuses[*((int *)x)].msg, sizeof(statuses[0].msg),
             "Copying '%s'\n", args[curr]);
    pid_t pid = fork();
    if (pid) {
      int status;
      pid_t wpid = waitpid(pid, &status, 0);
      if (wpid == -1)
        perror("waitpid failed");
    } else {
      execlp("rsync", "rsync", "-a", "--partial", args[curr], dest,
             (char *)NULL);
      _exit(1);
    }
  }
  snprintf(statuses[*((int *)x)].msg, sizeof(statuses[0].msg),
           "Thread %d done.\n", *((int *)x) + 1);
  thread_done[*((int *)x)] = 1;
  return x;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: arsync [sources ...] [dest]\n");
    exit(1);
  }
  initscr();
  noecho();
  curs_set(0);
  scrollok(stdscr, FALSE);
  for (int i = 0; i < NUM_THREADS; i++)
    statuses[i].msg[0] = '\0';
  args = malloc(sizeof(char *) * (argc - 2));
  for (int i = 1; i < argc - 1; i++) {
    args[i - 1] = malloc(strlen(argv[i]) + 1);
    strcpy(args[i - 1], argv[i]);
  }
  max = argc - 2;
  dest = malloc(strlen(argv[argc - 1]) + 1);
  strcpy(dest, argv[argc - 1]);
  sem = sem_open("/sem", O_CREAT, 0644, 1);
  pthread_t threads[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++) {
    int *ptr = malloc(sizeof(int));
    *ptr = i;
    pthread_create(threads + i, NULL, worker, (void *)ptr);
  }

  int threads_running = 1;
  while (threads_running) {
    threads_running = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
      move(i, 0);
      clrtoeol();
      mvprintw(i, 0, "%s", statuses[i].msg);
      threads_running |= !thread_done[i];
    }
    move(NUM_THREADS, 0);
    clrtoeol();
    refresh();
    usleep(150000); // 50ms delay in updates
  }
  for (int i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i], NULL);
  sem_close(sem);
  sem_unlink("/sem");
  endwin();
  return 0;
}
