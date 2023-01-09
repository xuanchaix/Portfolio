#ifndef QUEUE_H
#define QUEUE_H
#include "List.h"

//FIFO
typedef struct Queue {
	int size;
	List head;
	List tail;
}Queue;

//初始化
void init(Queue& q);

//入队，在队列末尾插入值为val的节点
void push(Queue& q, int val);

//出队列
//请注意队列中只剩下最后一个元素的情况
bool pop(Queue& q);

//判断是否为空
bool empty(Queue& q);

//打印所有节点的数据，每个节点(包括最后一个结点)后面加一个空格，最后加一个换行(空链表也需要输出)
void show(Queue& q);

//返回队首元素值，如果队列为空，返回-1
int front(Queue& q);

//返回队尾元素值，如果队列为空，返回-1
int back(Queue& q);

// 相当于一个析构函数。清空队列，也需要清空head。
void clear(Queue&q);
#endif // !QUEUE_H