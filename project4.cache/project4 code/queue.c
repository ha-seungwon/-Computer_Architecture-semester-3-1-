#include <stdio.h>
#include "set.h"

void init(int queue[], int data, int rear, int front)
{
    int tmp = (rear + 1) % MAX;
    if (tmp == front) {
        return;
    }
    else {
        rear = (rear + 1) % MAX;
        queue[rear] = data;
    }
}
int delete(int queue[], int rear, int front)
{
    if (front == rear){
        return;
    }
    else{
        front = (front + 1) % MAX;
        return queue[front];
    }
}
