#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include <stdio.h>
#include <iostream>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 361;                       // CUSTOM WIDTH AND HEIGHT
const int SCREEN_HEIGHT = 636;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		
		//Deallocates texture
		void free();
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );



	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
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

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
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
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
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
	}
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

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
    SDL_Init( SDL_INIT_VIDEO );

    //Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    //Create vsynced renderer for window
    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

    //Initialize renderer color
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    IMG_Init( IMG_INIT_PNG );
	return success;
}

LTexture gBackground;

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load sprites
	if( !gBackground.loadFromFile( "background.png" ) )                   //  LOAD MEDIA
	{
		printf( "Failed to load button sprite texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gBackground.free();

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
	//Start up SDL and create window
    init();
    loadMedia();
    
    bool quit = false;

    //Event handler
    SDL_Event e;

    //While application is running
    while( !quit ){
        while( SDL_PollEvent( &e ) != 0 ){
            if( e.type == SDL_QUIT )         quit = true;
            
            else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP){
                int x, y;
                SDL_GetMouseState(&x, &y);
                std::cout << x << " " << y;
                quit = true;
            }
        }

        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );
        
        gBackground.render(0, 0);

        SDL_RenderPresent( gRenderer );

	}
	close();

	return 0;
}
