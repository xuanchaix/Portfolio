#include "myString.h"
#include "math.h"
#include <iostream>
using namespace std;

myString::myString(){
    capacity_ = 8;
    size_ = 0;
    str_ = (char*) malloc(8192);
    for(int i = 0; i < 8; i++){
        str_[i] = 0;
    }
}

myString::myString(const char* str){
    capacity_ = 8;
    size_ = 0;
    str_ = (char*) malloc(8192);
    int i = 0;
    while(str[i] != '\0'){
        str_[i] = str[i];
        ++size_;
        ++i;
    }
    myString::Resize();
}

myString::myString(const myString& str){
    capacity_ = 8;
    size_ = str.size_;
    str_ = (char*) malloc(8192);
    for(int i = 0; i < size_; i++){
        str_[i] = str.str_[i];
    }
    myString::Resize();
}

myString::~myString(){
    capacity_ = 0;
    size_ = 0;
    free(str_);
    str_ = nullptr;
}

void myString::append(char ch){
    str_[size_] = ch;
    ++size_;
    myString::Resize();
}
void myString::append(char *ch){
    int i = 0;
    while(ch[i] != '\0'){
        str_[size_] = ch[i];
        ++size_;
        ++i;
    }
    myString::Resize();
}
void myString::append(const myString& ch){
    for(int i = 0; i < ch.size_; i++){
        str_[size_] = ch.str_[i];
        ++size_;
    }
    myString::Resize();
}
	//扩容,每次容量增加一倍
void myString::Resize(){
    while(size_ >= capacity_){
        capacity_ = capacity_ * 2;
    }
}

int myString::size() const{
    return size_;
}

int myString::get_capacity() const{
    return capacity_;
}

//打印字符串
void myString::printStr(){
    for(int i = 0; i < size_; i++){
        printf("%c", str_[i]);
    }
    printf("\n");
}

//复制重载
myString& myString::operator=(myString& str){
    capacity_ = str.capacity_;
    size_ = str.size_;
    for(int i = 0; i < size_; i++){
        str_[i] = str.str_[i];
    }
    return *this;
}

//字符加
myString& myString::operator+=(char ch){
    str_[size_] = ch;
    ++size_;
    myString::Resize();
    return *this;
}

//字符串加
myString& myString::operator+=(const char* str){
    int i = 0;
    while(str[i] != '\0'){
        str_[size_] = str[i];
        ++size_;
        ++i;
    }
    myString::Resize();
    return *this;
}

//myString加
myString& myString::operator+=(const myString& str){
    for(int i = 0; i < str.size_; i++){
        str_[size_] = str.str_[i];
        ++size_;
    }
    myString::Resize();
    return *this;
}

bool myString::operator<(const myString& str){
    int size = min(size_, str.size_);
    for(int i = 0; i < size; i++){
        if(str_[i] < str.str_[i]){
            return true;
        }
        else if(str_[i] > str.str_[i]){
            return false;
        }
    }
    if(size_ < str.size_){
        return true;
    }
    return false;
}
bool myString::operator>(const myString& str){
    int size = min(size_, str.size_);
    for(int i = 0; i < size; i++){
        if(str_[i] > str.str_[i]){
            return true;
        }
        else if(str_[i] < str.str_[i]){
            return false;
        }
    }
    if(size_ > str.size_){
        return true;
    }
    return false;
}
bool myString::operator==(const myString& str){
    if(size_ == str.size_){
        for(int i = 0; i < size_; i++){
            if(str_[i] != str.str_[i]){
                return false;
            }
        }
        return true;
    }
    return false;
}
bool myString::operator>=(const myString& str){
    int size = min(size_, str.size_);
    for(int i = 0; i < size; i++){
        if(str_[i] > str.str_[i]){
            return true;
        }
        else if(str_[i] < str.str_[i]){
            return false;
        }
    }
    if(size_ >= str.size_){
        return true;
    }
    return false;
}
bool myString::operator<=(const myString& str){
    int size = min(size_, str.size_);
    for(int i = 0; i < size; i++){
        if(str_[i] < str.str_[i]){
            return true;
        }
        else if(str_[i] > str.str_[i]){
            return false;
        }
    }
    if(size_ <= str.size_){
        return true;
    }
    return false;
}
bool myString::operator!=(const myString& str){
    if(size_ != str.size_){
        return true;
    }
    int size = min(size_, str.size_);
    for(int i = 0; i < size; i++){
        if(str_[i] != str.str_[i]){
            return true;
        }
    }
    return false;
}

char& myString::operator[](const int index){
    char *p = &str_[index];
    return str_[index];
}
char myString::operator[](const int index)const{
    return str_[index];
}
/*
int main(){
    myString str0;
    str0.~myString();
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
cout << (str3 <= str4) << endl;
cout << (str3 > str4) << endl;
cout << (str3 >= str4) << endl;
myString str5("AAAAAAAAAA");
myString str6("AAAAAAAAAA");
cout << (str5 == str6) << endl;
cout << (str5 != str6) << endl;
cout << str1[1] << " " << str1[2] << endl;
}*/