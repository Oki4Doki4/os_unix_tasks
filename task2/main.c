#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>




#define LOCK_FILE_ERROR "Error: bad trying to lock a file\n"
#define UNLOCK_FILE_ERROR "Error: bad trying to unlock file (good! Wait 5 minutes)\n"

#define READ_FILE_ERROR "File read Error\n"
#define WRITE_FILE_ERROR "File write Error\n"
#define params_error "Error: wrong parametr\n"
//#define sigint_error "SIGINT handling setup error\n"

char *file_input_name, *flock_name;
bool check_file_lock = false;
int lock_count, unlock_count;
int buffer_size = 1024;

bool msg_exception(char *message, bool err_exit){
    fprintf(stderr, "%s", message);
    if (err_exit)
        exit(EXIT_FAILURE);
    return false;
}

void remove_empty_lock_file() {
    char *buffer = (char *) malloc(buffer_size);
    int fd = open(flock_name, O_RDONLY);
    if (fd != -1) {
        ssize_t read_bytes = read(fd, buffer, buffer_size); 
        if (close(fd) != -1 && read_bytes == 0)
            remove(flock_name);
    }
    free(buffer);
}

// чекеры lock/unlock файл ли
bool lock_file(bool err_exit) {
    char pid[buffer_size];
    int fd_lock_file = -1;
    while (fd_lock_file == -1) {
        fd_lock_file = open(flock_name, O_CREAT | O_EXCL | O_WRONLY, 0600);
    }
    check_file_lock = true;
    sprintf(pid, "%d\n", getpid());
    if (write(fd_lock_file, pid, strlen(pid)) != strlen(pid) || close(fd_lock_file) == -1) 
        return msg_exception(LOCK_FILE_ERROR, err_exit);
    return true;
}


bool unlock_file(bool err_exit) {
    char *pid_ = (char *) malloc(buffer_size);
    int fd = open(flock_name, O_RDONLY);
    if (
        fd == -1 || read(fd, pid_, buffer_size) == -1 || atoi(pid_) != getpid() || close(fd) == -1 
        || remove(flock_name) == -1)
        return msg_exception(UNLOCK_FILE_ERROR, err_exit);
    check_file_lock = false;
    free(pid_);
    return true;
}



void sigintHandler(int sig) {
    if (check_file_lock && unlock_file(false)) 
        unlock_count++;
    remove_empty_lock_file();
    char buffer[buffer_size];
    sprintf(buffer, "%d\t%d\t%d\n", getpid(), lock_count, unlock_count);
    int fd = open("result.txt", O_WRONLY | O_APPEND | O_CREAT);
    if (fd == -1 || write(fd, buffer, strlen(buffer)) != strlen(buffer))
        msg_exception(WRITE_FILE_ERROR, true);
    close(fd);
    exit(EXIT_SUCCESS);
}

void count_lock_unlock(bool err_exit, int sleep_time){
    char *buffer = (char *) malloc(buffer_size);
    while (true) {
        if (lock_file(err_exit)) {
            lock_count++;

            int input_fd = open(file_input_name, O_RDONLY);
            if (input_fd == -1 || read(input_fd, buffer, buffer_size) == -1) {
                unlock_file(err_exit);
                msg_exception(READ_FILE_ERROR, true);
            }

            sleep(sleep_time);
            if (unlock_file(err_exit))
                unlock_count++;
        }
    }
}


int main(int argc, char** argv) {
    
    bool err_exit = false;
    int index, sleep_time;
    

    signal(SIGINT, sigintHandler);

  //  read_args(argc, argv);
// используем getopt для разбора параметров

    while ((index = getopt(argc, argv, "s:e")) != -1)
    {
        switch (index)
        {
            case 's':
                sleep_time = atoi(optarg);
                break;
            case 'e':
                err_exit = true;
                break;
            default:
                msg_exception(params_error, true);
        }
    }

    while (optind < argc) 
    {
        char *file = (char *) malloc(strlen(argv[optind]) + 1);
        strcpy(file, argv[optind]);
        optind++;
        if (file_input_name == NULL) {
            file_input_name = file;
            continue;
        }
        msg_exception(params_error, true);
    }

    sleep(1);

    if (file_input_name == NULL)
        msg_exception(params_error, true);
    
    flock_name = malloc(strlen(file_input_name) + strlen(".lck") + 1);
    strcpy(flock_name, file_input_name);
    strcat(flock_name, ".lck");

    count_lock_unlock(err_exit, sleep_time);

}