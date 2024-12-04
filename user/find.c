#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *dir, char *file)
{
    char buf[512], *p;    // buf to save the file name
    int fd;      
    struct dirent de;     //to get the dir info
    struct stat st;       //to get the file states

    if ((fd = open(dir, 0)) < 0)      //cannot open
    {
        fprintf(2, "find: cannot open %s\n", dir);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", dir);
        close(fd);
        return;
    }

    if (st.type!= T_DIR)
    {
        fprintf(2, "find: %s is not a directory\n", dir);
        close(fd);
        return;
    }

    if (strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        fprintf(2, "find: directory too long\n");
        close(fd);
        return;
    }

    strcpy(buf, dir);
    p = buf + strlen(buf);     //p at the end of dir
    *p++ = '/';                //add / to concatenation the file name
    while(read(fd, &de, sizeof(de)) == sizeof(de))
    {
        // de.inum==0:invalid or special dir
        if(de.inum == 0)
            continue;

        // donnot read these dir
        if(!strcmp(de.name, ".") ||!strcmp(de.name, ".."))
            continue;

        memmove(p, de.name, DIRSIZ);  //concatenation the file name
        p[DIRSIZ] = 0;

        if(stat(buf, &st) < 0)
        {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }


        if(st.type == T_DIR)        //find recursively
        {
            find(buf, file);
        }
        else if(st.type == T_FILE &&!strcmp(de.name, file))   //find it
        {
            printf("%s\n", buf);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc!= 3)
    {
        fprintf(2, "usage: find dirName fileName\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
