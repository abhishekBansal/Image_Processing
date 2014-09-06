//using namespace std;
#ifndef _LINKLIST_H_
#define _LINKLIST_H_

#include <iostream>

using namespace std;
class linklist
{
	private:
	  struct node
         {
            int data;
			int position;
			node *link;
         }*p;

	  	
    public:
		struct values
		{
			int pos;
			float col;
		};

		 linklist();
         void append( int pos ,int val);
         void add_as_first( int val, int pos);
         void addafter( int val,int pos,int pos2);
         void del();
         void display();
		 int* retValues();
		 int* retNeighbours(int pos);
		 int seekIndex(int pos);
		 void modify(int pos, int val);
         int count();
         ~linklist();
};
#endif
