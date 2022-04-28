
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void initList(linkedList* ll){
    ll->size = 0;
    ll->head = NULL;
    ll->tail = NULL;
}

void appendNode(linkedList* ll, int socket, char* handle, int handleLen){

    Node* new = (Node*)malloc(sizeof(Node));
    new->next = NULL;
    if(!ll->head){
        ll->head = new;
        ll->tail = new;
    } else if(ll->size == 1) {
        ll->head->next = new;
        ll->tail = new;
    }else {
        ll->tail->next = new;
        ll->tail = new;
    }
    new->socket = socket;
    memcpy(new->handle, handle, handleLen);
    new->handle[handleLen] = 0;
    ll->size++;
};

void removeNode(linkedList* ll, int index){
    int i;
    if(index+1 > ll->size) {
        perror("Remove Error: out of bounds\n");
        return;
    }
    if(index == 0){
        if(ll->size == 1){
            free(ll->head);
            ll->head = NULL;
            ll->tail = NULL;
        } else {
            Node* temp = ll->head;
            ll->head = ll->head->next;
            free(temp);
        }
    } else {
        Node* current = ll->head;
        for(i = 0; i<index - 1; i++){
            current = current->next;
        }
        Node* after = current->next->next;
        free(current->next);
        current->next = after;
        if(index+1 == ll->size){
            ll->tail = current;
        }
    }
    ll->size--;
}

void printList(linkedList* ll){
    printf("Linked List contents: [");
    Node* node = ll->head;
    while(node){
        printf("(%d, %s), ", node->socket, node->handle);
        node = node->next;
    }
    printf("]\n");
}

void setHandle(linkedList* ll, int index, char* handle, int handleLen){
    int i=0;
    if(index+1 > ll->size) {
        perror("Set Handle Error: out of bounds\n");
        return;
    }
    Node* node = ll->head;
    for(i = 0; i<index; i++){
        node = node->next;
    }
    memcpy(node->handle, handle, handleLen);
    node->handle[handleLen] = 0;
}

int getHandle(linkedList* ll, char* handle, int handleLen){
    char temp[101];
    int index = 0;
    memcpy(temp, handle,handleLen);
    temp[handleLen] = 0;
    Node* node = ll->head;
    while(node){
        if(!strcmp(node->handle, temp))
            return index;
        index++;
        node = node->next;
    }
    return -1;

}

char* getHandleAtIndex(linkedList *ll, int index){
    Node* node = ll->head;
    int i;
    for(i = 0; i<index; i++){
        node = node->next;
    }
    return node->handle;
}

int copyHandle(linkedList *ll, char *newlocation,int index){
    Node* node = ll->head;
    int i;
    for(i = 0; i<index; i++){
        node = node->next;
    }
    int length = (int)strlen(node->handle);
    memcpy(newlocation, node->handle, length);
    return length;

}

//returns index of socket or -1 if not found
int getSocket(linkedList* ll, int socket){
    int index = 0;
    Node* node = ll->head;
    while(node){
        if(socket == node->socket){
            return index;
        }
        index++;
        node = node->next;
    }
    return -1;
}

int getSocketAtIndex(linkedList *ll, int index){
    Node* node = ll->head;
    int i;
    for(i = 0; i<index; i++){
        node = node->next;
    }
    return node->socket;
}

//int main(int argc, char *argv[]) {
//    linkedList* ll = malloc(sizeof(linkedList));
//    ll->size = 0;
//    appendNode(ll, 27, "Test1", 5);
//    printList(ll);
//    appendNode(ll, 19, "Test2", 5);
//    printList(ll);
//    removeNode(ll, 0);
//    printList(ll);
//
//    appendNode(ll, 27, "Test1", 5);
//    printList(ll);
//    appendNode(ll, 14, "Test3", 5);
//    printList(ll);
//    removeNode(ll, 2);
//    printList(ll);
//    appendNode(ll, 118, "Test4", 5);
//    printList(ll);
//
//    removeNode(ll, 2);
//    printList(ll);
//
//    appendNode(ll, 118, "Test5", 5);
//    printList(ll);
//
//    setHandle(ll, 1, "T1", 2);
//    printList(ll);
//    printf("T1 at index %d\n", getHandle(ll, "Test4" , 5));
//
//    removeNode(ll,0);
//    removeNode(ll,0);
//    removeNode(ll,0);
//    appendNode(ll, 118, "Test5", 5);
//    printList(ll);
//    removeNode(ll,0);
//    printList(ll);
//    free(ll);
//}
