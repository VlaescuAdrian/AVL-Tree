#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 1024
#define ELEMENT_TREE_LENGTH 3

#include "AVLTree.h"

int min(int a, int b){
    if(a < b)
        return a;
    return b;
}

typedef struct Range{
    int *index;
    int size;
    int capacity;
}Range;

void printFile(char* fileName){
    if(fileName == NULL) return;
    FILE * file = fopen(fileName,"r");
    if (file == NULL) return;
    char *buf = (char*) malloc(BUFLEN+1);
    while(fgets(buf,BUFLEN,file) != NULL){
        printf("%s",buf);
    }
    printf("\n");
    free(buf);
    fclose(file);
}

void printWordsInRangeFromFile(Range* range, char* fileName){
    if(fileName == NULL || range == NULL) return;
    FILE * file = fopen(fileName,"r");
    if (file == NULL) return;
    char *buf = (char*) malloc(BUFLEN+1);
    for(int i = 0; i < range->size;i++){
        fseek(file,range->index[i],SEEK_SET);
        if(fgets(buf,BUFLEN,file) != NULL){
            char* token = strtok(buf," .,\n");
            printf("%d. %s:%d\n",i+1, token, range->index[i]);
        }
    }
    printf("\n");
    free(buf);
    fclose(file);
}

void in_order(TTree *tree, TreeNode *node, char *q, Range *range){
    if(node->lt != NULL)
        in_order(tree, node->lt, q, range);
    int ok = 1;
    for(int i = 0; i < strlen(q); ++ i){
        if(q[i] != *((char*)(node->elem) + i)){
            ok = 0;
            break;
        }
    }
    if(ok){
        TreeNode* begin = node;
        TreeNode* end = node->end->next;
        while(begin != end){
            *(range->index + range->size) = *((int*)(begin->info));
            begin = begin->next;
            range->size ++;
        }
    }
    if(node->rt != NULL)
        in_order(tree, node->rt, q, range);
}

void in_order_multi(TTree *tree, TreeNode *node, char *q, char *p, Range *range){
    if(node->lt != NULL)
        in_order_multi(tree, node->lt, q, p, range);
    int ok = 1;
    int i = 0;
    for(i = 0; i < min(strlen(q), strlen(p)); ++ i){
        if(q[i] > *((char*)(node->elem) + i) || p[i] < *((char*)(node->elem) + i)){
            ok = 0;
            break;
        }
    }
    for(; i < strlen(q); ++ i){
        if(q[i] > *((char*)(node->elem) + i)){
            ok = 0;
            break;
        }
    }
    for(; i < strlen(p); ++ i){
        if(p[i] < *((char*)(node->elem) + i)){
            ok = 0;
            break;
        }
    }
    if(ok){
        TreeNode* begin = node;
        TreeNode* end = node->end->next;
        while(begin != end){
            *(range->index + range->size) = *((int*)(begin->info));
            begin = begin->next;
            range->size ++;
        }
    }
    if(node->rt != NULL)
        in_order_multi(tree, node->rt, q, p, range);
}


void printTreeInOrderHelper(TTree* tree, TreeNode* node){
    if(node != NULL){
        printTreeInOrderHelper(tree, node->lt);
        TreeNode* begin = node;
        TreeNode* end = node->end->next;
        while(begin != end){
            printf("%d:%s  ",*((int*)begin->info),((char*)begin->elem));
            begin = begin->next;
        }
        printTreeInOrderHelper(tree, node->rt);
    }
}

void printTreeInOrder(TTree* tree){
    if(tree == NULL) return;
    printTreeInOrderHelper(tree, tree->root);
}


void* createStrElement(void* str){
    // TODO: Cerinta 2
    /* Allocate a string element to store in the tree.
     */
    char *string = (char*)calloc(20, sizeof(char));
    if(string == NULL)
        return NULL;
    strcpy(string, (char*)str);
    string[3] = NULL;
    return string;
}

void destroyStrElement(void* elem){
    // TODO: Cerinta 2
    /* Free a string element.
     */
    free(elem);
}


void* createIndexInfo(void* index){
    // TODO: Cerinta 2
    /* Allocate space to store a new index
     */
    return ((int*)index);
}

void destroyIndexInfo(void* index){
    // TODO: Cerinta 2
    /* Free storage space for the index.
     */
    free(index);
}

int compareStrElem(void* str1, void* str2){
    // TODO: Cerinta 2
    /*  0  means strings are equal
     * -1  means str1 < str2
     *  1  means str1 > str2
     */
    if(strcmp((char*)str1, (char*)str2) < 0)
        return -1;
    else if(strcmp((char*)str1, (char*)str2) > 0)
        return 1;
    return 0;
}


TTree* buildTreeFromFile(char* fileName){
    // TODO: Cerinta 2
    /* Parse the text file keeping track of indices!
     * Words separators are the following: " :.,\r\n"
     * At each step insert the <token, index> pair in the tree.
     */
    TTree *tree = createTree(createStrElement, destroyStrElement, createIndexInfo, destroyIndexInfo, compareStrElem);
    if(fileName == NULL) return NULL;
    FILE * file = fopen("text.txt","r");
    if (file == NULL) return NULL;
    char ch, str[1002], *token;
    int k = 0, ant = 0, cnt = 1;
    int *index_curr = (int*)calloc(1, sizeof(int));
    *index_curr = 0;
    while((ch = fgetc(file)) != EOF)
        str[k++] = ch;
    token = strtok(str, ". ,'\n':");
    ant = strlen(token) + 1;
    token = createStrElement(token);
    insert(tree, token, index_curr);
    while(token != NULL){
        token = strtok(NULL, ". ,'\n':");
        ++ cnt;
        if(token == NULL)
            break;
        int *index_new = (int*)calloc(1, sizeof(int));
        *index_new = ant;
        insert(tree, createStrElement(token), index_new);
        ant = *index_new + strlen(token) + 1;
        if(cnt == 27)
            ++ ant;
    }
    return tree;
}


Range* singleKeyRangeQuery(TTree* tree, char* q){
    // TODO: Cerinta 3
    /* Get the words indices begining with the patern specified by q
     * How can do you traverse the tree?
     * There might be duplicates, can you make use of the tree duplicate lists?
     */
    Range *range = (Range*)calloc(1, sizeof(Range));
    range->index = (int*)calloc(1, sizeof(int));
    range->capacity = 20;
    range->size = 0;
    in_order(tree, tree->root, q, range);
    return range;
}

Range* multiKeyRangeQuery(TTree* tree, char* q, char* p){
    // TODO: Cerinta 4
    /* Get the words indices begining with the any patern
     * between q and p (inclusive).
     */
    Range *range = (Range*)calloc(1, sizeof(Range));
    range->index = (int*)calloc(1, sizeof(int));
    range->capacity = 20;
    range->size = 0;
    for(int i = 0; i < strlen(q); ++ i)
        if(p[i] < q[i]){
            char aux = q[i];
            q[i] = p[i];
            p[i] = aux;
        }
    in_order_multi(tree, tree->root, q, p, range);
    return range;
}


int main(void) {

    printf("The text file:\n");
    printFile("text.txt");

    TTree* tree = buildTreeFromFile("text.txt");
    printf("Tree In Order:\n");
    printTreeInOrder(tree);
    printf("\n\n");

    printf("Single search:\n");
    Range *range = singleKeyRangeQuery(tree,"v");
    printWordsInRangeFromFile(range,"text.txt");

    printf("Multi search:\n");
    Range *range2 = multiKeyRangeQuery(tree,"j","pr");
    printWordsInRangeFromFile(range2,"text.txt");

    if(range != NULL) free(range->index);
    free(range);

    if(range2 != NULL) free(range2->index);
    free(range2);

    destroyTree(tree);
    return 0;
}






