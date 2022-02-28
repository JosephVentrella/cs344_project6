#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ALIGNMENT 16   // Must be power of 2
#define GET_PAD(x) ((ALIGNMENT - 1) - ((x - 1) & (ALIGNMENT - 1)))
#define PADDED_SIZE(x) ((x) + GET_PAD(x))
#define PTR_OFFSET(p, offset) ((void*)((char *)(p) + (offset)))

struct block {
    struct block *next;
    int size;     // Bytes
    int in_use;   // Boolean
};

struct block *head = NULL;

void split_space(struct block *current_node, int requested_size){
    int padded_struct_block_size = PADDED_SIZE(sizeof(struct block));
    int required_space = requested_size + padded_struct_block_size;
    int available_space = current_node->size;
    if (available_space > required_space){
        struct block *new_node = PTR_OFFSET(current_node, padded_struct_block_size);
        new_node->next = NULL;
        new_node->size = available_space-required_space;
        new_node->in_use = 0;
        current_node->size = requested_size;
        current_node->next = new_node;
    }
}

void myfree(void *p){
    struct block *freed = p - PADDED_SIZE(sizeof(struct block));
    struct block *cur = head;
    freed->in_use = 0;
    while (cur->next != NULL)
    {
        if (cur->in_use == 0 && cur->next->in_use == 0){
            cur->size = cur->next->size + cur->size + PADDED_SIZE(sizeof(struct block));
            cur->next = cur->next->next;
        }
        else {
            cur = cur->next;
        }
    }
    
}

void* myalloc(int bytes ){
    if(bytes <= 1008){
        if (head == NULL) {
            head = sbrk(1024);
            head->next = NULL;
            head->size = 1024 - PADDED_SIZE(sizeof(struct block));
            head->in_use = 0;
        }
        struct block *cur = head;
        while(cur != NULL){
            if(cur->in_use != 1 && cur->size >= PADDED_SIZE(bytes)){
                split_space(cur, PADDED_SIZE(bytes));
                cur->in_use = 1;
                int padded_block_size = PADDED_SIZE(sizeof(struct block));
                return PTR_OFFSET(cur, padded_block_size);
            }
            cur = cur->next;
        }
    }   
    return NULL;
}



void print_data(void)
{
    struct block *b = head;

    if (b == NULL) {
        printf("[empty]\n");
        return;
    }

    while (b != NULL) {
        // Uncomment the following line if you want to see the pointer values
        //printf("[%p:%d,%s]", b, b->size, b->in_use? "used": "free");
        printf("[%d,%s]", b->size, b->in_use? "used": "free");
        if (b->next != NULL) {
            printf(" -> ");
        }

        b = b->next;
    }

    printf("\n");
}

int main(void) {
    void *p, *q, *r, *s;

    p = myalloc(10); print_data();
    q = myalloc(20); print_data();
    r = myalloc(30); print_data();
    s = myalloc(40); print_data();

    myfree(q); print_data();
    myfree(p); print_data();
    myfree(s); print_data();
    myfree(r); print_data();
}