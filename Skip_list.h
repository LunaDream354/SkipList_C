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

Skip_head *skip_search(Skip_head *head, void *data, Skip_compare search);
void skip_sort(Skip_head *head, Skip_compare sort);
void skip_skip_organize(Skip_head *head, size_t jump_size);
void skip_swap_postions(Skip_head *head, size_t position_a, size_t position_b);
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
        head->size++;
        return;
    }
    Skip_node *node_start = head->next;
    head->next = node;
    node->next = node_start;
    head->size++;
    skip_skip_organize(head, head->jumpSize);
}

void *skip_pop(Skip_head *head) {
    if (head == NULL) {
        printf("\nHead is null\n");
        exit(1);
    }
    if (!head->size) {
        return NULL;
    }
    Skip_node *node = skip_node_get_at(head, head->size - 1);
    void *data = node->data;
    if (head->size > 1)
        skip_node_get_at(head, head->size - 2)->next = NULL;
    free(node);
    head->size--;
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
    Skip_node *node_current = (Skip_node *)malloc(sizeof(Skip_node));
    node_current->data = data;
    node_current->next = NULL;
    if (position != head->size) {
        Skip_node *node_front = skip_node_get_at(head, position);
        node_current->next = node_front;
    }
    Skip_node *node_back = skip_node_get_at(head, position - 1);
    node_back->next = node_current;
    head->size++;
    skip_skip_organize(head, head->jumpSize);
}

void *skip_remove_at(Skip_head *head, size_t position) {
    if (head == NULL) {
        printf("\nHead is null\n");
        exit(1);
    }
    if (position > head->size) {
        printf("\nInvalid postion to insert\n");
        exit(1);
    }
    Skip_node *node_current = skip_node_get_at(head, position);
    void *node_data = node_current->data;
    if (position != head->size) {
        Skip_node *node_front = skip_node_get_at(head, position + 1);
        if (position) {
            Skip_node *node_behind = skip_node_get_at(head, position - 1);
            node_behind->next = node_front;
        } else {
            head->next = node_front;
        }
    }
    head->size--;
    skip_skip_organize(head, head->jumpSize);
    return node_data;
}

void *skip_get_at(Skip_head *head, size_t position) {
    return skip_node_get_at(head, position)->data;
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
        for (size_t i = 0; i < position; i++)
            node_crawler = node_crawler->next;
        return node_crawler;
    } else {
        Skip_skip *skip_crawler = head->skips;
        for (size_t i = 0; (long)i < (long)jumps_required - 1; i++)
            skip_crawler = skip_crawler->next_skip;
        Skip_node *node_crawler = skip_crawler->next;
        for (size_t i = 0; i < position % head->jumpSize; i++)
            node_crawler = node_crawler->next;
        return node_crawler;
    }
}

Skip_head *skip_search(Skip_head *head, void *data, Skip_compare search) {
    if (head == NULL) {
        printf("\nHead is null\n");
        exit(1);
    }
    if (search == NULL) {
        printf("\nSearch function is null\n");
        exit(1);
    }
    Skip_head *head_return = skip_create_list(5);
    Skip_node *node_crawler = head->next;
    while (node_crawler != NULL) {
        if (search(data, node_crawler->data))
            skip_push(head_return, node_crawler->data);
        node_crawler = node_crawler->next;
    }
    return head_return;
}

void skip_sort(Skip_head *head, Skip_compare sort) {
    if (head == NULL) {
        printf("\nHead is null\n");
        exit(1);
    }

    Skip_node *to_search_crawler = head->next;
    for (size_t to_search = 0; to_search < head->size - 1; to_search++) {
        Skip_node *to_compare_crawler = to_search_crawler->next;
        for (size_t to_compare = to_search + 1; to_compare < head->size; to_compare++) {
            if (sort(to_search_crawler->data, to_compare_crawler->data)) {
                skip_swap_postions(head, to_compare, to_search);
            }
            to_search_crawler = to_search_crawler->next;
        }
    }
    skip_skip_organize(head, head->jumpSize);
}

void skip_skip_organize(Skip_head *head, size_t jump_size) {
    head->jumpSize = jump_size;
    size_t skip_needed = head->size / jump_size - 1;
    skip_needed += head->size % jump_size ? 1 : 0;
    if (head->size < jump_size)
        skip_needed = 0;
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
            if (skip_crawler->next_skip == NULL) {
                printf("couldnt allocate memory");
                exit(1);
            }
            skip_crawler = skip_crawler->next_skip;
        }
    }
    // attribute the new skips
    skip_crawler = head->skips;
    Skip_node *node_crawler = head->next;
    Skip_skip *skip_crawler_before = NULL;
    while (skip_crawler != NULL) {
        for (size_t i = 0; i < head->jumpSize; i++)
            node_crawler = node_crawler->next;
        skip_crawler->next = node_crawler;
        if (skip_crawler_before != NULL)
            skip_crawler_before->next_skip = skip_crawler;
        skip_crawler_before = skip_crawler;
        skip_crawler = skip_crawler->next_skip;
    }
}

void skip_swap_postions(Skip_head *head, size_t a, size_t b) {
    Skip_node *a_current = skip_node_get_at(head, a);
    Skip_node **a_after = (Skip_node **)calloc(1, sizeof(Skip_node *));
    if (a < head->size - 1) {
        // free(a_after);
        *a_after = skip_node_get_at(head, a + 1);
    }
    Skip_node **a_before = (Skip_node **)calloc(1, sizeof(Skip_node *));
    if (a == 1) {
        free(a_before);
        a_before = &head->next;
    } else if (a > 1)
        *a_before = skip_node_get_at(head, a - 1);

    Skip_node *b_current = skip_node_get_at(head, b);
    Skip_node **b_before = (Skip_node **)calloc(1, sizeof(Skip_node *));
    if (b == 1) {
        free(b_before);
        b_before = &head->next;
    } else if (b > 1)
        *b_before = skip_node_get_at(head, b - 1);
    Skip_node **b_after = (Skip_node **)calloc(1, sizeof(Skip_node *));
    if (b < head->size - 1) {
        free(a_after);
        *b_after = skip_node_get_at(head, a + 1);
    }
    if (*a_before != NULL)
        (*a_before)->next = b_current;
    else
        head->next = b_current;
    b_current->next = *a_after;
    if (*b_before != NULL)
        (*b_before)->next = a_current;
    else
        head->next = a_current;
    a_current->next = *b_after;
}
void skip_free(Skip_head **head) {
    Skip_node *node_crawler = (*head)->next;
    while (node_crawler != NULL) {
        Skip_node *node_crawler_next = node_crawler->next;
        free(node_crawler);
        node_crawler = node_crawler_next;
    }
    Skip_skip *skip_crawler = (*head)->skips;
    while (skip_crawler != NULL) {
        Skip_node *skip_crawler_next = skip_crawler->next;
        free(skip_crawler);
        node_crawler = skip_crawler_next;
    }
    *head = NULL;
}
#endif