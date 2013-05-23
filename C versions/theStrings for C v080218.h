/*	This file was created by Billy Tetrud. Use of this file is free as long as it is credited.
*/

#include <stdlib.h>
#include "definebool.h"

// copies to to from - in other words
// to = from
void Scopy(char* to, char* from)
{	int n=0;
	for(; from[n]!=0; n++)
	{	to[n]=from[n];
	}
	to[n]=0;
}	

void Scopy2(char* to, char* from, char* from2)
{	int n=0;
	for(; from[n]!=0; n++)
	{	to[n]=from[n];
	}

	Scopy(&(to[n]), from2);
}	

void Scopy3(char* to, char* from, char* from2, char* from3)
{	int n=0;
	for(; from[n]!=0; n++)
	{	to[n]=from[n];
	}

	Scopy2(&(to[n]), from2, from3);
}	

/*
// returns true if isThis is greater than gtThis by ascii alphabetical order
bool stringGT(char* isThis, char* gtThis)
{	
}

// returns true if isThis is less than ltThis by ascii alphabetical order
bool stringLT(char* isThis, char* ltThis)
{	
}
*/

// returns true if strings are the same
bool SEQ(char* a, char* b)
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

// returns true if strings are the same - up to a point
// point is the length of the strings that is tested
bool SEQsub(char* a, char* b, int point)
{	int n;
	for(n=0; a[n]!=0 && n+1<point; n++)
	{	if(a[n]!=b[n])
			return false;
	}
	if(a[n]!=b[n])
		return false;
	else
		return true;
}

// returns true if b is the substring of a starting from n
bool Scontain(char* a, char* b, int n)
{	for(; a[n]!=0; n++)
	{	if(a[n]!=b[n])
			return false;
	}
		return true;
}

// concatinates strings
//  atEndOfThis = (atEndOfThis putThis)
void Scat(char* atEndOfThis, char* putThis)
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

// concatinates strings
//  atEndOfThis = (atEndOfThis putThis)
void Scat2(char* atEndOfThis, char* putThis, char* putThis2)
{	int n;
	for(n=0; atEndOfThis[n]!=0; n++)
	{}	// now a[n]==0
	int m;
	for(m=0; putThis[m]!=0; m++)
	{	atEndOfThis[n]=putThis[m];
		n++;
	}
	
	Scopy(&(atEndOfThis[n]), putThis2);
}

// concatinates strings
//  atEndOfThis = (atEndOfThis putThis)
void Scat3(char* atEndOfThis, char* putThis, char* putThis2, char* putThis3)
{	int n;
	for(n=0; atEndOfThis[n]!=0; n++)
	{}	// now a[n]==0
	int m;
	for(m=0; putThis[m]!=0; m++)
	{	atEndOfThis[n]=putThis[m];
		n++;
	}

	Scopy2(&(atEndOfThis[n]), putThis2, putThis3);
}

// concatinates strings
//  atEndOfThis = (atEndOfThis putThis)
// returns a reference to the atEndOfThis string
char* ScatChar(char* atEndOfThis, char putThis)
{	int n;
	for(n=0; atEndOfThis[n]!=0; n++)
	{}	// now a[n]==0
	atEndOfThis[n]=putThis;
	atEndOfThis[n+1]=0;	
	return atEndOfThis;
}

// string length
int Slen(char* a)
{	int n=0;
	for(; a[n]!=0; n++)
	{}
	return n;
}

// string to integer
int Sti(char* a)
{	return atoi(a);
}

// integer to string
char* itS(int a, char* b)
{	if(a==0)
	{	b[0]='0';
		b[1]=0;
		return b;
	}
	
	int n=0;
	if(a<0)
	{	b[0]='-';
		a=-a;
		n++;
	}
	for(; a>0; n++)
	{	b[n] = '0' + a%10;
		a/=10;
	}
	b[n]=0;
	n-=1;
	
	// reverse b
	int m;
	for(m=0; n>m; m++, n--)
	{	char temp;
		temp = b[m];
		b[m] = b[n];
		b[n] = temp;
	}
	return b;
}

// tests if every character in 'a' is 'in'
bool allEQ(char* a, char in)
{	int n;
	for(n=0; a[n]!=0; n++)
	{	if(a[n]!=in)
			return false;
	}
	return true;
}

// tests if all characters in the subarray defined by 'index' is 'in'
bool allEQIndex(char* a, int* index, char in)	// index is a -1 ending array of indecies that correspond to elemetns of a to test
{	int n;
	for(n=0; index[n]!=-1; n++)
	{	if(a[index[n]]!=in)
			return false;
	}
	return true;
}

// tests if every character in 'a' is in 'ins'
bool allIn(char* a, char* ins)
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

// returns the index of the character after the search string
// returns -1 if it doesn't find the searchForThis string
int find(char* searchForThis, char* inThis, int startingHere)
{	int searchState=0;
	int contentLength = Slen(inThis);
	int searchStrLength = Slen(searchForThis);
	
	int n;
	for(n=startingHere; n<contentLength; n++)
	{	if(inThis[n] == searchForThis[searchState])
		{	searchState++;
			if(searchState == searchStrLength)
			{	return n+1;
			}
		}
		else 
		{	searchState = 0;
		}
	}
	
	return -1; // didn't find it
}

// finds the searchForThis string and stores the characters 
// between (and including) the startingHere value and the character before the searchForThis string starts
// Should work even if "inThis" is the same string as "storeHere"
// Probably will not work if "searchForThis" is the same as "storeHere"
// returns the index of the character after the search string
// returns -1 if it doesn't find the searchForThis string
int findAndStore(char* searchForThis, char* inThis, int startingHere, char* storeHere)
{	
	int searchState=0;
	int contentLength = Slen(inThis);
	int searchStrLength = Slen(searchForThis);
	
	int n;
	for(n=startingHere; n<contentLength; n++)
	{	if(n-startingHere > searchStrLength)
		{	storeHere[n-searchStrLength-startingHere-1] = inThis[n-searchStrLength-1];
		}
		
		if(inThis[n] == searchForThis[searchState])
		{	searchState++;
			if(searchState == searchStrLength)
			{	storeHere[n-searchStrLength-startingHere] = inThis[n-searchStrLength];
				storeHere[n-searchStrLength-startingHere+1] = 0;
				return n+1;
			}
		}
		else 
		{	searchState = 0;
		}
	}
	
	for(n = contentLength-searchStrLength-1; n<contentLength; n++)
	{	storeHere[n-startingHere] = inThis[n];
	}
	
	storeHere[n-startingHere] = 0;
	return -1; // didn't find it
}
