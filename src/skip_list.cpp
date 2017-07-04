#ifndef _SKIP_LIST_CPP_
#define _SKIP_LIST_CPP_
#include "skip_list.hpp"
#include "KvStor.hpp"
#include <iostream>


//using namespace std;

namespace KvStor
{
    Node * skipList::new_node(unsigned level)
    {
      if (level == 0)
        return NULL;

      Node *p = new Node;
      p->next = new Node*[level];
      return p;
    }

    int skipList::delete_node(Node *p)
    {
      if (p == NULL)
        return 0;

      delete[] p->next;
      delete p;
      return 0;
    }

    Node * skipList::create_node(unsigned level, keyType key, valueType val, int64 size) 
    {
      Node *p = new_node(level);
      if (!p)
          return NULL;
      p->key = key;
      p->value = val;
      p->inmem = true;
      p->addr = -1;
      p->size = size;
      return p;
    }

    skip_list * skipList::create_sl()
    {
      skip_list *sl = new skip_list;
      sl->level = 1;

      Node *h = create_node(MAX_L-1, "", "", 0);
      if ( h == NULL )
      {
          return NULL;
      }
      sl->head = h;
      for (int i = 0; i < MAX_L; i++)
      {
          h->next[i] = NULL;
      }
      srand(time(0));
      return sl;
    }

    int skipList::randomLevel()
    {
      int level=1;
      while (rand()%2)
          level++;
      level=(MAX_L>level)? level:MAX_L;
      return level;
    }

    bool skipList::insert(skip_list *sl, keyType key, valueType val, int64 size, int64 addr, bool sync)
    {
      if(!sl)
        return false;

      Node *update[MAX_L];
      Node *q = NULL;

      Node *p = sl->head;

      int i = sl->level - 1;
      for( ; i >= 0; --i )
      {
          while( (q = p->next[i]) && q->key < key )
              p = q;
          update[i] = p;
      }

      if(q && q->key == key) /*key exists, set the value*/
      {
          q->value = val;
          q->size = size;
          q->inmem = true;

          return true;
      }
      else
      {
        unsigned level = randomLevel();
        if(level > sl->level)
        {
            for(i = sl->level; i < level; ++i)
            {
                update[i] = sl->head;
            }
            sl->level = level;
        }
        q = create_node(level, key, val, size);
        if(!q)
            return false;
        q->inmem = true;

        for(i = level - 1; i >= 0; --i)
        {
            q->next[i] = update[i]->next[i];
            update[i]->next[i] = q;
        }
        q->addr = addr;
      }
      /*sync to disk*/
      if (sync)
      {
          using namespace KvStor;

          kvstor* kv = new kvstor();
          bool ret = kv->sync_data_to_disk(val, q->addr);
          if(ret)
          {
              bool res = kv->write_index_to_disk(key, q->addr, size);
              if (!res)
              {
                  return false;
              }
          }
      }

      return true;
    }

    bool skipList::erase(skip_list *sl, keyType key)
    {
      Node *update[MAX_L];
      Node *q = NULL, *p = sl->head;
      int i = sl->level-1;
      for(; i >= 0; --i)
      {
          while( ( q = p->next[i] ) && q->key < key)
            p=q;
          update[i] = p;
      }

      if(!q || (q&&q->key != key)) /*key not exists*/
          return true;

      for(i = sl->level-1; i >= 0; --i)
      {
          if (update[i]->next[i] == q)
          {
              update[i]->next[i] = q->next[i];
              if (sl->head->next[i] == NULL) /*if the key is top level key*/
                  sl->level--;
          }
      }
      delete_node(q);
      return true;
    }

    valueType * skipList::search(skip_list *sl, keyType key)
    {
      Node *q, *p = sl->head;
      q = NULL;
      int i = sl->level - 1;
      for(; i >= 0; --i)
      {
          while( (q = p->next[i]) && q->key < key)
              p=q;

          if(q && key == q->key)
              return &(q->value);
      }

      return NULL;
    }

    void skipList::print_sl(skip_list *sl)
    {
      Node *q;
      int i=sl->level-1;
      using namespace std;
      for(; i>=0; --i)
      {
        q=sl->head->next[i];
        printf("level %d: ", i+1);

        while(q)
        {
            cout << q->key << " ";
            cout << q->size << " ";
            q = q->next[i];
            cout << "|" ;
        }
        cout << endl;
      }
    }
   
    void skipList::free_sl(skip_list *sl)
    {
      if(!sl)
        return;
  
      Node *q = sl->head;
      Node *next;
      while(q)
      {
        next = q->next[0];
        delete_node(q);
        q = next;
      }
      delete_node(sl->head);
      sl = NULL;
    }
    
    bool skipList::sync_index_to_disk(skip_list *sl)
    {
      if (sl->head == NULL)
      {
        return true;
      }
      Node * p = sl->head->next[0];

      std::ofstream fo(INDEX_FILE, std::ios::trunc);
      if(!fo)
      {
          fprintf(stderr, "Can not open file %s..", INDEX_FILE);
          return false;
      }
      while(p != NULL)
      {
        fo << p->key << " " << p->addr << " " << p->size << std::endl;
        p = p->next[0];
      }
      fo.close();
      return true;
    }


}
#endif //_SKIP_LIST_CPP_
