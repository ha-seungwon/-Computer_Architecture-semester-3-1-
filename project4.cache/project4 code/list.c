#include <stdio.h>
#include <stdlib.h>
#include "set.h"


void init(Node* head) {
    head = NULL;
}

void insert(Node* head,int data) {
    Node* ptr;
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;    // 데이터 할당 
    newNode->next = NULL;    // next 포인터 초기화 

    if (head == NULL) {    // empty
        head = newNode;
    }
    else {
        // not empty, 가장 앞에 노드 추가 
            newNode->next = head;
            head = newNode;
            return; 
    }

}
int deleteNode(Node* head,int data) {
    Node* cur;
    cur =  head;
    int result = 0;
    if (head == NULL) {    // empty list 
        return -1;
    }

    while (cur->next != NULL) {
        cur = cur->next;
    }
    result = cur->data;
    if (result != NULL) {
        return result;
    }

    return -1;    // 해당 데이터가 리스트에 존재하지 않음 
}


int search_list(Node* head,int data) {
    Node* ptr;
    for (ptr = head; ptr; ptr = ptr->next) {
        if (ptr->data == data) {    // data 발견  
            return 1;
        }
    }

    return -1; // 데이터 미 발견 
}
void printList(Node* head) {
    Node* ptr;
    for (ptr = head; ptr->next; ptr = ptr->next) {
        printf("%d->", ptr->data);
    }
    printf("%d\n", ptr->data);
}
