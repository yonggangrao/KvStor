#ifndef _KV_STOR_CPP_
#define _KV_STOR_CPP_
#include "skip_list.hpp"
#include "KvStor.hpp"
#include <stdlib.h>
#include<sstream>

using namespace std;

namespace KvStor
{
    bool kvstor::set(keyType key, valueType value)
    {
		skipList *SL = new skipList();
		bool ret = SL->insert(gsl, key, value, value.size(), -1, true);

		return ret;
    }

    valueType* kvstor::get(keyType key)
    {
		skipList *SL = new skipList();
		valueType * val = SL->search(gsl, key);

		return val;
    }

    bool kvstor::del(keyType key)
    {
		skipList *SL = new skipList();
		bool ret = SL->erase(gsl, key);
		
		return ret;
    }

    bool kvstor::create_kvstor()
    {
    	fdat = open("DATA_FILE", O_CREAT | O_APPEND | O_RDWR, 0666 );
    	if (!fdat)
    	{
			cout << "can not open data file, starting up failed.." << endl;
			return false;
    	}
    	fidx = open("INDEX_FILE", O_CREAT | O_APPEND | O_RDWR, 0666 );
    	if (!fidx)
    	{
			cout << "can not open index file, starting up failed.." << endl;
			return false;
    	}
 
    	ifstream fi ;
		fi.open(INDEX_FILE);
		if(!fi)
		{
			cout << "can not open index file, starting up failed.." << endl;
			return false;
		}
		ifstream fidat ;
		fidat.open(DATA_FILE);
		if(!fidat)
		{
			cout << "can not open data file, starting up failed.." << endl;
			return false;
		}
		string line;

		skipList *SL = new skipList();
		gsl = SL->create_sl();

	    while (!fi.eof()) 
	    {
	    	std::getline(fi, line);
	    	if (!line.size())
	    		continue;

	    	std::vector<string> v;
			util::string s;
			s.split(line, " ", v);
	    	string key = v[0];
	    	int64 addr = atoi(v[1].c_str());
	    	int64 size = atoi(v[2].c_str());

	    	string value = "";
	    	char buf[DATA_READ_SIZE];

			fidat.seekg(0, std::ios::end);
			std::streampos end = fidat.tellg();

	    	if (addr >= end) /*illage addr*/
	    		continue;

	    	fidat.seekg(addr);

			fidat.read(buf, size);

			string t(buf);
			value = t;
			value = value.substr(0, size);
			SL->insert(gsl, key, value, size, addr, false);
	    }

	    fi.close();
    	return true;
    }

    bool kvstor::destory_kvstor()
    {
		skipList *SL = new skipList();

    	SL->sync_index_to_disk(gsl);

    	return true;
    }

    bool kvstor::sync_data_to_disk(valueType value, int64 &addr)
    {
		gfdat.open( DATA_FILE, ios::in|ios::out|ios::app);
		if(!gfdat)
		{
			cout << "can not open data file, starting up failed.." << endl;
			return false;
		}
		gfdat.seekp(0, ios::end);
    	addr = gfdat.tellp();

    	gfdat << value;
    	
		gfdat.close();
      	return true;
    }
    bool kvstor::write_index_to_disk(keyType key, int64 addr, int64 size)
    {
		gfidx.open( INDEX_FILE, ios::in|ios::out|ios::app);
		if(!gfidx)
		{
			cout << "can not open data file, starting up failed.." << endl;
			return false;
		}
      	stringstream ss;
     	ss << " " << addr << " " << size;
		string line = key + ss.str();

    	gfidx << line << endl;
    	
		gfidx.close();

      	return true;
    }
}
#endif //_KV_STOR_CPP_
