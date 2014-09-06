#include "linklist.h"
using namespace std;

linklist::linklist()
{
     p=NULL;
}

void linklist::append( int pos, int val)
{
     node *q,*t;

   if( p == NULL )
   {
      p = new node;
      p->data = val;
	  p->position=pos;
      p->link = NULL;
   }
   else
   {
        q = p;
      while( q->link != NULL )
           q = q->link;

      t = new node;
      t->data = val;
	  t->position=pos;
      t->link = NULL;
      q->link = t;
   }
}

void linklist::add_as_first(int val, int pos)
{
     node *q;
   q = new node;
   q->data = val;
   q->position;
   q->link = p;
   p = q;
}

void linklist::addafter( int val,int pos,int pos2)
{
    node *q,*t;
   q=p;
   while(q->position!=pos2)
   {
        q = q->link;
   }
   t = new node;
   t->data = val;
   t->position = pos;
   t->link = q->link;
   q->link = t;
}

void linklist::del()
{
	p=NULL;
    /* node *q,*r;
   q = p;
   if( q->position == pos )
   {
        p = q->link;
      delete q;
      return;
   }*/

/*   r = q;
   while( q!=NULL )
   {
        if( q->position == pos )
      {
           r->link = q->link;
         delete q;
         return;
      }

      r = q;
      q = q->link;
   }
   //cout<<"\nElement "<<da<<" not Found.";*/
}

void linklist::display()
{
     node *q;
//  cout<<endl;

   for( q = p ; q != NULL ; q = q->link )
	   cout<<"\n"<<q->data<<" " <<q->position<<"\n" ;

}

int linklist::count()
{
     node *q;
   int c=0;
   for( q=p ; q != NULL ; q = q->link )
        c++;

   return c;
}
int* linklist::retValues()
{
	node *q;
	int cnt = count();

	int *val = new int[cnt*2];
	int i=0;
	for( q = p ; q != NULL ; q = q->link )
	{
		val[i]= q->position;
		i++;
		val[i]= q->data;
		i++;
	}
	return val;
}
int* linklist::retNeighbours(int x)//returns values in format x1,val1,x2,val2
{								
	node *q;
	q=p;
	int val[4] ;
	
	int cnt = count();
	while(cnt!=0)
	{
		if(x==q->position)
		{
			val[0]=q->position;
			val[1]=q->data;
			val[2]=q->link->position;
			val[3]=q->link->data;
			return val;
		}
		else if(x < (q->link)->position)  // coordinates are stored in ascending manner
		{
			val[0]=q->position;
			val[1]=q->data;
			val[2]=q->link->position;
			val[3]=q->link->data;
			return val;
		}
		q=q->link;
		cnt--;
	}
}

int linklist::seekIndex(int pos) //return index at which new element is to be inserted
{									//return -1 if element already present in list
	node *q;
	q=p;
	int cnt = count();
	while(cnt!=0)
	{
		if(pos==q->position)
			return -1;
		if(pos < (q->link)->position)
			return q->position;
		q=q->link;
		cnt--;
	}
}
void linklist::modify(int pos, int val)
{
	node *q;
	q=p;
	while(q->position != pos)
	{
		q=q->link;
	}
	q->data=val;
	
}
linklist::~linklist()
{
     node *q;
   if( p == NULL )
        return;

   while( p != NULL )
   {
        q = p->link;
      delete p;
      p = q;
   }
}
