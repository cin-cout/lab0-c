#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q) {
        return NULL;
    }
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l) {
        return;
    }
    struct list_head *node;
    struct list_head *safe;
    list_for_each_safe (node, safe, l) {
        q_release_element(list_entry(node, element_t, list));
    }
    test_free(l);
}


/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *q = malloc(sizeof(element_t));
    if (!q) {
        return false;
    }
    int s_len = strlen(s);
    q->value = malloc(s_len + 1);
    if (!(q->value)) {
        test_free(q);
        return false;
    }
    strncpy(q->value, s, s_len);
    q->value[s_len] = '\0';
    list_add(&q->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *q = malloc(sizeof(element_t));
    if (!q) {
        return false;
    }
    int s_len = strlen(s);
    q->value = malloc(s_len + 1);
    if (!(q->value)) {
        test_free(q);
        return false;
    }
    strncpy(q->value, s, s_len);
    q->value[s_len] = '\0';
    list_add_tail(&q->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *cur_r = list_entry(head->next, element_t, list);
    list_del(head->next);
    if (sp) {
        strncpy(sp, cur_r->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return cur_r;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *cur_r = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp) {
        strncpy(sp, cur_r->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return cur_r;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;
    }
    int len = 0;
    struct list_head *node;
    list_for_each (node, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *slow = head->next;
    struct list_head *fast = head->next;
    while (fast != head && fast != head->prev) {
        slow = slow->next;
        fast = fast->next->next;
    }
    // del slow
    list_del_init(slow);
    element_t *cur_f = list_entry(slow, element_t, list);
    free(cur_f);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head) {
        return false;
    }
    struct list_head *cur = head;
    while (cur->next != head && cur->next->next != head) {
        element_t *c = list_entry(cur->next, element_t, list),
                  *n = list_entry(cur->next->next, element_t, list);
        if (!strcmp(c->value, n->value)) {
            while (cur->next->next != head && !strcmp(c->value, n->value)) {
                struct list_head *next = n->list.next;
                list_del(cur->next->next);
                q_release_element(n);
                n = list_entry(next, element_t, list);
            }
            list_del(cur->next);
            q_release_element(c);
        }
        cur = cur->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *node;
    list_for_each (node, head) {
        if (node->next == head) {
            break;
        }
        list_move(node, node->next);
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *node;
    struct list_head *safe;
    struct list_head *tmp;
    list_for_each_safe (node, safe, head) {
        tmp = node->prev;
        node->prev = node->next;
        node->next = tmp;
    }
    tmp = head->prev;
    head->prev = head->next;
    head->next = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *node, *safe, *tmp_head = head->next, *tmp;
    int cur_k = 1;
    list_for_each_safe (node, safe, head) {
        if (cur_k == 1) {
            tmp = node;
            for (int i = 1; i < k; i++) {
                tmp = tmp->next;
                if (tmp == head) {
                    return;
                }
            }
            tmp_head = node;
        } else {
            list_move_tail(node, tmp_head);
            tmp_head = node;
        }
        if (cur_k == k) {
            cur_k = 1;
        } else {
            cur_k++;
        }
    }
}

struct list_head *merge_two_queue(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; L1 && L2; *node = (*node)->next) {
        node = (strcmp(list_entry(L1, element_t, list)->value,
                       list_entry(L2, element_t, list)->value) >= 0)
                   ? &L2
                   : &L1;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) (void *) ((uintptr_t)(void *) L1 |
                                          (uintptr_t)(void *) L2);
    return head;
}

static struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next) {
        return head;
    }
    struct list_head *slow = head, *fast = head->next;
    for (; fast && fast->next; fast = fast->next->next, slow = slow->next)
        ;
    fast = slow->next;
    slow->next = NULL;
    return merge_two_queue(merge_sort(head), merge_sort(fast));
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    head->prev->next = NULL;
    head->next = merge_sort(head->next);
    struct list_head *tmp;
    for (tmp = head; tmp->next; tmp = tmp->next) {
        tmp->next->prev = tmp;
    }
    tmp->next = head;
    head->prev = tmp;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head)
        return 0;
    int len = 0;
    struct list_head *cur = head->prev;
    while (cur != head) {
        len++;
        if (cur->prev == head)
            break;
        element_t *c = list_entry(cur, element_t, list),
                  *p = list_entry(cur->prev, element_t, list);
        if (strcmp(c->value, p->value) > 0) {
            list_del(&p->list);
            q_release_element(p);
            len--;
        } else {
            cur = cur->prev;
        }
    }
    return len;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *f_q = list_entry(head->next, queue_contex_t, chain);
    if (list_is_singular(head))
        return f_q->size;
    queue_contex_t *cur_q;
    f_q->q->prev->next = NULL;
    list_for_each_entry (cur_q, head, chain) {
        if (cur_q != f_q) {
            cur_q->q->prev->next = NULL;
            f_q->q->next = merge_two_queue(f_q->q->next, cur_q->q->next);
            INIT_LIST_HEAD(cur_q->q);
            f_q->size += cur_q->size;
            cur_q->size = 0;
        }
    }
    struct list_head *tmp;
    for (tmp = f_q->q; tmp->next; tmp = tmp->next) {
        tmp->next->prev = tmp;
    }
    tmp->next = f_q->q;
    f_q->q->prev = tmp;
    return f_q->size;
}