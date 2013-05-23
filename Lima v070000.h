#include "BillysTypes v071024.h"

template<int size>
class CharMatrix
{public:
	char a[size];
	int len;
	
	T& operator [](int n)
	{	if(n>=size || n<0)
		{	printf("\n\nERROR: Attempted out of bounds array access");
		}
		if(n>=len)
		{	len=n+1;
		}
		return a[n];
	}
};

void limaInit()
{	
	#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)

		#include <windows.h>
		#define mainH int WINAPI WinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) 
 		
		args
		 
	#else
	
		#define mainH int main(int argc, char* argv[])
		
		
		
	#endif
}
