#include <SDL/SDL.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <gl/glaux.h>

#include <al/al.h>
#include <al/alc.h>
#include <al/alu.h>
#include <al/alut.h>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
float RpD = .0174532925; // Radians per Degree
float sqrtof2 = 1.41422;	 // rounded up slightly

#include "audioHandling v080228.h"
#include "imageHandling080000.h"

float EventFPS, GraphFPS;
int originalWidth; int originalHeight;	// Dimensions of the window.
int width, height;

#define mainH int WINAPI WinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )

ALboolean LoadALData(char WAVfileName[], ALfloat SourcePos[], ALfloat SourceVel[], ALuint* Buffer, ALuint* Source);

void EXIT(int a)
{   SDL_Quit();
	//alDeleteBuffers(1, Buffer);
    //alDeleteSources(1, Source);
    alutExit();
    exit(a);
}

void setupVideo();

void setup_ALL( int setupWidth, int setupHeight, int setupGraphFPS, int setupEventFPS )
{	originalWidth = setupWidth;
	originalHeight = setupHeight;
	width = setupWidth;
	height = setupHeight;
	GraphFPS = setupGraphFPS;
	EventFPS = setupEventFPS;

	//FreeConsole(); //closes the console windows
	
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )    // First, initialize SDL's video subsystem.
    {   MessageBox(NULL,"Video initialization failed. Press ok to quit.","Initialization Error",MB_OK | MB_ICONINFORMATION);
        EXIT(1);      // Failed, exit.
    }
    
    setupVideo();

	ilInit();
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);

	alutInit(0, 0);
    if(alGetError()!=AL_NO_ERROR)
	{ MessageBox(NULL,"OpenAL initialization failed. Um.. get a new computer.","You fail.",MB_OK | MB_ICONINFORMATION);
	}
	
	alDopplerFactor(0);					// no doppler effect
	//alDopplerVelocity(velocity);		// sets speed of sound

}

class _8BitArray
{public:
	unsigned char array;
	
	bool get(unsigned int n)
	{	if(n>7)
		{	printf("ERROR: incorrect usage of 8BitArray - only has elements 0 through 7.\n");
			EXIT(0);
		}
		return array & (1<<n);
	}
	int get(unsigned int index, int width)	// gets the integer value of a range of bits
	{	int value=0;
		if(index+1 < width)
		{	printf("Error in _8BitArray::set(int,int,int): index+1 must be smaller than width\n");
			return -1;
		}
		for(int n=0; n<width; n++)
		{	value += get(index-n)*(1<<(width-1-n));
		}
		return value;
	}
	void set(int index, bool value)
	{	char temp;
		if(value!=0 && value!=1)
			value=1;
		temp = array & ~(1<<index);
		array = temp | (value<<index);
	}
	void set(unsigned int index, int width, int value) // sets a range of bits to an integer value 
	{	if(index+1 < width)
		{	printf("Error in _8BitArray::set(int,int,int): index+1 must be smaller than width\n");
			return;
		}
		for(int n=0; n<width; n++)
		{	set(index-width+1+n,value%2);
			value/=2;
		}
		if(value!=0)
			printf("OVERFLOW ERROR in _8BitArray::set(int,int,int): integer %d did not fit in width of %d bits.\n",value, width);
	}
	void print()
	{	printf("%d%d%d%d%d%d%d%d", get(7),get(6),get(5),get(4),get(3),get(2),get(1),get(0));
	}
};

template <class T>
class dynamicArray
{	T* array;
	int size, nextPos;
  public:	
	dynamicArray()
	{	array = new T[10];
		size=10;
		nextPos=0;
	}
	void clear()
	{	delete array;
		array=0;
		size=0;
		nextPos=0;
	}
	
	T get(int index)
	{	return array[index];
	}
	T add(T value)
	{	if(nextPos==size)
		{	if(array==0)
			{	array = new T[10];
				size=10;
				nextPos=0;
			}
			else
			{	T* switchTemp= new T[size*2];
				size*=2;
				for(int n=0; n<size; n++)
				{	switchTemp[n]=array[n];
				}
				delete array;
				array = switchTemp;
			}
		}
		array[nextPos] = value;
		nextPos++;
	}
	int length()
	{	return nextPos;
	}

};

class imageWrapper
{public:
	GLuint texture;
	float HoverW;
	char TheName[50];
	
	imageWrapper(char* name)
	{	HoverW = LoadGLTextures(name, &texture);
		int n;
		for(n=0; name[n]!=0;n++)
		{	TheName[n]=name[n];
		}	
		TheName[n]=0;
	}
	
};

class listWrapper
{public:
	GLuint list;
	GLuint texture;
	
	listWrapper(imageWrapper* a, float H, float W, float D)
	{	texture = a->texture;
		
		list=glGenLists(1);
		glNewList(list, GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-W/2, H/2, 0);
				glTexCoord2f(1.0f, 0.0f); glVertex3f( W/2, H/2, 0); 
				glTexCoord2f(1.0f, 1.0f); glVertex3f( W/2,-H/2, 0);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-W/2,-H/2, 0);
			glEnd();
		glEndList();
	}
	
	void resetList(GLuint a, float H, float W, float D)
	{	glDeleteLists(list,1);
		texture=a;
		list=glGenLists(1);
		glNewList(list, GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-W/2, H/2, 0);
				glTexCoord2f(1.0f, 0.0f); glVertex3f( W/2, H/2, 0); 
				glTexCoord2f(1.0f, 1.0f); glVertex3f( W/2,-H/2, 0);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-W/2,-H/2, 0);
			glEnd();
		glEndList();
	}
	
};

void setupVideo()
{	const SDL_VideoInfo* info = NULL;
	int flags;		// Flags to pass into SDL_SetVideoMode.
	int bpp;		// Color depth in bits of our window.
	
	info = SDL_GetVideoInfo( );    // get some video information.
    if( !info )         // This should probably never happen. 
    {   MessageBox(NULL,"Video query failed. Press ok to quit.","Query Error",MB_OK | MB_ICONINFORMATION);
        EXIT(1);
    }
	
    bpp = info->vfmt->BitsPerPixel; // get the bpp (bits per pixel)

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );		// at least 5 bits of red..
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 ); 	// greee ..
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );  	// and blue ..
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );	// ALPHA !!! (still doesn't make it work right)
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );	// 16 bit depth buffer
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );  // double buffering 0/1 off/on

	// flags for video mode
    flags = SDL_OPENGL|SDL_RESIZABLE;//|SDL_NOFRAME ;//| SDL_FULLSCREEN;
    if( SDL_SetVideoMode( originalWidth, originalHeight, bpp, flags ) == 0 )     // Set the video mode
	{   MessageBox(NULL,"Video mode set failed for a variety of possible reasons, including DISPLAY not being set, \
						the specified resolution not being available, etc. Press ok to exit."
						,"Initialization Error",MB_OK | MB_ICONINFORMATION);
        EXIT(1);
    }
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
   	glOrtho(-100,100, 100-200*(float)originalHeight/originalWidth,100, 1,300);
   	glMatrixMode(GL_MODELVIEW);
   	glLoadIdentity();
   	
   	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL, 0.0f);
   	
   	glEnable(GL_TEXTURE_2D);		// Enable Texture Mapping 
    glShadeModel( GL_SMOOTH );  	// Our shading model--Gouraud (smooth).
	glClearDepth(300.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do

    glViewport( 0, 0, originalWidth, originalHeight );		// Setup our viewport.
    gluPerspective( 60.0, (float)originalWidth/originalHeight, 1.0, 1024.0 );    // EXERCISE: Replace this with a call to glFrustum.    

}		

class objRECT
{ private:
	char itsName[100];
	
	// All objects in group
	GLfloat groupRot, groupPos[3], groupVel[3];
	GLfloat groupSizeMult;
	ALfloat groupGain;
	char groupFlipH;
	
	// Atributes gained from parent
	GLfloat parentRot, parentPos[3], parentVel[3];
	GLfloat parentSizeMult;
	ALfloat parentGain;
	char parentFlipH;
	
	// Child objects
	objRECT** Q;		// objects relative to current object
	int nextOpenQPosition;
	int sizeofQ;

	// private main object qualities
	listWrapper* theDisplayList;
	imageWrapper* pic; // null pointer means don't draw - obviously
	int groupRenderPic, groupRenderSound;
	GLfloat H, W;		// height, width
	char flipH;

	friend class mainGroup;
		
  public:
	// main object qualities
	GLfloat rot, D;		//rotation around z, depth (depth is relative to groupPos[3]
	GLfloat sizeMult;			
	ALuint source, buffer;
	ALfloat gain;
	int renderPic, renderSound;
	
	// Child objects
	int maxLevel;		// maximum sublevel draws below the original draw call

	GLfloat getAbsoluteGroupRot()
	{	return parentRot + groupRot;
	}
	GLfloat getAbsoluteGroupPosX()
	{	return 	parentPos[0] + (groupPos[0]*cos(parentRot*RpD) - groupPos[1]*sin(parentRot*RpD))*parentSizeMult*parentFlipH;
	}
	GLfloat getAbsoluteGroupPosY()
	{	return 	parentPos[1] + (groupPos[1]*cos(parentRot*RpD) + groupPos[0]*sin(parentRot*RpD))*parentSizeMult;
	}
	GLfloat getAbsoluteGroupPosZ()
	{	return groupPos[2] + parentPos[2];
	}
	GLfloat getAbsoluteGroupSizeMult()
	{	return groupSizeMult*parentSizeMult;
	}
	ALfloat getAbsoluteGroupGain()
	{	return groupGain*parentGain;
	}
	int getAbsoluteGroupFlipH()
	{	return parentFlipH*groupFlipH;
	}
	
	
	GLfloat getGroupRot()
	{	return groupRot;
	}
	GLfloat getGroupPosX()
	{	return 	groupPos[0] ;
	}
	GLfloat getGroupPosY()
	{	return 	groupPos[1];
	}
	GLfloat getGroupPosZ()
	{	return groupPos[2];
	}
	GLfloat getGroupSizeMult()
	{	return groupSizeMult;
	}
	ALfloat getGroupGain()
	{	return groupGain;
	}
	int getGroupRenderPic()
	{	return groupRenderPic;
	}
	int getGroupFlipH()
	{	return groupFlipH;
	}
	
	GLfloat getH()
	{	return H;
	}
	GLfloat getW()
	{	return W;
	}
	
	bool testQEquals(int index, objRECT* objectToTest)
	{	if(Q[index]==objectToTest)
		{	return true;
		}
		else return false;
	}
	
	void propogateGroupChanges(int level)
	{	if(level > maxLevel*5)
		{	printf("WARNING: parent attribute-change propogation has propogated 5 times the maximum draw level.\n");
			printf("The program will stop propogation at this point and continue normally. However, since all \n");
			printf("the changes did not propogate, some objects may not appear in the correct places, or at all.\n");
			printf("The objRECT type of object should not be drawn by more than one parent.");
		//	return;
		}
		//else
		if(Q==0)
			return;
		//else	
		for(int n=0; n!=nextOpenQPosition; n++)
		{	Q[n]->parentRot = getAbsoluteGroupRot();
			Q[n]->parentPos[0] = getAbsoluteGroupPosX();
			Q[n]->parentPos[1] = getAbsoluteGroupPosY();
			Q[n]->parentPos[2] = getAbsoluteGroupPosZ();
			Q[n]->parentVel[0] = groupVel[0] + parentVel[0];
			Q[n]->parentVel[1] = groupVel[1] + parentVel[1];
			Q[n]->parentVel[2] = groupVel[2] + parentVel[2];
			Q[n]->parentSizeMult = getAbsoluteGroupSizeMult();
			Q[n]->parentGain = getAbsoluteGroupGain();
			Q[n]->parentFlipH = getAbsoluteGroupFlipH();
			
			Q[n]->propogateGroupChanges(level+1);
		}
	}
	
	void setDim(GLfloat Height, GLfloat HoverW, GLfloat Depth) //set dimensions
	{	H=Height; W=Height/HoverW; D=Depth; 
		
		//reset display list
		if(pic && theDisplayList)
		{	theDisplayList->resetList(pic->texture, H, W, D);
		}	
	}
	
	void setGroupAll(GLfloat rotation, GLfloat x, GLfloat y, GLfloat z, GLfloat size, ALfloat gain)		// Set ALL
	{	groupRot = rotation;
		if(groupRot > 360)
			groupRot-=360;
		groupPos[0]=x; groupPos[1]=y; groupPos[2]=z;
		groupSizeMult = size;
		groupGain = gain;
		propogateGroupChanges(0);
	}
	
	void setGroupRot(GLfloat rotation)								// rotation
	{	groupRot = rotation;
		if(groupRot > 360)
			groupRot-=360;
		propogateGroupChanges(0);
	}
	void setGroupPos(GLfloat x, GLfloat y, GLfloat z)		// Set position
	{	groupPos[0]=x; groupPos[1]=y; groupPos[2]=z;
		propogateGroupChanges(0);
	}
	void setGroupPosX(GLfloat x)								// X position
	{	groupPos[0]=x;
		propogateGroupChanges(0);
	}
	void setGroupPosY(GLfloat y)								// Y position
	{	groupPos[1]=y;
		propogateGroupChanges(0);
	}
	void setGroupPosZ(GLfloat z)								// Z position
	{	groupPos[2]=z;
		propogateGroupChanges(0);
	}
	void setGroupSizeMult(GLfloat a)							// Size
	{	groupSizeMult = a;
		propogateGroupChanges(0);
	}
	void setGroupGain(ALfloat a)								// Gain
	{	groupGain = a;
		propogateGroupChanges(0);
	}
	void setGroupFlipH(char a)
	{	if(a!=1 && a!=-1)
			printf("a must be 1 or -1. No change made.\n");
		else
		{	groupFlipH = a;
			propogateGroupChanges(0);
		}
	}	
	
	void setGroupRenderPic(bool a)
	{	groupRenderPic=a;
		//propogateGroupChanges(0);
	}
	void setGroupRenderSound(bool a)
	{	groupRenderSound=a;
		//propogateGroupChanges(0);
	}
	
	objRECT(char* texName, GLfloat heightSize, char* soundName, GLfloat Tx, GLfloat Ty, GLfloat Tz, GLfloat rotation, char* objname)
	{	int j;
		for(j=0; objname[j]!=0; j++)
		{	itsName[j]=objname[j];
		}
		itsName[j]=0;
		
		maxLevel=10;
		Q=0; nextOpenQPosition=0; sizeofQ=0;
		
		rot=0; groupRot=rotation;
		gain=1; groupGain=1;
		sizeMult=1; groupSizeMult=1; 
		pic=0; renderPic=0; groupRenderPic=1;
		renderSound=0; groupRenderSound=1;
		theDisplayList=0;
		flipH=1;	groupFlipH=1; 
		
		groupPos[0]=Tx; groupPos[1]=Ty; groupPos[2]=Tz;
		groupVel[0]=0; groupVel[1]=0; groupVel[2]=0;
		
		parentRot=0;
		parentPos[0]=0; parentPos[1]=0; parentPos[2]=0;
		parentVel[0]=0; parentVel[1]=0; parentVel[2]=0;
		parentSizeMult=1;
		parentGain=1;
		parentFlipH=1;
		
		if(texName[0]!=0)
		{	pic = new imageWrapper(texName);
			renderPic=1;
			if(pic->HoverW!=0)
			{	H=heightSize; W=heightSize/pic->HoverW; D=0;
			}
			else
			{	H=heightSize; W=heightSize; D=0;
			}
			theDisplayList = new listWrapper(pic, H, W, D);
		}
		else
		{	setDim(heightSize,1,0);
		}	
		if(soundName[0]!=0)
		{	LoadALData(soundName, groupPos, groupVel, &buffer, &source);
			renderSound=1;
		}
	}
	
	objRECT(char* texName, GLfloat heightSize, GLfloat widthSize, char* soundName, GLfloat Tx, GLfloat Ty, GLfloat Tz, GLfloat rotation, char* objname)
	{	int j;
		for(j=0; objname[j]!=0; j++)
		{	itsName[j]=objname[j];
		}
		itsName[j]=0;
		
		maxLevel=10;
		Q=0; nextOpenQPosition=0; sizeofQ=0;
		
		rot=0; groupRot=rotation;
		gain=1; groupGain=1;
		sizeMult=1; groupSizeMult=1; 
		pic=0; renderPic=0; groupRenderPic=1;
		renderSound=0; groupRenderSound=1;
		theDisplayList=0;
		flipH=1;	groupFlipH=1; 
		
		groupPos[0]=Tx; groupPos[1]=Ty; groupPos[2]=Tz;
		groupVel[0]=0; groupVel[1]=0; groupVel[2]=0;
		
		parentRot=0;
		parentPos[0]=0; parentPos[1]=0; parentPos[2]=0;
		parentVel[0]=0; parentVel[1]=0; parentVel[2]=0;
		parentSizeMult=1;
		parentGain=1;
		parentFlipH=1;
		
		setDim(heightSize,heightSize/widthSize,0);
		if(texName[0]!=0)
		{	pic = new imageWrapper(texName);
			renderPic=1;
			theDisplayList = new listWrapper(pic, H, W, D);
		}
		if(soundName[0]!=0)
		{	LoadALData(soundName, groupPos, groupVel, &buffer, &source);
			renderSound=1;
		}
	}
	
	
	objRECT(imageWrapper* picture, GLfloat heightSize, char* soundName, GLfloat Tx, GLfloat Ty, GLfloat Tz, GLfloat rotation, char* objname)
	{	int j;
		for(j=0; objname[j]!=0; j++)
		{	itsName[j]=objname[j];
		}
		itsName[j]=0;
		
		maxLevel=10;
		Q=0; nextOpenQPosition=0; sizeofQ=0;
		
		rot=0; groupRot=rotation;
		gain=1; groupGain=1;
		sizeMult=1; groupSizeMult=1; 
		pic=picture; renderPic=0; groupRenderPic=1;
		renderSound=0; groupRenderSound=1;
		theDisplayList=0;
		flipH=1;	groupFlipH=1; 
		
		groupPos[0]=Tx; groupPos[1]=Ty; groupPos[2]=Tz;
		groupVel[0]=0; groupVel[1]=0; groupVel[2]=0;
		
		parentRot=0;
		parentPos[0]=0; parentPos[1]=0; parentPos[2]=0;
		parentVel[0]=0; parentVel[1]=0; parentVel[2]=0;
		parentSizeMult=1;
		parentGain=1;
		parentFlipH=1;
		
		if(pic!=0)
		{	renderPic=1;
			if(pic->HoverW!=0)
			{	H=heightSize; W=heightSize/pic->HoverW; D=0;
			}
			else
			{	H=heightSize; W=heightSize; D=0;
			}
			theDisplayList = new listWrapper(pic, H, W, D);
		}
		else
		{	setDim(heightSize,1,0);
		}
		if(soundName[0]!=0)
		{	LoadALData(soundName, groupPos, groupVel, &buffer, &source);
			renderSound=1;
		}
	}
	
	
	objRECT(imageWrapper* picture, GLfloat heightSize, GLfloat widthSize, char* soundName, GLfloat Tx, GLfloat Ty, GLfloat Tz, GLfloat rotation, char* objname)
	{	int j;
		for(j=0; objname[j]!=0; j++)
		{	itsName[j]=objname[j];
		}
		itsName[j]=0;
		
		maxLevel=10;
		Q=0; nextOpenQPosition=0; sizeofQ=0;
		
		rot=0; groupRot=rotation;
		gain=1; groupGain=1;
		sizeMult=1; groupSizeMult=1; 
		pic=picture; renderPic=0; groupRenderPic=1;
		renderSound=0; groupRenderSound=1;
		theDisplayList=0;
		flipH=1;	groupFlipH=1; 
		
		groupPos[0]=Tx; groupPos[1]=Ty; groupPos[2]=Tz;
		groupVel[0]=0; groupVel[1]=0; groupVel[2]=0;
		
		parentRot=0;
		parentPos[0]=0; parentPos[1]=0; parentPos[2]=0;
		parentVel[0]=0; parentVel[1]=0; parentVel[2]=0;
		parentSizeMult=1;
		parentGain=1;
		parentFlipH=1;
		
		setDim(heightSize,heightSize/widthSize,0);
		if(pic!=0)
		{	renderPic=1;
			theDisplayList = new listWrapper(pic, H, W, D);
		}
		if(soundName[0]!=0)
		{	LoadALData(soundName, groupPos, groupVel, &buffer, &source);
			renderSound=1;
		}
		
	}	
	
	
	objRECT(listWrapper* list, char* soundName, GLfloat Tx, GLfloat Ty, GLfloat Tz, GLfloat rotation, char* objname)
	{	int j;
		for(j=0; objname[j]!=0; j++)
		{	itsName[j]=objname[j];
		}
		itsName[j]=0;
		
		maxLevel=10;
		Q=0; nextOpenQPosition=0; sizeofQ=0;
		
		rot=0; groupRot=rotation;
		gain=1; groupGain=1;
		sizeMult=1; groupSizeMult=1; 
		pic=0; renderPic=1; groupRenderPic=1;
		renderSound=0; groupRenderSound=1;
		flipH=1;	groupFlipH=1; 
		
		groupPos[0]=Tx; groupPos[1]=Ty; groupPos[2]=Tz;
		groupVel[0]=0; groupVel[1]=0; groupVel[2]=0;
		
		parentRot=0;
		parentPos[0]=0; parentPos[1]=0; parentPos[2]=0;
		parentVel[0]=0; parentVel[1]=0; parentVel[2]=0;
		parentSizeMult=1;
		parentGain=1;
		parentFlipH=1;
		
		//renderPic=1; //previously set
		theDisplayList = list;

		if(soundName[0]!=0)
		{	LoadALData(soundName, groupPos, groupVel, &buffer, &source);
			renderSound=1;
		}
	}
	
	
	imageWrapper* getTexture()
	{	return pic;
	}
	
	listWrapper* getList()
	{	return theDisplayList;
	}
	
	//WARNING: possible memory leak cause
	void setTexture(imageWrapper* a)
	{	pic = a;
		if(a!=0)
		{	setDim(H,pic->HoverW,0);
			renderPic=1;
		}
		else
			renderPic=0;
		//printf("Warning: memory leaks may be caused by setTexture(imageWrapper*) if the texture has the last remaining reference to the imageWrappper\n");
		
		//reset display list
		theDisplayList->resetList(pic->texture, W, H, D);
	}
	
	
	void setQSize(int QSize)
	{	if(nextOpenQPosition>=QSize)
		{	printf("ERROR: Program attempted to resize an object queue to a smaller size than\n");
			printf("       the amount of objects it was holding. Q will not be resized.\n");
			return;
		}
		if(Q==0)
		{	Q = new objRECT*[QSize];
			sizeofQ=QSize;
			return;
		}
		//else
		objRECT** temp = new objRECT*[QSize];
		int en;
		for(en=0; en!=nextOpenQPosition; en++)
		{	temp[en] = Q[en];
		}
		delete Q;
		Q = temp;
		sizeofQ=QSize;
	}
	
	
	void addToQ(objRECT* object)
	{	if(object==0)
		{	printf("Can't add a null object\n");
			return;
		}
		//printf("crash here: %d\n",object->nextOpenQPosition);
		if(nextOpenQPosition>=sizeofQ)
		{	setQSize((int)(1.5*(sizeofQ+2)));
		}
		Q[nextOpenQPosition] = object;
		nextOpenQPosition++;

		// Propogate object's attributes to child
		object->parentRot = getAbsoluteGroupRot();
		object->parentPos[0] = getAbsoluteGroupPosX();
		object->parentPos[1] = getAbsoluteGroupPosY();
		object->parentPos[2] = getAbsoluteGroupPosZ();
		object->parentVel[0] = groupVel[0] + parentVel[0];
		object->parentVel[1] = groupVel[1] + parentVel[1];
		object->parentVel[2] = groupVel[2] + parentVel[2];
		object->parentSizeMult = getAbsoluteGroupSizeMult();
		object->parentGain = getAbsoluteGroupGain();	
		
		object->propogateGroupChanges(0);
	}
	
	objRECT* removeFromQ(int index)
	{	objRECT* returnThis = Q[index];
		for(int n=index; n+1!=nextOpenQPosition; n++)
		{	Q[n]=Q[n+1];
		}
		nextOpenQPosition--;
		
		return returnThis;
	}
	
	int getNOP()
	{	return nextOpenQPosition;
	}
	
	void draw(unsigned int level)
	{	static ALfloat totalPos[3];
		if(level>maxLevel)
			return;
		//if(itsName[0]!='1' && itsName[0]!='2')
		//	printf("In Draw() for %s",itsName);
		//if() // if the object is completely out of view
		{	if(renderPic && groupRenderPic)
			{	//if(itsName[0]!='1' && itsName[0]!='2')	
				//	printf("    and Actually Drawing");
				
				totalPos[0]= getAbsoluteGroupPosX();
				totalPos[1]= getAbsoluteGroupPosY();
				totalPos[2]= getAbsoluteGroupPosZ();
				
				glLoadIdentity();
				glTranslatef(getAbsoluteGroupPosX(),getAbsoluteGroupPosY(),getAbsoluteGroupPosZ()-100+D);
				glRotatef(getAbsoluteGroupRot() + rot,0.0f,0.0f,1.0f);
				glScalef(getAbsoluteGroupSizeMult()*sizeMult*getAbsoluteGroupFlipH()*flipH, getAbsoluteGroupSizeMult()*sizeMult, 1);
				glCallList(theDisplayList->list);
				/*
				glBindTexture(GL_TEXTURE_2D, pic->texture);
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(-W/2, H/2, 0);
					glTexCoord2f(1.0f, 0.0f); glVertex3f( W/2, H/2, 0); 
					glTexCoord2f(1.0f, 1.0f); glVertex3f( W/2,-H/2, 0);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(-W/2,-H/2, 0);
				glEnd();
				//*/
			}

			//if(itsName[0]!='1' && itsName[0]!='2')
			//	printf("\n");
			if(Q!=0 && groupRenderPic)
			{	int temp;
				for(int n=0; n!=nextOpenQPosition; n++)
				{	Q[n]->draw(level+1);
				}
			}
			
		}
	}
	
	void updateSound(unsigned int level)
	{	static ALfloat totalPos[3];
	
		if(renderSound && groupRenderSound)
		{	totalPos[0]= getAbsoluteGroupPosX();
			totalPos[1]= getAbsoluteGroupPosY();
			totalPos[2]= getAbsoluteGroupPosZ();
			
			alSourcefv(source, AL_POSITION, totalPos);
			//alSourcefv(source, AL_VELOCITY, vel);
			alSourcef(source, AL_GAIN, getAbsoluteGroupGain()*gain);
		}
		
		if(Q!=0 && groupRenderSound)
		{	int temp;
			for(int n=0; n!=nextOpenQPosition; n++)
			{	Q[n]->updateSound(level+1);
			}
		}
		
	}
	
	bool PointCollide(float x, float y)
	{	float sM = getAbsoluteGroupSizeMult()*sizeMult;
		if(getAbsoluteGroupRot()+rot!=0)
		{	
						// this attempt is buggy
			/*if(H < W)			// attempt at avoiding sin and cos
			{	float lenTemp = W*sqrtof2;
				if(x-getAbsoluteGroupPosX() >  lenTemp || x-getAbsoluteGroupPosX() < -lenTemp || 
				   y-getAbsoluteGroupPosY() >  lenTemp || y-getAbsoluteGroupPosY() < -lenTemp )
				{	return false; }
				
				lenTemp = H/sqrtof2;
				if(x-getAbsoluteGroupPosX() <=  lenTemp && x-getAbsoluteGroupPosX() >= -lenTemp && 
				   y-getAbsoluteGroupPosY() <=  lenTemp && y-getAbsoluteGroupPosY() >= -lenTemp )
				{	printf("All day\n");
					return true; }
			}
			else				// attempt at avoiding sin and cos
			{	int lenTemp = H*sqrtof2;
				if(x-getAbsoluteGroupPosX() >  lenTemp || x-getAbsoluteGroupPosX() < -lenTemp ||
				   y-getAbsoluteGroupPosY() >  lenTemp || y-getAbsoluteGroupPosY() < -lenTemp )
				{	return false; }
				
				lenTemp = W/sqrtof2;
				if(x-getAbsoluteGroupPosX() <=  lenTemp && x-getAbsoluteGroupPosX() >= -lenTemp && 
				   y-getAbsoluteGroupPosY() <=  lenTemp && y-getAbsoluteGroupPosY() >= -lenTemp )
				{	printf("moosecap\n");
					return true; }
			}*/
			
			float cosTemp = cos((getAbsoluteGroupRot()+rot)*RpD);
			float sinTemp = sin((getAbsoluteGroupRot()+rot)*RpD);
			float slopeWallsT = sinTemp / cosTemp; //((H/2*cosTemp+W/2*sinTemp)-(H/2*cosTemp-W/2*sinTemp)) / ((W/2*cosTemp+H/2*sinTemp)+(W/2*cosTemp-H/2*sinTemp));
			float slopeWallsS = -cosTemp / sinTemp; //((H/2*cosTemp+W/2*sinTemp)+(H/2*cosTemp-W/2*sinTemp)) / ((W/2*cosTemp-H/2*sinTemp)-(W/2*cosTemp+H/2*sinTemp) );
			float bT = getAbsoluteGroupPosY() +  (H/2*cosTemp+W/2*sinTemp)*sM - slopeWallsT*(getAbsoluteGroupPosX() + (W/2*cosTemp-H/2*sinTemp)*sM );
			float bB = getAbsoluteGroupPosY() -  (H/2*cosTemp+W/2*sinTemp)*sM - slopeWallsT*(getAbsoluteGroupPosX() - (W/2*cosTemp-H/2*sinTemp)*sM );
			float bL = getAbsoluteGroupPosY() +  (H/2*cosTemp-W/2*sinTemp)*sM - slopeWallsS*(getAbsoluteGroupPosX() - (W/2*cosTemp+H/2*sinTemp)*sM );
			float bR = getAbsoluteGroupPosY() +  (H/2*cosTemp+W/2*sinTemp)*sM - slopeWallsS*(getAbsoluteGroupPosX() + (W/2*cosTemp-H/2*sinTemp)*sM );
			
			if( (slopeWallsT*x + bB <= y && slopeWallsT*x + bT >= y ||
				 slopeWallsT*x + bB >= y && slopeWallsT*x + bT <= y ) &&
				((y - bL)/slopeWallsS <= x && (y - bR)/slopeWallsS >= x ||
				 (y - bL)/slopeWallsS >= x && (y - bR)/slopeWallsS <= x ) )
			{	return true;
			}
			else return false;
		}
		else
		{	if( getAbsoluteGroupPosX() - W/2*sM <= x && 
				getAbsoluteGroupPosX() + W/2*sM >= x &&
				getAbsoluteGroupPosY() - H/2*sM <= y && 
				getAbsoluteGroupPosY() + H/2*sM >= y )
			{	return true;
			}
			else return false; 
		}
	}
	
	
	objRECT* firstPointCollide(float x, float y, int maxLevel) // will only check objects on the top maxLevel levels of Qs
	{	objRECT* temp;	
		for(int n=0; n<nextOpenQPosition; n++)
		{	if(Q[n]->PointCollide(x,y))
			{	return Q[n];
			}
		}
		return 0;
	}
	
	bool RectCollide(float BLx, float BLy, float TRx, float TRy)
	{	if(H < W)
		{//	W	 
		}
		else
		{
		}
		
		// test if it collided with rectangles total horizontal range
		 	// if so, test if it collided with rectangles total verticle range
		 	
	}
	
	/*bool ObjectCollide(objRECT* object)
	{	bool result;
		
		// first check if circles rectangles are inscribed in collide
		if((W^2+H^2)^.5+(object.W^2+object.H^2)^.5 < getAbsoluteGroupPosX()^2 + 
	
		result = PointCollide(object->currentPosition[0] - object->currentDimensions[0],
							  object->currentPosition[1] - object->currentDimensions[1]);
		result = result || 
				 PointCollide(object->currentPosition[0] + object->currentDimensions[0],
					 		  object->currentPosition[1] - object->currentDimensions[1]);
		result = result || 
				 PointCollide(object->currentPosition[0] - object->currentDimensions[0],
					 		  object->currentPosition[1] + object->currentDimensions[1]);
		result = result || 
				 PointCollide(object->currentPosition[0] + object->currentDimensions[0],
					 		  object->currentPosition[1] + object->currentDimensions[1]);
					 		  
		result = result ||
				 object->PointCollide(currentPosition[0] - currentDimensions[0],
				 					  currentPosition[1] - object->currentDimensions[1]);
		result = result ||
				 object->PointCollide(currentPosition[0] + currentDimensions[0],
				 					  currentPosition[1] - object->currentDimensions[1]);
		result = result ||
				 object->PointCollide(currentPosition[0] - currentDimensions[0],
				 					  currentPosition[1] + object->currentDimensions[1]);
		result = result ||
				 object->PointCollide(currentPosition[0] + currentDimensions[0],
				 					  currentPosition[1] + object->currentDimensions[1]);
				 					  
		return result;
	}*/
	
};


// One instance, called Main
class mainGroup
{ private:
	// All objects in group
	GLfloat groupRot, groupPos[3], groupVel[3];
	GLfloat groupSizeMult;
	ALfloat groupGain;
	int groupFlipH;
	
	// Child objects
	objRECT** Q;		// objects relative to current object
	int nextOpenQPosition;
	int sizeofQ;
	
  public:
		
	// Child objects	
	int maxLevel;		// maximum sublevel draws below the original draw call

	GLfloat getGroupRot()
	{	return groupRot;
	}
	GLfloat getGroupPosX()
	{	return groupPos[0];
	}
	GLfloat getGroupPosY()
	{	return groupPos[1];
	}
	GLfloat getGroupPosZ()
	{	return groupPos[2];
	}
	GLfloat getGroupSizeMult()
	{	return groupSizeMult;
	}
	ALfloat getGroupGain()
	{	return groupGain;
	}
	int getGroupFlipH()
	{	return groupFlipH;
	}
	
	void propogateGroupChanges()
	{	GLfloat temp;
		if(Q==0)
			return;
			
		for(int n=0; n!=nextOpenQPosition; n++)
		{	Q[n]->parentRot = groupRot;
			//temp = sqrt(groupPos[0]*groupPos[0]+groupPos[1]*groupPos[1]);
			Q[n]->parentPos[0] = (groupPos[0]*cos(groupRot*RpD) - groupPos[1]*sin(groupRot*RpD))*groupSizeMult;
			Q[n]->parentPos[1] = (groupPos[1]*cos(groupRot*RpD) + groupPos[0]*sin(groupRot*RpD))*groupSizeMult;
			Q[n]->parentPos[2] = groupPos[2];
			Q[n]->parentVel[0] = groupVel[0];
			Q[n]->parentVel[1] = groupVel[1];
			Q[n]->parentVel[2] = groupVel[2];
			Q[n]->parentSizeMult = groupSizeMult;
			Q[n]->parentGain = groupGain;
			Q[n]->parentFlipH = groupFlipH;
			
			Q[n]->propogateGroupChanges(0);
		}
	}

	void setGroupAll(GLfloat rotation, GLfloat x, GLfloat y, GLfloat z, GLfloat size, ALfloat gain)		// Set ALL
	{	groupRot = rotation;
		groupPos[0]=x; groupPos[1]=y; groupPos[2]=z;
		groupSizeMult = size;
		groupGain = gain;
		propogateGroupChanges();
	}
	
	void setGroupRot(GLfloat rotation)								// rotation
	{	groupRot = rotation;
		propogateGroupChanges();
	}
	void setGroupPos(GLfloat x, GLfloat y, GLfloat z)		// Set position
	{	groupPos[0]=x; groupPos[1]=y; groupPos[2]=z;
		propogateGroupChanges();
	}
	void setGroupPosX(GLfloat x)								// X position
	{	groupPos[0]=x;
		propogateGroupChanges();
	}
	void setGroupPosY(GLfloat y)								// Y position
	{	groupPos[1]=y;
		propogateGroupChanges();
	}
	void setGroupPosZ(GLfloat z)								// Z position
	{	groupPos[2]=z;
		propogateGroupChanges();
	}
	void setGroupSizeMult(GLfloat a)							// Size
	{	groupSizeMult = a;
		propogateGroupChanges();
	}
	void setGroupGain(ALfloat a)								// Gain
	{	groupGain = a;
		propogateGroupChanges();
	}
	
	void setGroupFlipH(int a)
	{	if(a!=1 && a!=-1)
			printf("a must be 1 or -1. No change made.\n");
		else
		{	groupFlipH = a;
			propogateGroupChanges();
		}
	}	
	
	mainGroup() : groupRot(0), maxLevel(10)  // basically creates a blank object
	{	groupPos[0]=0; groupPos[1]=0; groupPos[2]=0;
		groupVel[0]=0; groupVel[1]=0; groupVel[2]=0;
		
		Q=0; nextOpenQPosition=0; sizeofQ=0;
		groupSizeMult=1; groupGain=1;
		groupFlipH=1;
	}
	
	void setQSize(int QSize)
	{	if(Q==0)
		{	Q = new objRECT*[QSize];
			sizeofQ=QSize;
			return;
		}
		//else
		objRECT** temp = new objRECT*[QSize];
		int en;
		for(en=0; en!=nextOpenQPosition; en++)
		{	if(en>=QSize)
			{	printf("ERROR: Program attempted to resize an object queue to a smaller size than\n");
				printf("       the amount of objects it was holding. Q will not be resized.\n");
				return;
			}
			temp[en] = Q[en];
		}
		delete Q;
		Q = temp;
		sizeofQ=QSize;
	}
	
	void addToQ(objRECT* object)
	{	if(nextOpenQPosition>=sizeofQ)
		{	setQSize((int)(1.5*(sizeofQ+2)));
		}
		Q[nextOpenQPosition] = object;
		nextOpenQPosition++;
			
		// Propogate object's attributes to child
		object->parentRot = groupRot;
		object->parentPos[0] = groupPos[0];
		object->parentPos[1] = groupPos[1];
		object->parentPos[2] = groupPos[2];
		object->parentVel[0] = groupVel[0];
		object->parentVel[1] = groupVel[1];
		object->parentVel[2] = groupVel[2];
		object->parentSizeMult = groupSizeMult;
		object->parentGain = groupGain;
		
		object->propogateGroupChanges(0);
	}
	
	objRECT* removeFromQ(int index)
	{	objRECT* returnThis = Q[index];
		for(int n=index; n+1!=nextOpenQPosition; n++)
		{	Q[n]=Q[n+1];
		}
		nextOpenQPosition--;
		
		return returnThis;
	}
	
	
	void draw()
	{	glClearColor(0.0f, 0.1f, 0.6f, 0.0f);				// Blue Background
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
		glMatrixMode( GL_MODELVIEW );
		
		if(Q!=0)
		{	//printf("Q[0]: %d\n",Q[0]);
			for(int n=0; n!=nextOpenQPosition; n++)
			{	Q[n]->draw(0);
				Q[n]->updateSound(0);
			}
		}
		//printf("END\n\n\n");
		SDL_GL_SwapBuffers( );
	}
	
	objRECT* firstPointCollide(float x, float y, int maxLevel) // will only check objects on the top maxLevel levels of Qs
	{	objRECT* temp;	
		for(int n=0; n<nextOpenQPosition; n++)
		{	if(Q[n]->PointCollide(x,y))
			{	return Q[n];
			}
			else if(maxLevel && (temp = Q[n]->firstPointCollide(x,y, maxLevel-1)))
			{	return temp;
			}
		}
		return 0;
	}
	
} Main;
