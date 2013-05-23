#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

/* must link to dynamic libraries:
	DevIL.dll
	ilu.dll
*/
/* maybe has to link to 
	ilut.dll
	libPNG12.DLL
	SDL_image.dll
*/

/* must link to static libraries:
-lopengl32 
-lDevIL 
-lilut
-lilu
*/

// returns ratio of surface width over height.
float LoadGLTextures(char* name, GLuint* StoreHere)									// Load Bitmaps And Convert To Textures
{	float HoverW;
	SDL_Surface *surface;	// This surface will tell us the details of the image
 	ILuint texid;
 	int oldWidth, oldHeight;
 
 	ilGenImages(1, &texid); /* Generation of one image name */
	ilBindImage(texid); /* Binding of image name */
	bool THEWHO = ilLoadImage(name);
	if (THEWHO)
	{	
		if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))	// Convert every colour component into unsigned byte. IL_RGBA for images with alpha
		{	printf("ilConvertImage FAILED. YOU FAIL. GAME OVER.\n");
			getchar();
		}
		oldWidth = ilGetInteger(IL_IMAGE_WIDTH);
		oldHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		if ( (ilGetInteger(IL_IMAGE_WIDTH) & (ilGetInteger(IL_IMAGE_WIDTH) - 1)) != 0) 
		{	//printf("warning: image.bmp's width is not a power of 2\n");
			int powerOf2=2;
			while(powerOf2 < oldWidth)
			{	powerOf2*=2;
			}
			iluScale( powerOf2, ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_DEPTH));
		}
		if ( (ilGetInteger(IL_IMAGE_HEIGHT) & (ilGetInteger(IL_IMAGE_HEIGHT) - 1)) != 0) 
		{	//printf("warning: image.bmp's height is not a power of 2\n");
			int powerOf2=2;
			while(powerOf2 < oldHeight)
			{	powerOf2*=2;
			}
			iluScale(ilGetInteger(IL_IMAGE_WIDTH), powerOf2, ilGetInteger(IL_IMAGE_DEPTH));
		}
	
		//printf("Image %s: width, height = %d, %d\n", name, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT) );
		glGenTextures(1, StoreHere); /* Texture name generation */
		glBindTexture(GL_TEXTURE_2D, *StoreHere); /* Binding of texture name */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* Linear interpolation for magnification filter */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* Linear interpolation for minifying filter */
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
					ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
					ilGetData()); /* Texture specification */
	}
	else
	{	printf("An error occured bitch. ilLoadImage FAILED for %s.\n", name);
		//getchar();
		return 0;
	}
	HoverW = (float)oldHeight/oldWidth;
	ilDeleteImages(1, &texid); // Release memory used by image after loading it into vid memory.
 
 	return HoverW; // no error
}
