#include <iostream>
#include <string>
#include <fstream>
#include "skip_list.cpp"
#include "KvStor.cpp"

using namespace std;
using namespace KvStor;
	
int main()
{
	kvstor* kv = new kvstor();
	if( !kv->create_kvstor())
	{
		return -1;
	}
	
	cout << endl << endl;
	cout << "-------KvStor Usage:-------";
	cout << endl << endl;
	cout << "command\t\tfunction\t\texample" << endl;
	cout << "set \t\tset key's value\t\teg.`set name Tom`" << endl;
	cout << "get \t\tget key's value\t\teg.`get name`" << endl;
	cout << "del \t\tdelete the key\t\teg.`del name`" << endl;
	cout << "exit \t\texit KvStor\t\teg.`exit`" << endl;
	cout << endl;

	keyType key;
	valueType value;
	valueType *val;
	string op, input;
	while (true)
	{
		cout << "KvStor> ";
	    std::getline(cin, input);
		if ( !input.size() )
			continue;

		vector<string> v;
		util::string s;
		s.split(input, " ", v);

		op = v[0];
		if (op == "set")
		{
			key = v[1];
			value = v[2];
			kv->set(key, value);
		}
		else if (op == "get")
		{
			key = v[1];
			val = kv->get(key);
			if (val)
			cout << *val << endl;
		}
		else if (op == "del")
		{
			key = v[1];
			kv->del(key);
		}
		else if (op == "exit")
		{
			kv->destory_kvstor();
			cout << "bye.." << endl;
			break;
		}
		else
		{
			cout << "command not found.." << endl;
		}
	}

	return  0;
}