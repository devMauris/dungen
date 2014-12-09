#include "Engine.h"



Engine::Engine(int width, int height, float FPS_max)
{
	this->screen = NULL;

	this->width = width;
	this->height = height;
	this->FPS_max = FPS_max;
	this->FPS = -1.f;
	this->bg_r = 10;
	this->bg_g = 10;
	this->bg_b = 20;

	this->ui = new UI();
	this->level = new Level();
	this->ui->setRenderFps(1);
	this->log = this->ui->addText("", 10, 28);
	
	this->button_exit = this->ui->addButton("Exit", this->width - 48, 0, 48, 48, 60, 0, 0);
	this->button_generate = this->ui->addButton("Generate", this->width - 80, this->height - 48, 80, 48, 0, 60, 0);
	this->button_continue = this->ui->addButton("Continue", this->width - 170, this->height - 48, 80, 48, 0, 60, 0);
	this->button_export = this->ui->addButton("Export BMP", this->width - 80, this->height - 106, 80, 48, 60, 60, 60);
	this->exporting = 0;
}

Engine::~Engine()
{
	SDL_FreeSurface(this->screen);
	delete this->ui;
	delete this->level;
}

int Engine::initialize()
{
	//#MAIN SDL INITIALIZING#
	freopen("stdout.txt","wt",stdout);
	printf("Initializing SDL...\n");
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL error: %s\n",SDL_GetError());
		return 1;
	}

	//create window
	printf("Done!\nCreating window...\n");
	SDL_WM_SetCaption("DunGen", NULL);
	SDL_ShowCursor(1);
	this->screen = SDL_SetVideoMode(this->width, this->height, 32, SDL_HWSURFACE);
	if(this->screen == NULL)
	{
		printf("SDL error: %s\n",SDL_GetError());
		return 1;
	}

	//Load font
	printf("Done!\nLoading font.png...\n");
	this->font = IMG_Load("data//font.png");
	if(this->font == NULL)
	{
		printf("Font: font.png is missing from data\n");
		return 1;
	}
	printf("Done!\n");
	return 0;
}

int Engine::work()
{
	//SDL vars for WM
	SDL_Event event;
	timer FPS_counter,FPS_updater;
	int FPS_frames=0;
	this->working = 1;
	this->active = 1;
	
	//fixing additional mouse events due to Win
	if(this->active)
	{
		//SDL_WarpMouse(this->width/2,this->height/2);
		while(SDL_PollEvent(&event));
	}
	
	//FPS updater start
	FPS_updater.t_Start();
	//#MAIN LOOP
	while(this->working)
	{
		bool lag = 0;
		//Timer for each frame
		FPS_counter.t_Start();
		//mouse
		//SDL_ShowCursor(!this->active);

		//#SDL EVENTS HANDLING#
		while(SDL_PollEvent(&event))
		{
			ui->handle(&event);
			switch(event.type)
			{
			case SDL_QUIT:
				this->working = 0;
				fclose(stdout);
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
				case SDLK_q:
					this->working = 0;
					break;
				}
				
				break;

			
			}
			
		}
		UIEvent uievent;
		while(ui->pollEvent(&uievent))
		{
			switch(uievent.type)
			{
				case UIEVENT_BUTTONCLICKED:
					if(uievent.id == this->button_exit)
						this->working = 0;
					if(uievent.id == this->button_generate)
					{
						this->level->generate(120, 10);
						sprintf(this->ui->getText(this->log)->text, "Generation started\n");
					}	
					if(uievent.id == this->button_continue)
						this->level->nextStep();
					if(uievent.id == this->button_export)
						this->exporting = 1;
				break;

			}
		}
		
		this->level->step();
		//#RENDER

		this->renderQuad(0, 0, this->width, this->height, this->bg_r, this->bg_g, this->bg_b);
		this->level->render();
		if(!exporting)
		{
			if(this->level->onRoomGeneration)
			{
				for(int i =0; i<this->level->current_room+1; i++)
				{
					int h = this->level->rands[i] * 40 + 1;
					int y = 470 - h;
					this->renderQuad(10 + i*2, y, 2, h, 0, 232, 0);
				}
			}
			
			this->ui->render();
		}
		
		if(exporting)
		{
			if(this->level->onRoomGenerationFinished && !this->level->onRoomSeparationFinished)
				SDL_SaveBMP(this->screen, "00export_generation.bmp");
			if(this->level->onRoomSeparationFinished && !this->level->onRoomClearingFinished)
				SDL_SaveBMP(this->screen, "01export_separation.bmp");
			if(this->level->onRoomClearingFinished && !this->level->onDelaunayFinished)
				SDL_SaveBMP(this->screen, "02export_cleaning.bmp");
			if(this->level->onDelaunayFinished && !this->level->onCorridorFinished)
				SDL_SaveBMP(this->screen, "03export_tree_building.bmp");
			if(this->level->onCorridorFinished)
				SDL_SaveBMP(this->screen, "04export_final.bmp");

			this->exporting = 0;
		}
		else
		{
			//END of render
			SDL_Flip(this->screen); //actual render
		}
		
		//#FPS CONTROL#
		FPS_frames++;
		//Regulating FPS
		if(FPS_counter.t_GetTime() < 1000.f / FPS_max && FPS_max > 0.0)
		{
			SDL_Delay((int) ((1000.f / FPS_max + 0.5) - FPS_counter.t_GetTime()));
		}

		//Calculating FPS
		if(FPS_updater.t_GetTime()>1000)
		{
			FPS = FPS_frames / (FPS_updater.t_GetTime() / 1000.f);
			//FPS_physics = FPS_frames / (FPS_physics_counter.t_GetTime() / 1000.f);
			
			FPS_frames = 0;
			FPS_updater.t_Start();
			
		}


		//END of main loop

	}


	return 0;
}

void Engine::renderSurface(int x, int y, SDL_Surface *source, SDL_Rect clip)
{
	 //Temporary rectangle to hold the offsets
    SDL_Rect offset;
    
    //Get the offsets
    offset.x = x;
    offset.y = y;
    
    //Blit the surface
	SDL_BlitSurface(source, &clip, this->screen, &offset);
}

void Engine::renderQuad(int x, int y, int w, int h, int r, int g, int b)
{
	SDL_Rect tmp;
	Uint32 color = SDL_MapRGB(this->screen->format, r, g, b);
	tmp.x = x;
	tmp.y = y;
	tmp.w = w;
	tmp.h = h;
	SDL_FillRect(this->screen, &tmp, color);
}

void Engine::renderText(int x, int y, char *text)
{
	// "*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
	int sym_at_x = x;
	int sym_at_y = y;

	int step = 8; //width
	int squeeze = 2;
	for(int i =0; i<strlen(text); i++)
	{
		if(text[i] == '\n')
		{
			sym_at_x = x;
			sym_at_y += step;
			continue;
		}

		//correction for slim symbols
		if(i>0 && strchr("!,.:;[]itl{|}", text[i-1]))
			squeeze = 4;
		else
			squeeze = 2;
		
		int font_x = (text[i] - '!')%23;
		int font_y = (text[i] - '!')/23;
		
		//render
		SDL_Rect clip;
		clip.x = font_x * 8;
		clip.y = font_y * 8;
		clip.w = 8;
		clip.h = 8;

		this->renderSurface(sym_at_x - squeeze, sym_at_y, this->font, clip);

		sym_at_x += step;
		sym_at_x -= squeeze;
	}
}

bool Engine::putPixel(int x, int y, int r, int g, int b)
{
	if(y < 0 | y > this->screen->h -1 | x < 0 | x > this->screen->w -1)
		return 1;
	
	//Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)this->screen->pixels;
    
    //Set the pixel
	pixels[ ( y * this->screen->w ) + x ] = SDL_MapRGB(this->screen->format, r, g, b);

	return 0;
}

Uint32 Engine::getPixel(int x, int y)
{
	//Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)this->screen->pixels;
    
    //Get the requested pixel
    return pixels[ ( y * this->screen->w ) + x ];

}

void Engine::renderLine(int x1, int y1, int x2, int y2, int r, int g, int b)
{
	float xf = x1;
    float yf = y1;
    float xt = x2;
    float yt = y2;
    float dy = yt - yf;
    float dx = xt - xf;
	float xy = dx/dy;
	float yx = dy/dx;
   
    if (fabs(dy) > fabs(dx)) 
	{
        int sign = dy < 0.0 ? -1 : 1;
		for( y2 = yf; y2 != yt; y2 += sign ) 
		{
            x2 = xf + ( y2 - yf ) * xy;
			this->renderQuad(x2, y2, 1, 1, r, g, b);
        }

    }
   
    else 
	{
		int sign = dx < 0.0 ? -1 : 1;
		for(x2 = xf; x2 != xt; x2 += sign) 
		{
            y2 = yf + ( x2 - xf ) * yx;
			
			this->renderQuad(x2, y2, 1, 1, r, g, b);
        }
		
    }

}

//wow so fast
double normalRand()
{
	static int i = 1;
	static double u[2] = {0.0, 0.0};
	register double r[2];

	if (i == 1)
	{
		r[0] = sqrt(-2*log((double)(rand()+1)/(double)(RAND_MAX+1)));
		r[1] = 2*M_PI*(double)(rand()+1)/(double)(RAND_MAX+1);
		u[0] = r[0]*sin(r[1]);
		u[1] = r[0]*cos(r[1]);
		i = 0;
	} 
	else
	{
		i = 1;
	}

	return u[i];

}

bool AABB(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return !((x2 + w2) <= x1 || x2 >= (x1 + w1) || (y2 + h2) <= y1 || y2 >= (y1 + h1));
}