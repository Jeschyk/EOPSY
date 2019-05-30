#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void help()
{
    printf("Usage:\n");
    printf("copy [-m] <file_name> <new_file_name>\n");
    printf("copy [-h]\n");
}

void copy_map(int fd_from, int fd_to)
{
    struct stat fileS;
    fstat(fd_from, &fileS);

    if (fileS.st_size < 0)
    {
        printf("Error size incorrect: %s\n", strerror(errno));
        exit(-1);
    }
    char* inmap = mmap(NULL, fileS.st_size, PROT_READ, MAP_PRIVATE, fd_from, 0);

    if (inmap == MAP_FAILED)
    {
        printf("Error memory mapping from input file: %s\n", strerror(errno));
        exit(-1);
    }
    if (ftruncate(fd_to, fileS.st_size) < 0)
    {
        printf("Error while truncating file: %s\n", strerror(errno));
        exit(-1);
    }
    char* outmap = mmap(NULL, fileS.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_to, 0);
    if (outmap == MAP_FAILED)
    {
        printf("Error memory mapping to output file: %s\n", strerror(errno));
        exit(-1);
    }

    memcpy(outmap, inmap, fileS.st_size);

    if (munmap(inmap, fileS.st_size) < 0)
    {
        printf("Error unmapping input file memory: %s\n", strerror(errno));
        exit(-1);
    }

    if (munmap(outmap, fileS.st_size) < 0)
    {
        printf("Error unmapping output file memory: %s\n", strerror(errno));
        exit(-1);
    }
}

void copy_read_write(int fd_from, int fd_to)
{
    char buffer[1024*1024];
    size_t original_bytes;
    size_t coppied_bytes;

    while((original_bytes = read(fd_from, &buffer, 1024*1024)) > 0)
    {
        if (original_bytes < 0)
        {
            printf("Error reading from file: %s\n", strerror(errno));
            exit(-1);
        }
        coppied_bytes = write(fd_to, &buffer, original_bytes);

        if(original_bytes != coppied_bytes)
        {
            printf("Error writing to file: %s\n", strerror(errno));
            exit(-1);
        }
    }
}


int main(int argc, char** argv)
{
    char MODE = getopt(argc, argv, "mh");

    if (MODE == 'h' || argc < 2)
    {
        help();
        return 0;
    }
    else
    {
        int inputfile = open(argv[optind], O_RDONLY);

        if (inputfile < 0)
        {
            printf("Could not open input file: %s\n", strerror(errno));
            exit(-1);
        }
        
        int outputfile = open(argv[optind +1], O_RDWR | O_CREAT, 0666);

        if (outputfile < 0)
        {
            printf("Could not create or open output file: %s\n", strerror(errno));
            exit(-1);
        }
        if (MODE == 'm')
        {
			printf("Mode m has been choosen, files are mapped to memory regions with mmap() and copied with memcpy()\n");
            copy_map(inputfile, outputfile);
        }
        else
        {
			printf("No mode has been choosen, using read() and write() to copy file contents\n");

            copy_read_write(inputfile, outputfile);
        }


        if (close(inputfile) < 0)
        {
            printf("Error while closing input file: %s\n", strerror(errno));
            exit(-1);
        }

        if (close(outputfile) < 0)
        {
            printf("Error while closing output file: %s\n", strerror(errno));
            exit(-1);
        }
    }
    return 0;
}