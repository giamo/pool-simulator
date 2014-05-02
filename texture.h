#ifndef _TEXTURE_
#define _TEXTURE_

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <GL/glut.h>
#include <GL/gl.h>

#define BITMAP_ID 0x4D42

typedef struct texture texture;
struct texture{
	GLubyte* data;			//Image data (up to 32 bits)
	unsigned int bpp;		//Image color depth in bits per pixel
	unsigned int width;		//Image width
	unsigned int height;	//Image height
	unsigned int ID;		//Texture ID used to select a texture
};

texture* newTexture(char* filename);
void deleteTexture(texture* tex);
int LoadTGA(char* filename, GLfloat minFilter, GLfloat maxFilter, texture* tex);

texture* newTexture(char* filename){
	texture* tex = (texture*)malloc(sizeof(texture));
	LoadTGA(filename, GL_LINEAR, GL_LINEAR, tex);
	return tex;
}

void deleteTexture(texture* tex){
	glDeleteTextures(1, &tex->ID);
	free(tex);
}

int LoadTGA(char* filename, float minFilter, float maxFilter, texture* tex){    
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};//Uncompressed TGA header
	GLubyte		TGAcompare[12];//Used to compare TGA header
	GLubyte		header[6];     //The first six useful bytes from the header
	GLuint		bytesPerPixel; //Holds the bpp of the TGA
	GLuint		imageSize;     //Used to store image size while in RAM
	GLuint temp;               //Temp variable
	GLuint type = GL_RGBA;     //Set the default OpenGL mode to RBGA (32 BPP)
	
	FILE* file = fopen(filename, "rb"); // Open The TGA File
	
	if(file==NULL || // Does File Even Exist?
			fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) || // Are There 12 Bytes To Read?
			memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0 || // Does The Header Match What We Want?
			fread(header,1,sizeof(header),file)!=sizeof(header)){// If So Read Next 6 Header Bytes
		if(file==NULL){// Did The File Even Exist? *Added Jim Strong*
			printf("%s does not exist.", filename);
			return 0;							
		}
		else{
			if(fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare)){
				printf("Non ci sono 12 byte da leggere\n");
			}
			if(memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0){
				printf("L'header non è quello che vogliamo\n");
			}
			if(fread(header,1,sizeof(header),file)!=sizeof(header)){
				printf("Non ci sono altri 6 byte da leggere\n");
			}
			fclose(file); // If anything failed, close the file
			//printf("Could not load %s correctly, general failure.", filename);
			return 0;						
		}
	}
	
	tex->width = header[1] * 256 + header[0]; // Determine The TGA Width	(highbyte*256+lowbyte)
	tex->height= header[3] * 256 + header[2]; // Determine The TGA Height	(highbyte*256+lowbyte)
	
	if(tex->width	<=0	|| // Is The Width Less Than Or Equal To Zero
			tex->height<=0 || // Is The Height Less Than Or Equal To Zero
			(header[4]!=24 && header[4]!=32)){ // Is The TGA 24 or 32 Bit?
		fclose(file); // If Anything Failed, Close The File
		printf("%s's height or width is less than zero, or the TGA is not 24 or 32 bits.", filename);
		return 0;							
	}
	
	tex->bpp = header[4];      // Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel= tex->bpp/8; // Divide By 8 To Get The Bytes Per Pixel
	imageSize = tex->width*tex->height*bytesPerPixel; // Calculate The Memory Required For The TGA Data
	
	tex->data = (GLubyte*)malloc(imageSize*sizeof(GLubyte)); // Reserve Memory To Hold The TGA Data
	
	if(tex->data==NULL || // Does The Storage Memory Exist?
			fread(tex->data, 1, imageSize, file)!=imageSize){ // Does The Image Size Match The Memory Reserved?
		if(tex->data!=NULL) // Was Image Data Loaded
			free(tex->data); // If So, Release The Image Data
		
		printf("Storate memory for %s does not exist or is corrupted.", filename);
		
		fclose(file); // Close The File
		return 0;     // Return False
	}
	
	GLuint i;
	for(i=0; i<(int)(imageSize); i+=bytesPerPixel){ // Loop Through The Image Data
	// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp = tex->data[i]; // Temporarily Store The Value At Image Data 'i'
		tex->data[i] = tex->data[i + 2]; // Set The 1st Byte To The Value Of The 3rd Byte
		tex->data[i+2] = temp; // Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}
	
	fclose (file); //Close the file
	
	// Build A Texture From The Data
	glGenTextures(1, &(tex->ID)); //Generate OpenGL texture IDs
	
	glBindTexture(GL_TEXTURE_2D, tex->ID); //Bind the texture to a texture object
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter); //Filtering for if texture is bigger than should be
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter); //Filtering for if texture is smaller than it should be
	
	if(tex->bpp==24) //Was the TGA 24 bpp?
		type=GL_RGB;							
	
	glTexImage2D(GL_TEXTURE_2D, 0, type, tex->width, tex->height, 0, type, GL_UNSIGNED_BYTE, tex->data);
	
	printf("Loaded %s correctly.\n", filename);
	free(tex->data);
	return 1;
}

#endif
