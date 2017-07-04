#ifndef _SKIP_LIST_HPP_
#define _SKIP_LIST_HPP_

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h> 
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>


namespace KvStor
{
    //pthread_rwlock_t rwlock;
    class kvstor;
    char INDEX_FILE[32] = "../data/index";
    char DATA_FILE[32] = "../data/data";
    typedef unsigned long long int64;
    typedef std::string keyType;
    typedef std::string valueType;

    #define MAX_L 16

typedef struct node  
{  
    keyType key;  
    int64 addr; /*address of value in data file*/
    bool inmem; /*value in memory or not*/
    valueType value;
    bool dirty;
    int64 size;
    struct node **next; 
} Node;
typedef struct skip_list  
{  
    unsigned level; 
    Node *head;  
} skip_list;


    class skipList
    {
    
    public:
        Node * new_node(unsigned level);
        int delete_node(Node *p);
        Node * create_node(unsigned level, keyType key, valueType val, int64 size);
        skip_list *create_sl();
        int randomLevel();
        bool insert(skip_list *sl, keyType key, valueType val, int64 size, int64 addr, bool sync);
        bool erase(skip_list *sl, keyType key);
        valueType *search(skip_list *sl, keyType key);
        void print_sl(skip_list *sl);
        void free_sl(skip_list *sl);
        bool sync_index_to_disk(skip_list *sl);

    private:

    };
}

#endif //_SKIP_LIST_HPP_

