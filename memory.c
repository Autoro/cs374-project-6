#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#define ALIGNMENT 16
#define GET_PAD(x) ((ALIGNMENT - 1) - ((x - 1) & (ALIGNMENT - 1)))
#define PADDED_SIZE(x) (x + GET_PAD(x))
#define PTR_OFFSET(p, offset) ((void*)((char*)p + offset))

typedef struct block block;
struct block
{
    int size;
    int in_use;
    block* next;
};

const int PADDED_BLOCK_SIZE = PADDED_SIZE(sizeof(block));
const int MEMORY_SIZE = 1024;

block* head = NULL;

void* myalloc(int size)
{
    if (head == NULL)
    {
        head = mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
        head->size = MEMORY_SIZE - PADDED_BLOCK_SIZE;
        head->in_use = 0;
        head->next = NULL;
    }

    int data_size = PADDED_SIZE(size);
    block* current = head;
    while (current != NULL && (current->in_use == 1 || current->size < data_size))
    {
        current = current->next;
    }

    if (current != NULL)
    {
        current->in_use = 1;

        return PTR_OFFSET(current, PADDED_BLOCK_SIZE);
    }

    return NULL;
}

void print_data()
{
    if (head == NULL)
    {
        printf("[empty]\n");
        return;
    }
    
    block* current = head;
    while (current != NULL)
    {
        if (current != head)
        {
            printf(" -> ");
        }
        
        printf("[%d,%s]", current->size, current->in_use ? "used" : "free");

        current = current->next;
    }

    printf("\n");
}

int main ()
{
    void* p;

    print_data();
    p = myalloc(1);
    print_data();
    
    printf("%p\n", p);
}
