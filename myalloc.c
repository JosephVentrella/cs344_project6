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
            if(cur->in_use != 1){
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
        // printf("[%p:%d,%s]", b, b->size, b->in_use? "used": "free");
        printf("[%d,%s]", b->size, b->in_use? "used": "free");
        if (b->next != NULL) {
            printf(" -> ");
        }

        b = b->next;
    }

    printf("\n");
}

int main(void) {
  // Allocate space for 5 ints
    void *p;

    print_data();
    p = myalloc(16);
    print_data();
    p = myalloc(16);
    printf("%p\n", p);
}