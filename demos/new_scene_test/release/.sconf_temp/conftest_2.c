
    #include <stdio.h>
    int main(int argc, char **argv) {
      // From Harbison&Steele, adopted from cmake
      union
      {
        long l;
        char c[sizeof (long)];
      } u;
      u.l = 1;
      if (u.c[sizeof (long) - 1] == 1)
        printf("big\n");
      else
        printf("little\n");
      return 0;
    }
    