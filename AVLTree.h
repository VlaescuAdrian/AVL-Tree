#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <stdlib.h>

#define MAX(a, b) (((a) >= (b))?(a):(b))
#define HEIGHT(x) ((x)?((x)->height):(0))

/*
 IMPORTANT!
 
 As we stick to pure C, we cannot use templates. We will just assume
 some type Item was previously defined.
 */
// -----------------------------------------------------------------------------

typedef struct node{
    void* elem;
    void* info;
    struct node *pt;
    struct node *lt;
    struct node *rt;
    struct node* next;
    struct node* prev;
    struct node* end;
    long height;
}TreeNode;

typedef struct TTree{
    TreeNode *root;
    void* (*createElement)(void*);
    void (*destroyElement)(void*);
    void* (*createInfo)(void*);
    void (*destroyInfo)(void*);
    int (*compare)(void*, void*);
    long size;
}TTree;

long max(long a, long b){
    return a > b ? a : b;
}

int getHeight(TTree *tree, TreeNode *copy){
    int height = 1;
    if(copy->rt != NULL)
        height = max(height, copy->rt->height + 1);
    if(copy->lt != NULL)
        height = max(height, copy->lt->height + 1);
    return height;
}

TTree* createTree(void* (*createElement)(void*), void (*destroyElement)(void*),
                  void* (*createInfo)(void*), void (*destroyInfo)(void*),
                  int compare(void*, void*)){
    // TODO: Cerinta 1
    /* 1. This tree does NOT have any sentinels!!!
     * "root" is just a pointer to the actual root node.
     *
     * 2. You must set all function pointers in the structure!!!
     */
    TTree* tree = (TTree*)malloc(sizeof(TTree));
    tree->createElement = createElement;
    tree->destroyElement = destroyElement;
    tree->createInfo = createInfo;
    tree->destroyInfo = destroyInfo;
    tree->compare = compare;
    tree->root = NULL;
    tree->size = 0;
//    tree->root->height = 1;
//    tree->root->next = tree->root->prev = tree->root->lt = tree->root->rt = NULL;
//    tree->root->end = tree->root;
    
    return tree;
}

TreeNode* createTreeNode(TTree *tree, void* value, void* info){
    // TODO: Cerinta 1
    /* Implementing and using this function
     * will greatly improve your chances of
     * getting this to work.
     */
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    node->height = 1;
    node->pt = node->rt = node->lt = node->next = node->prev = NULL;
    node->end = node;
    node->info = tree->createInfo(info);
    node->elem = tree->createElement(value);
    return node;
}

void destroyTreeNode(TTree *tree, TreeNode* node){
    // TODO: Cerinta 1
    /* Implementing and using this function
     * will greatly improve your chances of
     * getting this to work.
     */
    free(node->info);
    free(node->elem);
    TreeNode *node_curr = node;
    if(node->prev != NULL || node->end->next != NULL){
        if(node->prev != NULL && node->end->next != NULL){
            node->prev->next = node->end->next;
            node->end->next->prev = node->prev;
        }
        else{
            if(node->prev == NULL)
                node->end->next->prev = NULL;
            if(node->end->next == NULL)
                node->prev->next = NULL;
        }
    }
    if(node->end == node){
        free(node);
        return;
    }
    do{
        node_curr = node_curr->next;
        free(node_curr->prev);
    }while(node_curr != node->end);
    free(node_curr);
}


int isEmpty(TTree* tree){
    // TODO: Cerinta 1
    if(tree == NULL || tree->root == NULL)
        return 1;
    return 0;
}

TreeNode* search(TTree* tree, TreeNode* x, void* elem){
    
    // TODO: Cerinta 1
    if(tree->compare(x->elem, elem) == 0)
        return x;
    if(tree->compare(x->elem, elem) > 0 && x->lt != NULL)
        return search(tree, x->lt, elem);
    if(tree->compare(x->elem, elem) < 0 && x->rt != NULL)
        return search(tree, x->rt, elem);
    return NULL;
}

TreeNode* minimum(TTree*tree, TreeNode* x){
    // TODO: Cerinta 1
    while(x->lt != NULL)
        x = x->lt;
    return x;
    return NULL;
}

TreeNode* maximum(TTree* tree, TreeNode* x){
    // TODO: Cerinta 1
    while(x->rt != NULL)
        x = x->rt;
    return x;
    return NULL;
}

TreeNode* successor(TTree* tree, TreeNode* x){
    // TODO: Cerinta 1
    if(x->rt != NULL)
        return minimum(tree, x->rt);
    while(x != tree->root && x->pt != tree->root && x->pt->lt != x)
        x = x->pt;
    if(x == tree->root || (x->pt == tree->root && x->pt->lt != x))
        return NULL;
    return x->pt;
}

TreeNode* predecessor(TTree* tree, TreeNode* x){
    // TODO: Cerinta 1
    if(x->lt != NULL)
        return maximum(tree, x->lt);
    while(x != tree->root && x->pt != tree->root && x->pt->rt != x)
        x = x->pt;
    if(x == tree->root || (x->pt == tree->root && x->pt->rt != x))
        return NULL;
    return x->pt;
}

void avlRotateLeft(TTree* tree, TreeNode* x){
    // TODO: Cerinta 1
    /* You may want to use the macros at the top of this file.
     */
    TreeNode *pivot = x->rt->lt;
    if(x == tree->root){
        x->pt = x->rt;
        x->rt->lt = x;
        tree->root = x->rt;
        x->rt = pivot;
        tree->root->pt = NULL;
    }
    else{
        if(x->pt->lt == x){
            x->pt->lt = x->rt;
        }
        else{
            x->pt->rt = x->rt;
        }
        x->rt->pt = x->pt;
        x->pt = x->rt;
        x->rt->lt = x;
        x->rt = pivot;
    }
    x->height = getHeight(tree, x);
    x->pt->height = getHeight(tree, x->pt);
}

void avlRotateRight(TTree* tree, TreeNode* y){
    // TODO: Cerinta 1
    /* You may want to use the macros at the top of this file.
     */
    TreeNode *pivot = y->lt->rt;
    if(y == tree->root){
        y->pt = y->lt;
        y->lt->rt = y;
        tree->root = y->lt;
        y->lt = pivot;
        tree->root->pt = NULL;
    }
    else{
        if(y->pt->lt == y){
            y->pt->lt = y->lt;
        }
        else{
            y->pt->rt = y->lt;
        }
        y->lt->pt = y->pt;
        y->pt = y->lt;
        y->lt->rt = y;
        y->lt = pivot;
    }
    y->height = getHeight(tree, y);
    y->pt->height = getHeight(tree, y->pt);
}


int avlGetBalance(TTree* tree, TreeNode *x){
    //TODO: // TODO: Cerinta 1
    
    /* Get AVL balance factor for node x.
     * You may want to use the macros at the top of this file.
     */
    int right, left;
    if(x->rt == NULL)
        right = 0;
    else
        right = x->rt->height;
    
    if(x->lt == NULL)
        left = 0;
    else
        left = x->lt->height;
    
    return left - right;
    
    return 0;
}


void avlFixUp(TTree* tree, TreeNode* y){
    //TODO: Cerinta 1
    /* Fix any unblance from this node to the top of tree
     */
    int balance = avlGetBalance(tree, y);
    while(y != tree->root && abs(balance) <= 1){
        y = y->pt;
        balance = avlGetBalance(tree, y);
    }
    int right, left;
    
    if(balance > 1){
        y = y->lt;
        if(y->rt == NULL)
            right = 0;
        else
            right = y->rt->height;
        
        if(y->lt == NULL)
            left = 0;
        else
            left = y->lt->height;
        y = y->pt;
        if(right > left){
            avlRotateLeft(tree, y->lt);
            avlRotateRight(tree, y);
        }
        else{
            avlRotateRight(tree, y);
        }
    }
    if(balance < -1){
        y = y->rt;
        if(y->rt == NULL)
            right = 0;
        else
            right = y->rt->height;
        
        if(y->lt == NULL)
            left = 0;
        else
            left = y->lt->height;
        y = y->pt;
        if(left > right){
            avlRotateRight(tree, y->rt);
            avlRotateLeft(tree, y);
        }
        else{
            avlRotateLeft(tree, y);
        }
    }
}

void insert(TTree* tree, void* elem, void* info) {
    //TODO: Cerinta 1
    /*
     * 1. Begin by implementing the normal BST insersion (no duplicates).
     * 2. Fix any unbalance caused by this insertion as the last operation.
     * 3. Now if this element is a duplicate all you have to do is to
     *    inserted in the approapiate list. Inseting the duplicate at the
     *    end of a list is easier and consistent!
     */
    TreeNode *node = createTreeNode(tree, elem, info);
    TreeNode *node_curr = node;
    if(isEmpty(tree)){
        tree->root = node;
        tree->size ++;
        return;
    }
    if(search(tree, tree->root, elem) != NULL){
        TreeNode *node_found = search(tree, tree->root, elem);
        node->next = node_found->end->next;
        node_found->end->next = node;
        if(node->next != NULL)
            node->next->prev = node;
        node->prev = node_found->end;
        node_found->end = node;
        
    }else{
        tree->size ++;
        TreeNode *copy = tree->root;
        TreeNode *parinte = copy;
        
        while(copy != NULL) {
            parinte = copy;
            if(tree->compare(copy->elem, elem) > 0) {
                copy = copy->lt;
            }
            else {
                copy = copy->rt;
            }
        }
        
        node->pt = parinte;
        
        if(tree->compare(parinte->elem, elem) > 0){
            parinte->lt = node;
            copy = parinte;
        }else{
            parinte->rt = node;
            copy = parinte;
        }
        
        while(copy != NULL) {
            copy->height = getHeight(tree, copy);
            copy = copy->pt;
        }
        
        copy = parinte;
        avlFixUp(tree, copy);
        copy = node_curr;
        while(copy != NULL) {
            copy->height = getHeight(tree, copy);
            copy = copy->pt;
        }
        copy = node_curr;
        
        copy->next = successor(tree, node_curr);
        if(copy->next != NULL)
            copy->next->prev = copy;
        copy->prev = predecessor(tree, node_curr);
        if(copy->prev != NULL){
            copy->prev = copy->prev->end;
            copy->prev->next = copy;
        }
        
    }
    
    
}

void delete(TTree* tree, void* elem){
    
    //TODO: Cerinta 1
    /*
     * 1. Begin by implementing the normal BST deletion (no duplicates).
     * 2. Fix any unbalance caused by this insertion as the last operation.
     * 3. Now what happends if the elem you are trining to delete has duplicates?
     *    Is it easier to delete the duplicate? Which one?
     *    What happends to the tree-list when you delete an elem with no duplicates?
     *
     */
    
    tree->size --;
    TreeNode *copy = search(tree, tree->root, elem);
    if(copy->end != copy){
        tree->size ++;
        TreeNode *node_end = copy->end;
        copy->end = node_end->prev;
        node_end->prev->next = node_end->next;
        if(node_end->next != NULL)
            node_end->next->prev = node_end->prev;
            return;
    }
    if(copy->rt == NULL && copy->lt == NULL){
        if(copy == tree->root){
            tree->root = NULL;
            return;
        }
        if(copy == copy->pt->lt)
            copy->pt->lt = NULL;
        else
            copy->pt->rt = NULL;
//        destroyTreeNode(tree, copy);
    }
    if(copy->next != NULL)
        copy->next->prev = copy->prev;
    if(copy->prev != NULL)
        copy->prev->next = copy->next;
    if(copy->rt == NULL && copy->lt != NULL){
        if(copy == copy->pt->lt){
            copy->pt->lt = copy->lt;
            copy->lt->pt = copy->pt;
        }
        else{
            copy->pt->rt = copy->lt;
            copy->lt->pt = copy->pt;
        }
    }
    if(copy->lt == NULL && copy->rt != NULL){
        if(copy == copy->pt->lt){
            copy->pt->lt = copy->rt;
            copy->rt->pt = copy->pt;
        }
        else{
            copy->pt->rt = copy->rt;
            copy->rt->pt = copy->pt;
        }
    }
    if(copy->rt != NULL && copy->lt != NULL){
        TreeNode *succ = minimum(tree, copy->rt);
        
        
        if(succ->rt == NULL && succ->lt == NULL){
            if(succ == succ->pt->lt)
                succ->pt->lt = NULL;
            else
                succ->pt->rt = NULL;
                    //        destroyTreeNode(tree, copy);
        }
        if(succ->rt == NULL && succ->lt != NULL){
            if(succ == succ->pt->lt){
                succ->pt->lt = succ->lt;
                succ->lt->pt = succ->pt;
            }
            else{
                succ->pt->rt = succ->lt;
                succ->lt->pt = succ->pt;
            }
        }
        if(succ->lt == NULL && succ->rt != NULL){
            if(succ == succ->pt->lt){
                succ->pt->lt = succ->rt;
                succ->rt->pt = succ->pt;
            }
            else{
                succ->pt->rt = succ->rt;
                succ->rt->pt = succ->pt;
            }
        }
        avlFixUp(tree, succ->pt);
        
        
        
        succ->pt = copy->pt;
        succ->lt = copy->lt;
        succ->rt = copy->rt;
        if(succ->lt != NULL)
            succ->lt->pt = succ;
        if(succ->rt != NULL)
            succ->rt->pt = succ;
        if(copy != tree->root){
            if(copy == succ->pt->rt)
                succ->pt->rt = succ;
            else
                succ->pt->lt = succ;
        }
        else
            tree->root = succ;
            
        TreeNode *node = succ;
        while(node != NULL) {
            node->height = getHeight(tree, node);
            node = node->pt;
        }
        avlFixUp(tree, succ);
        return;
    }
    TreeNode *node = copy->pt;
    while(node != NULL) {
        node->height = getHeight(tree, node);
        node = node->pt;
    }
    avlFixUp(tree, copy->pt);
    
    
}

void destroyTree(TTree* tree){
    //TODO: Cerinta 1
    /* What is the easiest way to get rid of all elements for this SPECIAL tree?
     */
}


#endif /* AVLTREE_H_ */


