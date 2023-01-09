### 1.队列（40分）

#### 题目描述

基于上次实验中实现的链表，实现一个FIFO(Fist In First Out)的队列。

我们将提供链表的实现List.h和List.cpp文件供参考，如果大家忘记了List.h的定义的话，我们将List.h放在本文末尾作为附录。

在Queue.h文件中，接口定义如下：

```c++
#ifndef QUEUE_H
#define QUEUE_H
#include "List.h"

//FIFO
typedef struct Queue {
    int size;
    List head;
    List tail;
};

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
```

调用示例：

```c++
Queue q;
init(q);
push(q, 1);
push(q, 2);
push(q, 3);
cout << front(q) << endl;
cout << back(q) << endl;
pop(q);
cout << front(q) << endl;
show(q);
```

输出结果：

```C++
1
3
2
2 3 
```

> 一个简单的示意图如下图所示，因此你需要在init函数中生成head和tail（他们此时应该都是指向相同的位置）。因为队列是先进先出，所以每次加入只需要加入队尾（tail 需要相应的改变），每次pop会将队头的结点移除。 ![img](http://114.212.189.236/online_judge/index.php/problems/queue.png)

#### 提交要求

- 只需提交一个源码文件：Queue.cpp (实现Queue.h中声明的接口) ,直接打包成zip格式的压缩包。**不要添加其他任何目录，压缩包中只包含这两个个文件!**
- List.h和List.cpp的在OJ上的实现与我们提供的参考文件List.h和List.cpp一致。
- 文件的编码格式只支持utf-8。
- 请严格按照给定的接口进行编码,否则无法调用测试用例。
- **提交的源码文件中不需要包含main函数,否则无法通过编译。**

## 附录

### 链表

#### 题目描述

实现链表，在List.h文件中，接口定义如下：

```c++
//注意：
// 我们保证传进来的任何指向Node的指针都是合法的，我们也只传这样的指针或者nullptr，但是不保证下标是合法的范围。


#ifndef LIST_H
#define LIST_H

//节点类型
typedef struct Node {
    int val;
    Node* next;
} Node;

//链表定义
typedef struct Node* List;

//初始化
//return: head pointer
List init();

// 插入节点，在head之后第i个位置插入值为val的节点，成功返回true，失败返回false。
bool insert(List& head, int i, int val);

//删除节点，删除head之后第i个位置的元素,成功返回true，失败返回false
//只有执行了delete操作的情况才能返回true。否则一律返回false。
bool erase(List& head, int i);

//打印所有节点的数据，每个节点(包括最后一个结点)后面加一个空格，最后加一个换行(空链表也需要输出)
void show(List& head);

//删除所有元素，也需要删除head pointer。注意内存泄漏。在clear调用之后，需要将head赋值为空指针。
void clear(List& head);

#endif // !LIST_H
```

调用示例：

```c++
List head = init();
cout << insert(head, 0, 8) << endl;
cout << insert(head, 0, 9) << endl;
cout << insert(head, 1, 10) << endl;
cout << insert(head, 10, 1) << endl;
cout << insert(head, 3, 11) << endl;
show(head);
cout << erase(head, 0) << endl;
cout << erase(head, 10) << endl;
show(head);
clear(head);
cout << insert(head, 0, 8) << endl;
cout << erase(head, 0) << endl;
```

输出结果：

```c++
1
1
1
0
1
9 10 8 11
1
0
10 8 11
0
0
```

------

一个简单的示意图如下图所示，因此你需要在init函数中生成head pointer。请仔细观察下图，并理解头指针和头结点的区别。链表的下标与数组一致，都是从0开始（也就是说**下图的val为5的结点的下标为0**）。**clear函数需要删除所有的元素包括head node**。请你一定保证代码的鲁棒性，**特别对于空指针的情况需要特别的注意**，我们使用的都是随机测试，如果用random函数你是无法通过任何测试的。另外，请不要使用STL，我们会在编译过程中禁止你使用STL，如果你使用的话将会产生编译错误。上述函数除了`init()`只会调用一次，其他所有的函数都可能会多次调用，请保证程序的鲁棒性。遇到error很多或者得分很低的情况，请不用慌张，仔细阅读程序接口注释的相关内容。 ![img](http://114.212.189.236/online_judge/index.php/problems/linklist.png)

#### 提交要求

- 提交一个源码文件List.cpp，实现给定的接口，直接打包成zip格式的压缩包。**不要添加其他任何目录,压缩包中只包含这一个文件!**
- 文件的编码格式只支持utf-8。
- 请严格按照给定的接口进行编码,否则无法调用测试用例。
- **提交的源码文件中不需要包含main函数,否则无法通过编译。**



#### 2. myString (60分)

本次上机要求实现一个myString类，myString.h如下：

```c++
#pragma once
#ifndef MYSTRING_H
#define MYSTRING_H
#include <string.h>
class myString {
private:
    //字符串数组
    char* str_;
    //已分配空间的大小
    int capacity_;
    //字符串大小
    int size_;
public:
    //默认构造函数
    //默认构造空间大小为8个字节
    myString();
    //注意一开始的capacity为8个字节，每次cap不足，扩容一倍
    myString(const char* str);
    //拷贝构造函数
    //注意一开始的capacity为8个字节，每次cap不足，扩容一倍
    myString(const myString& str);
    //析构函数
    ~myString();
    //复制重载
    myString& operator=(myString& str);
    //字符加
    myString& operator+=(char ch);
    //字符串加
    myString& operator+=(const char* str);
    //myString加
    myString& operator+=(const myString& str);
    //比较运算符重载
    bool operator<(const myString& str);
    bool operator>(const myString& str);
    bool operator==(const myString& str);
    bool operator>=(const myString& str);
    bool operator<=(const myString& str);
    bool operator!=(const myString& str);

    char& operator[](const int index);
    char operator[](const int index)const;
    void append(char ch);
    void append(char* ch);
    void append(const myString& ch);
    //扩容,每次容量增加一倍
    void Resize();
    int size() const;
    int get_capacity() const;
    //打印字符串
    void printStr();
};


#endif // MYSTRING_H
```

调用示例：

```c++
myString str1("hello world");
str1.printStr();
cout << str1.get_capacity() << " " << str1.size() << endl;

myString str2(str1);
str2.printStr();
cout << str2.get_capacity() << " " << str2.size() << endl;

str2 += '!';
str2.printStr();
cout << str2.get_capacity() << " " << str2.size() << endl;

str2 += " This is my implement of sring!";
str2.printStr();
cout << str2.get_capacity() << " " << str2.size() << endl;

str2 += str1;
str2.printStr();
cout << str2.get_capacity() << " " << str2.size() << endl;

str1 = str2;
str1.printStr();
cout << str1.get_capacity() << " " << str1.size() << endl;

myString str3("AAAAAAAAAA");
myString str4("BBBBBBBBBB");
cout << (str3 < str4) << endl;
```

输出结果：

```c++
hello world
16 11
hello world
16 11
hello world!
16 12
hello world! This is my implement of sring!
64 43
hello world! This is my implement of sring!hello world
64 54
hello world! This is my implement of sring!hello world
64 54
1
```

#### 提交要求

- 提交一个源码文件myString.cpp，实现给定的接口，直接打包成zip格式的压缩包。**不要添加其他任何目录,压缩包中只包含这一个文件!**
- **请注意myString的字符串中如果出现'\0'，则会将它看做普通字符**，这一点与`std::string`保持一致，因此C标准库里的`str*()`和`sprintf()`等不宜直接使用.
- 文件的编码格式只支持utf-8。
- 请严格按照给定的接口进行编码,否则无法调用测试用例。
- **提交的源码文件中不需要包含main函数,否则无法通过编译。**