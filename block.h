//
// Created by Olya on 03.11.2020.
//

#ifndef UNTITLED1_BLOCK_H
#define UNTITLED1_BLOCK_H

#include <stdio.h>

#define BUFFER_SIZE 1024

typedef struct block{
    bool status;
    size_t previous_size;
    size_t size;
} Block_t;


#endif //UNTITLED1_BLOCK_H
