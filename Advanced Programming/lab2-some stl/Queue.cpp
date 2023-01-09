#include "Queue.h"
#include <iostream>
using namespace std;

void init(Queue& q){
    List q_head = new Node;
    q_head->next = nullptr;
    q.head = q_head;
    q.tail = q_head;
    q.size = 0;
}

//入队，在队列末尾插入值为val的节点
void push(Queue& q, int val){
    List temp = new Node;
    temp->next = nullptr;
    temp->val = val;
    q.tail->next = temp;
    q.tail = temp;
    ++q.size;
}

//出队列
//请注意队列中只剩下最后一个元素的情况
bool pop(Queue& q){
    if(q.head->next == q.tail){
        List temp = q.tail;
        q.tail = q.head;
        q.head->next = nullptr;
        delete temp;
        --q.size;
        return true;
    }
    else if(q.head->next != nullptr){
        List temp = q.head->next;
        q.head->next = q.head->next->next;
        delete temp;
        --q.size;
        return true;
    }
    else{
        return false;
    }
}

//判断是否为空
bool empty(Queue& q){
    if(q.tail == q.head){
        return true;
    }
    return false;
}

//打印所有节点的数据，每个节点(包括最后一个结点)后面加一个空格，最后加一个换行(空链表也需要输出)
void show(Queue& q){
    List current = q.head->next;
    while(current != nullptr){
        printf("%d ", current->val);
        current = current->next;
    }
    printf("\n");
    delete current;
}

//返回队首元素值，如果队列为空，返回-1
int front(Queue& q){
    if(empty(q)){
        return -1;
    }
    else{
        return q.head->next->val;
    }
}

//返回队尾元素值，如果队列为空，返回-1
int back(Queue& q){
    if(empty(q)){
        return -1;
    }
    else{
        return q.tail->val;
    }
}

// 相当于一个析构函数。清空队列，也需要清空head。
void clear(Queue&q){
    List current = q.head->next;
    while(current != nullptr){
        List temp = current;
        current = current->next;
        delete temp;
    }
    q.head = nullptr;
    q.tail = nullptr;
    q.size = 0;
}
/*
int main(){
    Queue q;
init(q);
cout << empty(q) << endl;
push(q, 1);
pop(q);
pop(q);

cout << empty(q) << endl;
cout << front(q) << endl;
push(q, 2);
push(q, 3);
cout << front(q) << endl;
cout << back(q) << endl;
pop(q);
cout << front(q) << endl;
show(q);
}*/