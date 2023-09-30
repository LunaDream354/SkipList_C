#ifndef SKIP_LIST_H
#define SKIP_LIST_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Skip_node {
    void *data;
    struct Skip_node *next;
} Skip_node;

typedef struct Skip_skip {
    Skip_node *next;
    struct Skip_skip *next_skip;
} Skip_skip;

typedef struct Skip_head {
    Skip_skip *skips;
    size_t size;
    size_t jumpSize;
    Skip_node *next;
} Skip_head;

typedef bool (*Skip_compare)(void *, void *);

Skip_head *skip_create_list(size_t jump_size);
void skip_push(Skip_head *head, void *data);
void *skip_pop(Skip_head *head);
void skip_add_at(Skip_head *head, size_t position, void *data);
void *skip_remove_at(Skip_head *head, size_t position);
void *skip_get_at(Skip_head *head, size_t position);
Skip_node *skip_node_get_at(Skip_head *head, size_t position);

void **skip_search(Skip_head *head, Skip_compare search);
void skip_sort(Skip_head *head, Skip_compare sort);
void skip_skip_organize(Skip_head *head, size_t jump_size);
void skip_free(Skip_head **head);

Skip_head *skip_create_list(size_t jump_size) {
    if (!jump_size) {
        printf("\nJump size is equal to zero\n");
        exit(1);
    }
    Skip_head *head = (Skip_head *)malloc(sizeof(Skip_head));
    if (head == NULL) {
        printf("couldnt allocate memory");
        exit(1);
    }
    head->size = 0;
    head->jumpSize = jump_size;
    head->next = NULL;
    head->skips = (Skip_skip *)malloc(sizeof(Skip_skip));
    head->skips->next = NULL;
    head->skips->next_skip = NULL;
    return head;
}

void skip_push(Skip_head *head, void *data) {
    if (head == NULL) {
        printf("\nHead is null\n");
        exit(1);
    }
    Skip_node *node = (Skip_node *)malloc(sizeof(Skip_node));
    node->data = data;
    if (!head->size) {
        head->next = node;
        head->skips->next = node;
        head->size++;
        return;
    }
    node->next = head->next;
    head->next = node;
    head->size++;
    skip_skip_organize(head, head->jumpSize);
}

void *skip_pop(Skip_head *head) {
    if (head == NULL) {
        printf("\nHead is null\n");
        exit(1);
    }
    if (!head->size) {
        return;
    }
    Skip_node *node = skip_node_get_at(head, head->size - 1);
    void *data = node->data;
    if (head->size > 1)
        skip_node_get_at(head, head->size - 2)->next = NULL;
    free(node);
    skip_skip_organize(head, head->jumpSize);
    return data;
}

void skip_add_at(Skip_head *head, size_t position, void *data) {
    if (head == NULL) {
        printf("\nHead is null\n");
        exit(1);
    }
    if (position > head->size) {
        printf("\nInvalid postion to insert\n");
        exit(1);
    }
    if (!position) {
        skip_push(head, data);
        return;
    }
    size_t current_jumps_needed = position / head->jumpSize;
    size_t before_jumps_needed = (position - 1) / head->jumpSize;
}

Skip_node *skip_node_get_at(Skip_head *head, size_t position) {
    if (head == NULL) {
        printf("\nHead is null\n");
        exit(1);
    }
    if (position >= head->size) {
        printf("\nPosition not valid\n");
        exit(1);
    }
    size_t jumps_required = position / head->jumpSize;
    if (!jumps_required) {
        Skip_node *node_crawler = head->next;
        for (size_t i = 0; i < position - 1; i++)
            node_crawler = node_crawler->next;
        return node_crawler;
    } else {
        Skip_skip *skip_crawler = head->skips;
        for (size_t i = 0; i < jumps_required - 1; i++)
            skip_crawler = skip_crawler->next_skip;
        Skip_node *node_crawler = head->next;
        for (size_t i = 0; i < position % head->jumpSize; i++)
            node_crawler = node_crawler->next;
        return node_crawler;
    }
}

void skip_skip_organize(Skip_head *head, size_t jump_size) {
    head->jumpSize = jump_size;
    size_t skip_needed = (head->size - 1) / jump_size - 1;

    // 1 2 3 4 5
    //     |    
    // 1 2 3 4 5
    //   |   |  
    skip_needed -= (head->size - 1) % jump_size == 0 ? 1 : 0;
    Skip_skip *skip_crawler = head->skips;
    size_t skip_quant = 0;
    while (skip_crawler != NULL) {
        skip_quant++;
        skip_crawler = skip_crawler->next_skip;
    }
    // see if skip quantity need to change
    if (skip_quant > skip_needed) {
        skip_crawler = head->skips;
        Skip_skip *skip_new_first = head->skips->next_skip;
        for (size_t i = 0; i < skip_quant - skip_needed; i++) {
            skip_new_first = skip_crawler->next_skip;
            free(skip_crawler);
            skip_crawler = skip_new_first;
        }
        head->skips = skip_new_first;
    } else if (skip_quant < skip_needed) {
        skip_crawler = head->skips;
        while (skip_crawler->next_skip != NULL) {
            skip_crawler = skip_crawler->next_skip;
        }
        for (size_t i = 0; i < skip_needed - skip_quant; i++) {
            skip_crawler->next_skip = (Skip_skip *)calloc(1, sizeof(Skip_skip));
            if (skip_crawler == NULL) {
                printf("couldnt allocate memory");
                exit(1);
            }
            skip_crawler = skip_crawler->next_skip;
        }
    }
    // attribute the new skips
    skip_crawler = head->skips;
    Skip_node *node_crawler = head->next;
    while (skip_crawler != NULL) {
        for (size_t i = 0; i < head->jumpSize; i++)
            node_crawler = node_crawler->next;
        skip_crawler->next = node_crawler;
        skip_crawler = skip_crawler->next_skip;
    }
}

#endif