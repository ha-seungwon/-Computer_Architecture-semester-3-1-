#include <stdio.h>
#include <stdlib.h>
#include "set.h"


void init(Node* head) {
    head = NULL;
}

void insert(Node* head,int data) {
    Node* ptr;
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;    // ������ �Ҵ� 
    newNode->next = NULL;    // next ������ �ʱ�ȭ 

    if (head == NULL) {    // empty
        head = newNode;
    }
    else {
        // not empty, ���� �տ� ��� �߰� 
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

    return -1;    // �ش� �����Ͱ� ����Ʈ�� �������� ���� 
}


int search_list(Node* head,int data) {
    Node* ptr;
    for (ptr = head; ptr; ptr = ptr->next) {
        if (ptr->data == data) {    // data �߰�  
            return 1;
        }
    }

    return -1; // ������ �� �߰� 
}
void printList(Node* head) {
    Node* ptr;
    for (ptr = head; ptr->next; ptr = ptr->next) {
        printf("%d->", ptr->data);
    }
    printf("%d\n", ptr->data);
}
