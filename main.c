#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "tosfs.h"
int main(){
    struct stat statsOfFile;

    int fd = open("test_tosfs_files",O_RDWR); 
    if (fd==-1){
        perror ("Error open");
        return EXIT_FAILURE;
    }
    int fileStat = fstat(fd, &statsOfFile);
    if (fileStat==-1){
        perror ("Error fstat");
        return EXIT_FAILURE;
    }
    void *fs_data = mmap(NULL, statsOfFile.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fs_data==MAP_FAILED){
        perror("error mapping");
        return EXIT_FAILURE;
    }

    struct tosfs_superblock* superBlock = (struct tosfs_superblock*) (fs_data);

    printf("magic number: %x\n", superBlock->magic);
    printf("block bitmap: " PRINTF_BINARY_PATTERN_INT32"\n", PRINTF_BYTE_TO_BINARY_INT32(superBlock->block_bitmap));
    printf("inode bitmap: " PRINTF_BINARY_PATTERN_INT32"\n", PRINTF_BYTE_TO_BINARY_INT32(superBlock->inode_bitmap));
    printf("block size:   %x\n", superBlock->block_size);
    printf("blocks:       %x\n", superBlock->blocks);
    printf("inodes:       %x\n", superBlock->inodes);
    printf("root inode:   %x\n", superBlock->root_inode);

    struct tosfs_inode *inode_table= (struct tosfs_inode *)(fs_data + TOSFS_INODE_BLOCK * TOSFS_BLOCK_SIZE);

    printf("\nInodes:\n");
    for (int i = 0; i < superBlock->inodes; i++) {
        printf("Inode %d:\n", inode_table[i].inode);
        printf("  Bloc de données: %d\n", inode_table[i].block_no);
        printf("  UID: %d, GID: %d\n", inode_table[i].uid, inode_table[i].gid);
        printf("  Mode: %d, Permissions: %d\n", inode_table[i].mode, inode_table[i].perm);
        printf("  Taille: %d octets\n", inode_table[i].size);
        printf("  Nombre de liens: %d\n", inode_table[i].nlink);
    }
    return 0;
}