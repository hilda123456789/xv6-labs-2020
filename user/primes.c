#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int fd) {
  int my_num = 0;                // the numer that I got to print and filter
  int forked = 0;                // sign whether I called a process
  int passed_num = 0;            // the numer that I read 
  int pipes[2];                  //the sub pipe to pass to the next process
  while (1) {
    int read_bytes = read(fd, &passed_num, 4);  //read a num

    // read all the number of this pipe
    if (read_bytes == 0) {
      close(fd);
      if (forked) {           //created a sub pipe to pass
        close(pipes[1]);     
        //int child_pid;
        wait(0);
      }
      exit(0);
    }

    // print the number I got
    if (my_num == 0) {
      my_num = passed_num;
      printf("prime %d\n", my_num);
    }

    // write a sub pipe to filter and pass
    if (passed_num % my_num != 0) {
      if (!forked) {          //the first
        pipe(pipes);
        forked = 1;
        int ret = fork();     //call a sub-process to process the number that is not the multiple of my number
        if (ret == 0) {
          // child
          close(pipes[1]);    //sub process only need to read
          close(fd);          //sub process read from new sub pipe
          primes(pipes[0]);   //sub primes
        } 
        else {
          // parent
          close(pipes[0]);    //parent only need to write
        }
      }
      // the passed sub pipe
      write(pipes[1], &passed_num, 4);
    }
  }
}

int main(int argc, char *argv[]) {
  int pipes[2];
  pipe(pipes);
  for (int i = 2; i <= 35; i++) {
    write(pipes[1], &i, 4);          // write all the num into the pipe
  }
  close(pipes[1]);
  primes(pipes[0]);                  // to find and print the primes
  exit(0);
}
