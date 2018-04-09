#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <dirent.h>
#include <memory.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h>

//CALL IT BY ./a.out ./../../ "<" 2018-03-2113:59:00

char *operant_value;
time_t date_value;
char buffer[PATH_MAX];

char format[] = "%Y-%m-%d %H:%M:%S";


void print_file_info(const char *fpath, const struct stat *sb) {
    printf("%s %ld ",
           fpath,(intmax_t) sb->st_size);
    printf( (sb->st_mode & S_IRUSR) ? "r" : "-");
    printf( (sb->st_mode & S_IWUSR) ? "w" : "-");
    printf( (sb->st_mode & S_IXUSR) ? "x" : "-");
    printf( (sb->st_mode & S_IRGRP) ? "r" : "-");
    printf( (sb->st_mode & S_IWGRP) ? "w" : "-");
    printf( (sb->st_mode & S_IXGRP) ? "x" : "-");
    printf( (sb->st_mode & S_IROTH) ? "r" : "-");
    printf( (sb->st_mode & S_IWOTH) ? "w" : "-");
    printf( (sb->st_mode & S_IXOTH) ? "x" : "-");
    time_t t = sb->st_mtime;
    struct tm lt;
    localtime_r(&t, &lt);
    char timbuf[80];
    strftime(timbuf, sizeof(timbuf), "%c", &lt);
    printf(" %s\n", timbuf);
}

double date_compare(time_t date_1, time_t date_2) {
    return difftime(date_1, date_2);
}

static int display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
    if(tflag != FTW_F) {
        return 0;
    }

    struct tm mtime;
    (void) localtime_r(&sb->st_mtime, &mtime);

    int comparison_result = date_compare(date_value, sb->st_mtime);
    if (!(
            (comparison_result == 0 && strcmp(operant_value, "=") == 0)
            || (comparison_result > 0 && strcmp(operant_value, "<") == 0)
            || (comparison_result < 0 && strcmp(operant_value, ">") == 0)
    )) {
        return 0;
    }

    print_file_info(fpath, sb);
    return 0;
}

int nftw_ls(char * filename){
    int flag;
    // FTW_PHYS - if set, nftw() shall perform a physical walk and shall not follow symbolic links.
    flag = FTW_PHYS;
    if (nftw(filename, display_info, 20, flag) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    return 0;
}

void print_info(const char *path, const struct stat *file_stat) {
    printf(" %ld\t", file_stat->st_size);
    printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat->st_mode & S_IXOTH) ? "x" : "-");
    strftime(buffer, PATH_MAX, format, localtime(&file_stat->st_mtime));
    printf(" %s\t", buffer);
    printf(" %s\t", path);
    printf("\n");
}

void dir_ls(char *path) {
    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("dir");
        return;
    }

    struct dirent *rdir = readdir(dir);
    struct stat file_stat;

    char new_path[PATH_MAX];

    while (rdir != NULL) {
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, rdir->d_name);
        lstat(new_path, &file_stat);

        if (strcmp(rdir->d_name, ".") == 0 || strcmp(rdir->d_name, "..") == 0) {
            rdir = readdir(dir);
            continue;
        } else {
            if (S_ISREG(file_stat.st_mode)) {
                if (strcmp(operant_value, "=") == 0 && date_compare(date_value, file_stat.st_mtime) == 0) {
                    print_info(new_path, &file_stat);
                } else if (strcmp(operant_value, "<") == 0 && date_compare(date_value, file_stat.st_mtime) > 0) {
                    print_info(new_path, &file_stat);
                } else if (strcmp(operant_value, ">") == 0 && date_compare(date_value, file_stat.st_mtime) < 0) {
                    print_info(new_path, &file_stat);
                }
            }


            if (S_ISDIR(file_stat.st_mode)) {
                dir_ls(new_path);
            }
            rdir = readdir(dir);
        }
    }
    closedir(dir);
}



int main(int argc, char *argv[])
{
    if(argc < 3){
        printf("nalezy podac przynajmniej 2 argumenty\n");
        return 1;
    }

    char* filename = realpath(argv[1], NULL);
    //supported operands '<', '>','='
    char *operant = argv[2];
    operant_value = operant;

    char *usr_date = argv[3];
    struct tm *timestamp = (struct tm*) malloc(sizeof(struct tm));

    strptime(usr_date, format, timestamp);
    time_t date = mktime(timestamp);

    /* Display Date
    char buff[100];
    struct tm * timeinfo;
    timeinfo = localtime (&date);
    strftime(buff, sizeof(buff), "%b %d %Y %H:%M", timeinfo); */

    date_value = date;
    if(strcmp("nftw", argv[4]) == 0) {
        nftw_ls(filename);
    } else if(strcmp("sys", argv[4]) == 0){
        dir_ls(filename);
    }


    free (filename);
    return 0;
}
