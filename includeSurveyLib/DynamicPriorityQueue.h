#ifndef dyn_pq
#define dyn_pq

//#pragma warning (disable:4786)

#if _MSC_VER >= 1000
#pragma once
#endif

#include <vector>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <stddef.h>
#include <string>
//using namespace std;

#include "PtrGreater.h"

#include "assert.h"


//#include "TTransformWrapper.h"





template <class T> 

/*!\ingroup ShortestPath
	@{*/
class DynamicPriorityQueue
{
public:
	typedef typename std::vector<T>::size_type Size;
	typedef typename std::vector<T>::difference_type index;
	
	// constructor
	DynamicPriorityQueue(std::vector<T>& trans)
		: Indices(trans.size()), c(trans.size()), first(&*trans.begin()), csize(trans.size())
	{
		//index i;

		unsigned int i;

		//stores pointers and generate heap
		for (i=0; i<csize; i++)
			c[i] = &trans[i];
		make_heap(c.begin(), c.end(), comp);

		// constructs index array
		for (i=0; i<csize; i++)
		{
			Indices[c[i] - first] = i;
		}
	}


	void getIndices()
	{
		index i;

		for(i=0; i<csize; i++)
			std::cout << Indices[i] << std::endl;
	}

	index getIndice(int i)
	{
		return Indices[c[i] - first];
	}


	// changes a value at position at
	void changeKeyAt(index at, T trans)
	{
		index idx = Indices[at];
		// value still present in the queue?
		assert(idx < (signed)csize);

		if (*c[idx] != trans) {
			if (comp(&trans, c[idx]))
			{
				*c[idx] = trans;
				goDown(idx);
			}
			else
			{
				*c[idx] = trans;
				goUp(idx);
			}
		}
	}


	// index of the smallest element (=highest priority)
	index topIndex() const
	{
		return (c.front() - first); 
	}

	// value of the smallest element (=highest priority)
	const T& topKey() const
	{ 
		return *c.front(); 
	}



	

	//removes smallest element of the heap
	void pop()
	{
		//overwrite a pointer at the top with the address of the last element
		c[0] = c[--csize];

		//enter the new address 0 at the position belonging to this element in the auxiliary array
		Indices[c[0]-first] = 0;

		//getIndices();

		//let the element at the top sink to the correct position corresponding to its weight
		goDown(0);
	}




	bool empty() const
	{
		return csize == 0;
	}


	Size size() const
	{
		return csize; 
	}



private:
	std::vector<index> Indices;
	std::vector<T*> c;
	T* first;
	PtrGreater<T*> comp;
	Size csize;


	void goUp(index idx)
	{
		index Predecessor = (idx-1)/2;
		T* temp = c[idx];

		while (Predecessor != idx && comp(c[Predecessor], temp))
		{
			c[idx] = c[Predecessor];
			Indices[c[idx]-first] = idx;
			idx = Predecessor;
			Predecessor = (idx-1)/2;
		}

		c[idx] = temp;		
		Indices[c[idx]-first] = idx;
	}




	void goDown(index idx)
	{
		index Successor = (idx+1)*2-1;

		if ((signed)csize != 0 && Successor < (signed)csize-1 && comp(c[Successor], c[Successor+1]))
			++Successor;

		T* temp = c[idx];

		while(Successor < (signed)csize && comp(temp, c[Successor]))
		{
			c[idx] = c[Successor];
			Indices[c[idx]-first] = idx;
			idx = Successor;
			Successor = (idx+1)*2-1;

			if(Successor < (signed)csize-1 && comp(c[Successor], c[Successor+1]))
				++Successor;
		}

		c[idx] = temp;
		Indices[c[idx]-first] = idx;
	}

};

/*@}*/


#endif
