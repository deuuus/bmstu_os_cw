#include <linux/sched/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 16384

#define REPEAT 5
#define DELAY 10

int main(int argc, char *argv[])
{
    FILE *proc_file = NULL;
    FILE *out_file = fopen("out.txt", "w");

    char buf[BUF_SIZE] = {'\0'};
    
    for (int i = 0; i < REPEAT; i++)
    {
        proc_file = popen("cat /proc/bmstu_coursework", "r");

        if (proc_file == NULL)
        {
            printf("Error while popen.");
            return EXIT_FAILURE;
        }

        fgets(buf, sizeof(buf), proc_file);
        fprintf(out_file, "%s", buf);

        while (fgets(buf, sizeof(buf), proc_file) != NULL)
        {
            fprintf(out_file, "%s", buf);
        }

        pclose(proc_file);
        sleep(DELAY);
    }

    fclose(out_file);

    return EXIT_SUCCESS;
}