/*	This file was created by Billy Tetrud. Use of this file is free as long as it is credited.
*/


template<class T, int size>
class Arr
{public:
	T a[size];
	int len;
	
	Arr()
	{	len=0;
	}
	
	T& operator [](int n)
	{	//if(n>=size || n<0)
		//{	printf("\n\nERROR: Attempted out of bounds array access");
		//}
		if(n>=len)
		{	len=n+1;
		}
		return a[n];
	}
};


template<class T>
class SlinkedList		// singly linked list
{private:
	class node
	{public:
		T value;
		node* next;
	};
	
 public:
	node* head;
	node* tail;
	int length;
	
	SlinkedList()
	{	head = 0;
		tail = 0;
		length=0;
	}
	
	int len()
	{	return length;
	}
	
	// adds a node in spot n - pushing back nodes in greater positions
	void add(int n)
	{	node* newNode = new node;
		if(n>=length)	// adds even if n overflows the length
		{	if(head!=0)
				tail->next = newNode;
			else
				head = newNode;
			tail = newNode;
			tail->next = 0;
			length++;
		}
		else if(n==0)
		{	newNode->next = head;
			head = newNode;
			length++;
		}
		else
		{	node* temp;
			//find node n-1 from front
			temp = head;
			for(int k=0; k<n-1; k++)
			{	temp = temp->next;
			}
			
			newNode->next = temp->next;
			temp->next = newNode;
			length++;
		}
	}
	
	// adds x nodes into spot n
	void add(int n, int x)
	{	for(int k=0; k<x; k++)
		{	add(n);
		}
	}
	
	
	// deletes a node from spot n - pulling up nodes in greater positions
	void kill(int n)
	{	if(n==0)
		{	node* temp;
			temp = head->next;
			delete head;
			head = temp;
			length--;
		}
		else if(n>=length)	// deletes end even if n overflows the length
		{	delete tail->next;
			tail->next=0;
			length--;
		}
		else
		{	node* temp;
			node* temp2;
			//find node n-1 from front
			temp = head;
			for(int k=0; k<n-1; k++)
			{	temp = temp->next;
			}
			
			temp2 = temp->next;
			temp->next = temp->next->next;
			delete temp2;
			length--;
		}
	}
	
	void clear()
	{	while(length!=0)
			kill(0);
		/*
		node* temp=head;
		node* temp2;
		while(temp!=0)
		{	temp2=head->next;
			delete temp;
			temp = temp2;
		}
		length=0;
		head=0;
		tail=0;
		*/
	}
	
	// transfers the linked list from here to a
	// deletes the previous contents of a
	// clears this object
	void transfer(SlinkedList<T>& a)
	{	a.clear();
		a.head = head;
		a.tail = tail;
		a.length = length;
		head = 0;
		tail = 0;
		length = 0;
	}
	
	T& operator [](int n)
	{	if(n>=length-1)
		{	// nothing to access - so access the tail
			return tail->value;
		}
		
		node* temp;
		//find node n from front
		temp = head;
		for(int k=0; k<n; k++)
		{	temp = temp->next;
		}
		
		return temp->value;
	}
	
};


template<class T>
class minHeap
{private:
	class node
	{public:
		T value;
		int weight;
		node* l;
		node* r;
		
		node()
		{	l=0;
			r=0;
		}
		
	};
	
	int toggle;
	node* lastAdded;
	node* parentOfLastAdded;
	node* root;
	int length;
	
	// adds a node to a subtree
	void addTo(int weight, node** theNode)	//FUCKWOEIFJEWOIFGHEWGOUIHWEFIU
	{	node& a = **theNode;
		if(*theNode == 0)
		{	*theNode = new node;
			(*theNode)->weight = weight;
			lastAdded = *theNode;
			parentOfLastAdded = 0;
			length++;
		}
		else
		{	if(a.weight > weight)	// new node is smaller than parent
			{	///printf("%d > %d\n", a.weight, weight);
				node* temp = new node;
				temp->l = *theNode;
				*theNode = temp;
				temp->weight = weight;
				lastAdded = temp;
				parentOfLastAdded = 0;
				length++;
			}
			else	// new node is bigger than parent
			{	if(a.l==0)
				{	a.l = new node;
					a.l->weight = weight;
					lastAdded = a.l;
					parentOfLastAdded = &a;
					length++;
				}
				else if(a.r==0)
				{	a.r = new node;
					a.r->weight = weight;
					lastAdded = a.r;
					parentOfLastAdded = &a;
					length++;
				}
				else if(a.l->weight > weight && a.r->weight > weight)
				{	if(a.l->weight > a.r->weight)
					{	addTo(weight, &(a.l));
					}
					else
					{	addTo(weight, &(a.r));
					}
				}
				else if(a.l->weight > weight)
				{	node* temp = new node;
					temp->l = a.l;
					a.l = a.r;		// switch position of r and l for better balancing
					a.r = temp;
					temp->weight = weight;
					lastAdded = temp;
					parentOfLastAdded = a.r;
					length++;
				}
				else if(a.r->weight > weight)
				{	node* temp = new node;
					temp->l = a.r;
					a.r = a.l;		// switch position of r and l for better balancing
					a.l = temp;
					temp->weight = weight;
					lastAdded = temp;
					parentOfLastAdded = a.l;
					length++;
				}
				else	// bigger than both left and right
				{	if(toggle)
					{	addTo(weight, &(a.r));
					}
					else
					{	addTo(weight, &(a.l));
					}
					toggle = !toggle;
				}
			}
		}
	}
	
	// Kill slowly and painfully, by a slow fall to the bottom of the tree
	// hitting every branch on the way down.
	// parentRef is a pointer to the node* that points to a
	void killTopOf(node& a, node** parentRef)
	{	if((*parentRef)==0)
		{	// Theres nothing left to kill!
			// You've already pillaged and raped our whole heap!
		}
		else if(a.l==0 && a.r==0)
		{	delete &a;
			length--;
			*parentRef = 0;
		}
		else if(a.l==0)
		{	*parentRef = a.r;
			delete &a;
			length--;
		}
		else if(a.r==0)
		{	*parentRef = a.l;
			delete &a;
			length--;
		}
		else
		{	node* temp;
			node* rl;
			if(a.r->weight < a.l->weight)
			{	*parentRef = a.r;
			
				rl = a.r;
				
				temp = rl->r;	//
				rl->r = &a;		//
				a.r = temp;		//	Swap a and a.r
				temp = rl->l;	//
				rl->l = a.l;	//
				a.l = temp;		//
				
				killTopOf(a, &(rl->r));
			}
			else
			{	*parentRef = a.l;
			
				rl = a.l;
				
				temp = rl->r;	//
				rl->r = a.r;	//
				a.r = temp;		//Swap a and a.l
				temp = rl->l;	//
				rl->l = &a;		// 
				a.l = temp;		//

				killTopOf(a, &(rl->l));
			}
		}
	}
	
 public:
	
	minHeap()
	{	root=0;
		length=0;
		toggle=0;
		lastAdded=0;
		parentOfLastAdded=0;
	}
	
	int len()
	{	return length;
	}
	
	// add a node with weight to 
	void add(int weight)
	{	addTo(weight,&root);
	}
	
	// delete the top, and resort the heap
	void killTop()
	{	killTopOf(*root, &root);
	}
	
	// access the top
	T& top()
	{	return root->value;
	}
	
	T& accessLastAdded()
	{	return lastAdded->value;
	}
	
	T& ax()
	{	return accessLastAdded();
	}
	
	void printFE(node* a)
	{	printf("Value: %d\n", a->value);
		printf("Self: %d\n", a);
		printf("left: %d\n", a->l);
		printf("right: %d\n", a->r);
				//getchar();
		
		if(a->l != 0)
			printFE(a->l);
		if(a->r != 0)
			printFE(a->r);
	}
	
	void print()
	{	printf("length: %d\n", length);
		printf("lastAdded: %d\n", lastAdded);
		printf("parentOfLastAdded: %d\n", parentOfLastAdded);
		printf("Root: %d\n", root);
		
		//getchar();
		
		printFE(root);
	}
	
	
	void printTree()
	{	if(root==0)
		{	printf("\n");
			return;
		}
		SlinkedList<node*> FUCK, FUCKnext;
		FUCK.add(0);
		FUCK[0] = root;
		int done=0;
		
		while(!done)
		{	done=1;
			FUCKnext.add(0,FUCK.len()*2);
			
			for(int n=0; n<FUCK.len(); n++)
			{	if(FUCK[n] !=0)
				{	printf("%d  ", FUCK[n]->weight);
					FUCKnext[2*n] = FUCK[n]->l;
					FUCKnext[2*n+1] = FUCK[n]->r;
					done=0;
				//	printf("joke\n");
				}
				else
				{	printf("-  ");
					FUCKnext[2*n] = 0;
					FUCKnext[2*n+1] = 0;					
			//		printf("ewop\n");
				}
				//printf("WTF???\n");
			}
			FUCKnext.transfer(FUCK);
			printf("\n");
			//printf("WEOFJWEOIJE\n");
		}
	}
	
};

/*
// must not contain a class that contains pointers nor a reference of any distance to such a class 
template <class T, int size>
class Fifo
{public:
	Arr<T,size> Q;
	int front, back;//, length;
	
	Dfifo()	// creates empty queue
	{	front=0;
		back=0;
		//length=0;
	}
	
	T& operator [](int n)	// access middle of queue
	{	if(n+front>=Q.size())
		{	return Q[n - (Q.size()-front)];
		}
		else
		{	return Q[front+n];
		}
	}
	
	// add to back - because C++ is stupid, you may not push a literal type (integer literal, float literal, etc)
	void push(T& a)	
	{	(*this)[length()] =  a;

		if(back+1==Q.size())
		{	back=0;
		}
		else
			back+=1;
	}
	
	// pull from front - returns true if can't pop
	bool pop(T& a)
	{	if(front==back && Q.length==0)
			return true;
		else
		{	a = Q[front];
			if(front+1==Q.size())
			{	front=0;
			}
			else
				front+=1;
				
			if(front==back)
				Q.length=0;	// indicates that queue is empty, not full
			return false;
		}
	}
	
	
	int len()
	{	if(back>front)
		{	return back-front;
		}
		else if(back==front)
		{	if(Q.length==0)
				return 0;
			else
				return Q.length;
		}
		else
		{	return back + Q.length-front;
		}
	}
};
*/


