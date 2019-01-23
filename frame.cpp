/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and, strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <ctime>
#include <cstdlib>
#include <iostream>
using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 520;
const int SCREEN_HEIGHT = 700;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#ifdef _SDL_TTF_H
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Creates blank texture
		bool createBlank( int width, int height, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING );

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Set self as render target
		void setAsRenderTarget();

		//Gets image dimensions
		int getWidth();
		int getHeight();

		//Pixel manipulators
		bool lockTexture();
		bool unlockTexture();
		void* getPixels();
		void copyPixels( void* pixels );
		int getPitch();
		Uint32 getPixel32( unsigned int x, unsigned int y );

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;
		void* mPixels;
		int mPitch;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The application time based timer
class LTimer
{
    public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

    private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};

//The bg that will move around on the screen
class Background
{
    public:
		//The dimensions of the dot
		static const int BG_WIDTH[7];//={100,120,160,100,100,100,100,'\0'};
		static const int BG_HEIGHT[7];//={100,120,160,100,100,100,100,'\0'};

		bool visibility[7];
		//Maximum axis velocity of the dot
		static const int BG_VEL = 640;

		//Initializes the variables
		Background();

		//Moves the dot
		void move( float timeStep );

		//Shows the dot on the screen
		void render();

		//check
		void ifVisible();

    //private:
		float mPosX[7], mPosY[7];
		float mVelX, mVelY;
};
	const int Background::BG_WIDTH[7]={100,120,160,90,193,80,130};
	const int Background::BG_HEIGHT[7]={100,120,160,90,140,80,130};


class Mane1
{
    public:
		//The dimensions
		static const int Mane1_WIDTH[6];
		static const int Mane1_HEIGHT[6];
		bool visibility[6];
		//Maximum axis velocity
		static const int Mane1_VEL = 640;

		//Initializes the variables
		Mane1();

		//Moves the dot
		void move( float timeStep );

		//Shows the dot on the screen
		void render();

		void ifVisible();

    //private:
		float mPosX[6], mPosY[6];
		float mVelX, mVelY;
};
	const int Mane1::Mane1_WIDTH[6]={62,62,62,62,62,62};
	const int Mane1::Mane1_HEIGHT[6]={71,71,71,71,71,71};

class mane2
{};



class Rocket
{
    public:
		//The dimensions of the dot
		static const int R_WIDTH=87;
		static const int R_HEIGHT=118;

		//Maximum axis velocity of the dot
		static const int R_VEL = 640;

		//Initializes the variables
		Rocket();
		int dir;
		int degrees;
		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot
		void move( float timeStep );

		//Shows the dot on the screen
		void render();

    //private:
		float mPosX, mPosY;
		float mVelX, mVelY;

		SDL_Rect R_rect={mPosX, mPosY, R_WIDTH,R_HEIGHT};
		SDL_RendererFlip flipType = SDL_FLIP_HORIZONTAL;
};


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
//0-6 bg (7) 7-12 alien (6)
LTexture gDotTexture[7];
LTexture alienTexture[6];


LTexture RocketTexture[2];

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mPixels = NULL;
	mPitch = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Convert surface to display format
		SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, NULL );
		if( formattedSurface == NULL )
		{
			printf( "Unable to convert loaded surface to display format! %s\n", SDL_GetError() );
		}
		else
		{
			//Create blank streamable texture
			newTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
			if( newTexture == NULL )
			{
				printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
			}
			else
			{
				//Enable blending on texture
				SDL_SetTextureBlendMode( newTexture, SDL_BLENDMODE_BLEND );

				//Lock texture for manipulation
				SDL_LockTexture( newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch );

				//Copy loaded/formatted surface pixels
				memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

				//Get image dimensions
				mWidth = formattedSurface->w;
				mHeight = formattedSurface->h;

				//Get pixel data in editable format
				Uint32* pixels = (Uint32*)mPixels;
				int pixelCount = ( mPitch / 4 ) * mHeight;

				//Map colors				
				Uint32 colorKey = SDL_MapRGB( formattedSurface->format, 0, 0xFF, 0xFF );
				Uint32 transparent = SDL_MapRGBA( formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00 );

				//Color key pixels
				for( int i = 0; i < pixelCount; ++i )
				{
					if( pixels[ i ] == colorKey )
					{
						pixels[ i ] = transparent;
					}
				}

				//Unlock texture to update
				SDL_UnlockTexture( newTexture );
				mPixels = NULL;
			}

			//Get rid of old formatted surface
			SDL_FreeSurface( formattedSurface );
		}	
		
		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif
		
bool LTexture::createBlank( int width, int height, SDL_TextureAccess access )
{
	//Create uninitialized texture
	mTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height );
	if( mTexture == NULL )
	{
		printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
	}
	else
	{
		mWidth = width;
		mHeight = height;
	}

	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		mPixels = NULL;
		mPitch = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

void LTexture::setAsRenderTarget()
{
	//Make self render target
	SDL_SetRenderTarget( gRenderer, mTexture );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

bool LTexture::lockTexture()
{
	bool success = true;

	//Texture is already locked
	if( mPixels != NULL )
	{
		printf( "Texture is already locked!\n" );
		success = false;
	}
	//Lock texture
	else
	{
		if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
		{
			printf( "Unable to lock texture! %s\n", SDL_GetError() );
			success = false;
		}
	}

	return success;
}

bool LTexture::unlockTexture()
{
	bool success = true;

	//Texture is not locked
	if( mPixels == NULL )
	{
		printf( "Texture is not locked!\n" );
		success = false;
	}
	//Unlock texture
	else
	{
		SDL_UnlockTexture( mTexture );
		mPixels = NULL;
		mPitch = 0;
	}

	return success;
}

void* LTexture::getPixels()
{
	return mPixels;
}

void LTexture::copyPixels( void* pixels )
{
	//Texture is locked
	if( mPixels != NULL )
	{
		//Copy to locked pixels
		memcpy( mPixels, pixels, mPitch * mHeight );
	}
}

int LTexture::getPitch()
{
	return mPitch;
}

Uint32 LTexture::getPixel32( unsigned int x, unsigned int y )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32*)mPixels;

    //Get the pixel requested
    return pixels[ ( y * ( mPitch / 4 ) ) + x ];
}


LTimer::LTimer()
{
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    //Start the timer
    mStarted = true;

    //Unpause the timer
    mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause()
{
    //If the timer is running and isn't already paused
    if( mStarted && !mPaused )
    {
        //Pause the timer
        mPaused = true;

        //Calculate the paused ticks
        mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    //If the timer is running and paused
    if( mStarted && mPaused )
    {
        //Unpause the timer
        mPaused = false;

        //Reset the starting ticks
        mStartTicks = SDL_GetTicks() - mPausedTicks;

        //Reset the paused ticks
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
	//The actual timer time
	Uint32 time = 0;

    //If the timer is running
    if( mStarted )
    {
        //If the timer is paused
        if( mPaused )
        {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}

bool LTimer::isStarted()
{
	//Timer is running and paused or unpaused
    return mStarted;
}

bool LTimer::isPaused()
{
	//Timer is running and paused
    return mPaused && mStarted;
}


Background::Background()
{
	srand(time(0));
    //Initialize the position
	for (int i=0; i<7; i++)
    {
		mPosX[i] = rand()%(520 - BG_WIDTH[i]);
    	mPosY[i] = - BG_HEIGHT[i];
		visibility[i]= false;
	}

    //Initialize the velocity
    mVelY = 80;
}

Mane1::Mane1()
{
	srand(time(0));
    //Initialize the position
	for (int i=0; i<6; i++)
    {
		mPosX[i] = rand()%(520 - Mane1_WIDTH[i]);
    	mPosY[i] =  -Mane1_HEIGHT[i];
		visibility[i]= false;
	}

    //Initialize the velocity
    mVelY = 150;
}

Rocket::Rocket()
{
	degrees=45;
	srand(time(0));
    //Initialize the position
	dir= rand()%2;
	if (dir==0)
	{
		degrees=180-degrees;
	}
	mPosX=(SCREEN_WIDTH- R_WIDTH)/2;
	mPosY= SCREEN_HEIGHT-R_HEIGHT-30;
    //Initialize the velocity
    mVelY = 100;
}



void Background::move( float timeStep )
{
    //Move the dot up or down
	for (int i=0; i<7; i++)
	{
		if (visibility[i]==true)
		{
			mPosY[i] += mVelY * timeStep;

			if( mPosY[i] > SCREEN_HEIGHT)
			{
				mPosY[i] = - BG_HEIGHT[i];
				visibility[i]=false;
			}
		}
		

void Rocket::move( float timeStep )
{
    //Move the dot up or down
	if (dir==0)
	{
		mPosX+=mVelX;
		
	}
	else if(dir==1)
	{
		mPosX-=mVelX;
	}
}

void Mane1::move( float timeStep )
{
    //Move the dot up or down
	for (int i=0; i<6; i++)
	{
		if (visibility[i]==true)
		{
			mPosY[i] += mVelY * timeStep;

			if( mPosY[i] > SCREEN_HEIGHT)
			{
				mPosY[i] = - Mane1_HEIGHT[i];
				visibility[i]=false;
			}
		}
		
	}
}

>>>>>>> 70b5faafbabad8018e3789f4211699b20ed30bb3
void Background::render()
{

    //Show the dot
	for (int i=0; i<7; i++)
	{
		if (visibility[i]==true)
		{
			gDotTexture[i].render( (int)mPosX[i], (int)mPosY[i] );
		}
		
	}
	
}


void Rocket::render()
{

    //Show the dot
	RocketTexture[dir].render( (int)mPosX, (int)mPosY);
	//SDL_RenderCopyEx( gRenderer, , NULL, &R_rect, degrees, NULL, flipType );
	
}

void Mane1::render()
{
    //Show the dot
	for (int i=0; i<6; i++)
	{
		if (visibility[i]==true)
		{
			alienTexture[i].render( (int)mPosX[i], (int)mPosY[i] );
		}
		
	}
	
}

void Background::ifVisible()
{
	int bgvisible=0, bgdistance=0;
	for (int i=0; i<7; i++)
	{
		if (Background::visibility[i]==true)
		{
			bgvisible++;
			if (Background::mPosY[i]>175)
			{
				bgdistance++;
			}
		}
	}
	while ((bgvisible<4)&&(bgdistance==bgvisible))
	{
		int p=rand()%7 ;
		Background::visibility[p]=true;
		bgvisible++;
	}
}

void Mane1::ifVisible()
{
	srand(time(0));
	int manevisible=0, manedistance=0;
	for (int i=0; i<6; i++)
	{
		if (Mane1::visibility[i]==true)
		{
			manevisible++;
			if (Mane1::mPosY[i]>(rand()%300 +350))
			{
				manedistance++;
			}
		}
	}
	//int randomnumber= rand()%3;
	if ((manevisible<2)&&(manedistance==manevisible))
	{
		int p=rand()%6;
		Mane1::visibility[p]=true;
		manevisible++;
	}
}

>>>>>>> 70b5faafbabad8018e3789f4211699b20ed30bb3
bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 11, 24, 45, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;
	
	//Load dot texture
	if( !gDotTexture[0].loadFromFile( "p1.bmp" ) )
	{
		printf( "Failed to load p1 texture!\n" );
		success = false;
	}
	
	if( !gDotTexture[1].loadFromFile( "p2.bmp" ) )
	{
		printf( "Failed to load p1 texture!\n" );
		success = false;
	}

	if( !gDotTexture[2].loadFromFile( "p3.bmp" ) )
	{
		printf( "Failed to load p1 texture!\n" );
		success = false;
	}

	if( !gDotTexture[3].loadFromFile( "p4.bmp" ) )
	{
		printf( "Failed to load p1 texture!\n" );
		success = false;
	}

	if( !gDotTexture[4].loadFromFile( "p5.bmp" ) )
	{
		printf( "Failed to load p1 texture!\n" );
		success = false;
	}

	if( !gDotTexture[5].loadFromFile( "p6.bmp" ) )
	{
		printf( "Failed to load p1 texture!\n" );
		success = false;
	}
	
	if( !gDotTexture[6].loadFromFile( "p7.bmp" ) )
	{
		printf( "Failed to load p1 texture!\n" );
		success = false;
	}


	if( !RocketTexture[0].loadFromFile( "rocket.bmp" ) )
	{
		printf( "Failed to load p1 texture!\n" );
		success = false;
	}
	if( !RocketTexture[1].loadFromFile( "rocket.bmp" ) )
	{
		printf( "Failed to load p1 texture!\n" );
		success = false;

	for (int i=0; i<6; i++)
	{
			if( !alienTexture[i].loadFromFile( "alien.bmp" ) )
		{
			printf( "Failed to load alien texture!\n" );
			success = false;
		}
	}

	return success;
}

void close()
{
	//Free loaded images
	for (int i=0; i<7; i++)
	{
		gDotTexture[i].free();
	}
	RocketTexture.free();

	for (int i=0; i<6; i++)
	{
		alienTexture[i].free();
	}

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	Background bg;


	Rocket rocket;
	Mane1 alien;

	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Keeps track of time between steps
			LTimer stepTimer;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					//bg.handleEvent( e );
				}

				bg.ifVisible();
				alien.ifVisible();

				//Calculate time step
				float timeStep = stepTimer.getTicks() / 1000.f;

				//Move for time step
				bg.move( timeStep );

				rocket.move( timeStep);

				alien.move( timeStep );

				//Restart step timer
				stepTimer.start();

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 11, 24, 45, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render dot
				bg.render();
				rocket.render();

				alien.render();
				
				

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}