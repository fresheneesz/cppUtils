/*	This file was created by Billy Tetrud. Use of this file is free as long as it is credited.
*/

#include <stdlib.h>

// copies to to from - in other words
// to = from
void copyString(char* to, char* from)
{	int n;
	for(n=0; to[n]!=0; n++)
	{
	}
	int m;
	for(m=0; from[m]!=0; m++, n++)
	{	to[n]=from[m];
	}
	to[n]=0;
}	

// returns true if isThis is greater than gtThis by ascii alphabetical order
bool stringGT(char* isThis, char* gtThis)
{	
}

// returns true if isThis is less than ltThis by ascii alphabetical order
bool stringLT(char* isThis, char* ltThis)
{	
}

// returns true if strings are the same
bool stringEQ(char* a, char* b)
{	int n;
	for(n=0; a[n]!=0; n++)
	{	if(a[n]!=b[n])
			return false;
	}
	if(a[n]!=b[n])
		return false;
	else
		return true;
}

// concatinates strings
//  atEndOfThis = (atEndOfThis putThis)
void stringCat(char* atEndOfThis, char* putThis)
{	int n;
	for(n=0; atEndOfThis[n]!=0; n++)
	{}	// now a[n]==0
	int m;
	for(m=0; putThis[m]!=0; m++)
	{	atEndOfThis[n]=putThis[m];
		n++;
	}
	atEndOfThis[n]=0;	
}

// string length
int Slen(char* a)
{	int n=0;
	for(; a[n]!=0; n++)
	{}
	return n;
}

// string to integer
int sti(char* a)
{	return atoi(a);
}

// integer to string
void its(int a, char* b)
{	int n=0;
	if(a<0)
	{	b[0]='-';
		n=1;
	}
	while(a>0)
	{	b[n] = a%10;
		n++;
		a/=10;
	}
}

// tests if every character in 'a' is 'in'
bool allEQ1(char* a, char in)
{	int n;
	for(n=0; a[n]!=0; n++)
	{	if(a[n]!=in)
			return false;
	}
	return true;
}

// tests if every character in 'a' is in 'ins'
bool allEQ2(char* a, char* ins)
{	int n;
	for(n=0; a[n]!=0; n++)
	{	bool found=false;		// whether a[n] was found in ins
		int m;
		for(m=0; ins[n]!=0; m++)
		{	if(a[n]==ins[m])
			{	found = true;
				break;
			}
		}
		if(!found)
			return false;
	}
	return true;
}

// tests if all characters in the subarray defined by 'index' is 'in'
bool allEQ3(char* a, int* index, char in)	// index is a -1 ending array of indecies that correspond to elemetns of a to test
{	int n;
	for(n=0; index[n]!=-1; n++)
	{	if(a[index[n]]!=in)
			return false;
	}
	return true;
}
