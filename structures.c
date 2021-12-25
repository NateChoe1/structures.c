//static functions basically mean that you shouldn't use them.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TRIVIAL 1
#define FATAL 2

#define INITIAL_ALLOCATED 5

#define MIN(a, b) ((a < b) ? a:b)

typedef struct {
	int *data;
	int len;
	int allocatedLen;
} ArrayList;

ArrayList *newArrayList() {
	ArrayList *ar = malloc(sizeof(ArrayList));
	if (ar == NULL)
		return NULL;
	ar->len = 0;
	ar->allocatedLen = INITIAL_ALLOCATED;
	ar->data = malloc(ar->allocatedLen * sizeof(int));
	return ar;
}

int addArrayList(ArrayList *ar, int val) {
	if (ar->len >= ar->allocatedLen) {
		ar->allocatedLen *= 2;
		int *newdata = realloc(ar->data, ar->allocatedLen * sizeof(int));
		if (newdata == NULL) {
			ar->allocatedLen /= 2;
			return TRIVIAL;
		}
		ar->data = newdata;
	}
	ar->data[ar->len++] = val;
	return 0;
}

int removeArrayList(ArrayList *ar, int ind) {
	memmove(ar->data + ind, ar->data + ind + 1, ar->len - ind - 1);
	ar->len--;
	return 0;
}

typedef struct BST {
	int val;
	struct BST *left;
	struct BST *right;
} BST;

BST *newBST(int val) {
	BST *bst = malloc(sizeof(BST));
	if (bst == NULL)
		return NULL;
	bst->val = val;
	bst->left = NULL;
	bst->right = NULL;
	return bst;
}

int addBST(BST *bst, int val) {
	for (;;) {
		if (val < bst->val) {
			if (bst->left == NULL) {
				bst->left = newBST(val);
				return (bst->left == NULL) ? TRIVIAL:0;
			}
			//else
			bst = bst->left;
		}
		else {
			if (bst->right == NULL) {
				bst->right = newBST(val);
				return (bst->right == NULL) ? TRIVIAL:0;
			}
			//else
			bst = bst->right;
		}
	}
}

bool inBST(BST *bst, int val) {
	while (bst != NULL) {
		if (bst->val == val)
			return true;
		if (val < bst->val)
			bst = bst->left;
		else
			bst = bst->right;
	}
	return false;
}

typedef struct LinkedNode {
	int val;
	struct LinkedNode *next;
} LinkedNode;

typedef struct {
	LinkedNode *start;
	LinkedNode *end;
} LinkedList;

LinkedList *newLinkedList() {
	LinkedList *ret = malloc(sizeof(LinkedList));
	if (ret == NULL)
		return NULL;
	ret->start = NULL;
	ret->end = NULL;
	return ret;
}

LinkedNode *newLinkedNode(int val) {
	LinkedNode *node = malloc(sizeof(LinkedNode));
	if (node == NULL)
		return NULL;
	node->val = val;
	return node;
}

int initLinkedList(LinkedList *list, int val) {
	LinkedNode *node = newLinkedNode(val);
	if (node == NULL)
		return TRIVIAL;
	node->next = NULL;
	list->start = node;
	list->end = node;
	return 0;
}

int appendLinkedList(LinkedList *list, int val) {
	if (list->end == NULL)
		return initLinkedList(list, val);
	LinkedNode *node = newLinkedNode(val);
	if (node == NULL)
		return TRIVIAL;
	list->end->next = node;
	list->end = node;
	return 0;
}

int prependLinkedList(LinkedList *list, int val) {
	if (list->start == NULL)
		return initLinkedList(list, val);
	LinkedNode *node = newLinkedNode(val);
	if (node == NULL)
		return TRIVIAL;
	node->next = list->start;
	list->start = node;
	return 0;
}

#define HASH_SPACE 233

typedef struct {
	LinkedList *space[HASH_SPACE];
} HashSet;
//It would be trivial to convert a hashset into a hashmap, I just don't feel
//like doing that given that the purpose of this demo is to get into a class.

HashSet *newHashSet() {
	HashSet *ret = malloc(sizeof(HashSet));
	for (int i = 0; i < HASH_SPACE; i++) {
		ret->space[i] = newLinkedList();
		if (ret->space[i] == NULL) {
			for (int j = 0; j < i; j++)
				free(ret->space[j]);
			return NULL;
		}
	}
	return ret;
}

int hash(int n) {
	return n % HASH_SPACE;
}

int addHashSet(HashSet *set, int n) {
	LinkedList *list = set->space[hash(n)];
	LinkedNode *node = list->start;
	while (node != NULL) {
		if (node->val == n)
			return 0;
		node = node->next;
	}
	return appendLinkedList(list, n);
}

bool inHashSet(HashSet *set, int n) {
	LinkedList *list = set->space[hash(n)];
	LinkedNode *node = list->start;
	while (node != NULL) {
		if (node->val == n)
			return true;
		node = node->next;
	}
	return false;
}

typedef ArrayList Heap;
Heap *(*newHeap)() = newArrayList;

int smallestElementHeap(Heap *heap) {
	if (heap->len == 0)
		return -1;
	return heap->data[0];
}

int addHeap(Heap *heap, int val) {
	int code = addArrayList(heap, val);
	if (code)
		return code;
	int ind = heap->len - 1;
	while (ind) {
		if (val < heap->data[(ind - 1) / 2]) {
			int backup = heap->data[ind];
			heap->data[ind] = heap->data[(ind - 1) / 2];
			heap->data[(ind - 1) / 2] = backup;
		}
		else
			break;
	}
	return 0;
}

static void swapItemsArrayList(ArrayList *ar, int i1, int i2) {
	int backup = ar->data[i1];
	ar->data[i1] = ar->data[i2];
	ar->data[i2] = backup;
}

int popHeap(Heap *heap) {
	if (heap->len == 0)
		return TRIVIAL;
	heap->data[0] = heap->data[heap->len - 1];
	heap->len--;
	int ind = 0;
	for (;;) {
		if (ind * 2 + 1 >= heap->len)
			return FATAL;
		if (ind * 2 + 2 >= heap->len) {
			if (heap->data[ind] > heap->data[ind * 2 + 1])
				swapItemsArrayList(heap, ind, ind * 2 + 1);
		}
		else {
			int min = MIN(heap->data[ind * 2 + 1],
			              heap->data[ind * 2 + 2]);
			if (min > heap->data[ind])
				return 0;
			if (heap->data[ind * 2 + 1] < heap->data[ind * 2 + 2])
				swapItemsArrayList(heap, ind, ind * 2 + 1);
			else
				swapItemsArrayList(heap, ind, ind * 2 + 2);
		}
	}
}

int main() {
	ArrayList *ar = newArrayList();
	if (ar == NULL)
		goto error;

	int toadd[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	for (int i = 0; i < sizeof(toadd) / sizeof(toadd[0]); i++) {
		printf("Adding %d to arraylist\n", toadd[i]);
		if (addArrayList(ar, toadd[i]))
			goto error;
	}
	printf("Arraylist contents: ");
	for (int i = 0; i < ar->len; i++)
		printf("%d ", ar->data[i]);
	puts("\n");

	BST *bst = newBST(-1);
	if (bst == NULL)
		goto error;
	for (int i = 0; i < 5; i++) {
		int val = random() % 10;
		printf("Adding %d to BST\n", val);
		if (addBST(bst, val % 10))
			goto error;
	}
	for (int i = 0; i < 10; i++) {
		if (inBST(bst, i))
			printf("%d is in the BST\n", i);
		else
			printf("%d is not in the BST\n", i);
	}

	putchar('\n');

	LinkedList *list = newLinkedList();
	if (list == NULL)
		goto error;
	for (int i = 0; i < 5; i++) {
		printf("Appending %d to the linkedlist\n", i);
		appendLinkedList(list, i);
	}
	for (int i = 5; i < 10; i++) {
		printf("Prepending %d to the linkedlist\n", i);
		prependLinkedList(list, i);
	}
	LinkedNode *node = list->start;
	printf("List order: ");
	while (node != NULL) {
		printf("%d ", node->val);
		node = node->next;
	}

	puts("\n");

	HashSet *hashset = newHashSet();
	if (hashset == NULL)
		goto error;
	for (int i = 0; i < 5; i++) {
		int val = random() % 10;
		printf("Adding %d to hashset\n", val);
		if (addHashSet(hashset, val % 10))
			goto error;
	}
	for (int i = 0; i < 10; i++) {
		if (inHashSet(hashset, i))
			printf("%d is in the hashset\n", i);
		else
			printf("%d is not in the hashset\n", i);
	}
	putchar('\n');

	Heap *heap = newHeap();
	if (heap == NULL)
		goto error;
	for (int i = 0; i < 5; i++) {
		int val = rand() % 10;
		printf("Adding %d to the heap\n", val);
		if (addHeap(heap, val))
			goto error;
	}
	printf("Smallest value: %d\n", smallestElementHeap(heap));
	popHeap(heap);
	printf("Second smallest value: %d\n", smallestElementHeap(heap));

	exit(EXIT_SUCCESS);
error:
	fprintf(stderr, "Something went wrong!\n");
	exit(EXIT_FAILURE);
}


/*
 * With hashsets, queues, arraylists, and heaps, it's pretty trivial to create
 * a whole lot of other data structures. This entire demo was just to get into
 * a class without taking a data structures prerequisite, which is why a lot of
 * this code sucks. I mean, there's also the fact that I'm trying to use object
 * oriented paradigms in C, but let's not dwell on details here.
 * */
