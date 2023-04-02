#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>


#define PARAMS_ERROR "Parametr Error\n"
#define OPEN_FILE_ERROR "Error open file\n"
#define READ_FILE_ERROR "Error read file\n"
#define WRITE_FILE_ERROR "Error write file\n"
#define CLOSE_FILE_ERROR "Error close file\n"
#define LSEEK_ERROR "Error lseek function\n"
/*
#define def_error "default error for debug_testing\n"
*/
//#define ERROR_MSG "Error: Enter output filename\n"
//#define ERROR_ARG_TOO_MUCH "Error: Too much args\n"

void msg_exception(char *error_msg){
    fprintf(stderr, "%s", error_msg);
    exit(EXIT_FAILURE);
}

void check_close_file_error(char *input_file, int input_fd, int output_fd){
    if (input_file != NULL && close(input_fd) == -1 || close(output_fd) == -1)
        msg_exception(CLOSE_FILE_ERROR);
}

int main(int argc, char *argv[]) {
    /*
    if (argc < 2 || argc > 3) {
        fprintf(stderr, argc < 2 ? ERROR_MSG : ERROR_ARG_TOO_MUCH);
        msg_exception(def_error);
    }
    */
    char *input_file = NULL;
    char *output_file = NULL;

    int block_size = 4096;
    int index;
// используем getopt для разбора параметров
    while ((index = getopt(argc, argv, "b:")) != -1)
    {
        switch (index)
        {
            case 'b':
                block_size = atoi(optarg);
                break;
            default:
                msg_exception(PARAMS_ERROR);
        }
    }
    
    if (optind < argc) 
        while (optind < argc) {
            char *file = (char *) malloc(strlen(argv[optind]) + 1);
            strcpy(file, argv[optind]);
            optind++;
            if (input_file == NULL) {
                input_file = file;
                continue;
            }
            if (output_file == NULL) {
                output_file = file;
                continue;
            }
            msg_exception(PARAMS_ERROR);
        }
    
    if (input_file != NULL && output_file == NULL) {
        output_file = input_file;
        input_file = NULL;
    }
    if (output_file == NULL || input_file != NULL && 
        strcmp(input_file, output_file) == 0 || block_size < 1)
        msg_exception(PARAMS_ERROR);

    int input_fd = STDIN_FILENO;
    int output_fd;
    
    if (input_file != NULL)
        input_fd = open(input_file, O_RDONLY);
        output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (input_fd == -1 || output_fd == -1)
        msg_exception(OPEN_FILE_ERROR);

    int zero_blocks_count = 0;

    char *buffer = (char *) malloc(block_size);
    while (true)
    {
        ssize_t read_bytes_count = read(input_fd, buffer, block_size);
        if (read_bytes_count == -1)
            msg_exception(READ_FILE_ERROR);
        if (read_bytes_count == 0)
            break;
        char *buffer_ = buffer;
        bool zero_block = true;
        for (int index = 0; index < read_bytes_count; index++)
            if (*(buffer_++)) {
                zero_block = false;
                break;
            }
        if (zero_block == true) {
            zero_blocks_count++;
            if (lseek(output_fd, block_size, SEEK_CUR) == -1)
                //zero_blocks_count = 0;
                msg_exception(LSEEK_ERROR);
        }
        else
            if (write(output_fd, buffer, read_bytes_count) != read_bytes_count)
                msg_exception(WRITE_FILE_ERROR);
    }

    check_close_file_error(input_file, input_fd, output_fd);

}