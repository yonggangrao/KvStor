#ifndef _KV_STOR_HPP_
#define _KV_STOR_HPP_
#include <iostream>
#include <fstream>
#include "util.cpp"
#include "skip_list.hpp"
#include <pthread.h>
#include <unistd.h> 

namespace KvStor
{
    class skipLisp;
    skip_list *gsl; /*global skip list, in memory*/
    std::fstream gfdat;  /*data file*/
    std::fstream gfidx;  /*data file*/
    int fdat, fidx;

    const int DATA_READ_SIZE = 1024;

    class kvstor
    {
    
    public:
        bool create_kvstor();
        bool destory_kvstor();
        bool sync_data_to_disk(valueType value, int64 &addr);
        bool set(keyType key, valueType value);
        valueType* get(keyType key);
        bool del(keyType key);
        bool write_index_to_disk(keyType key, int64 addr, int64 size);

    private:

    };
}

#endif //_KV_STOR_HPP_

