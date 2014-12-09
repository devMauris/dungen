#ifndef ENGINE_H
#define ENGINE_H

#define M_PI 3.14159265359

#include <SDL.h>
#include <SDL_image.h>
#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

#include "timer.h"
#include "UI.h"
#include "Level.h"
#include "delTriangulation.h"


extern double normalRand();
extern bool AABB(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

class Engine
{
	private:
		SDL_Surface *font;
		bool working;
		bool exporting;
		float FPS_max;

		//buttons
		int button_exit;
		int button_generate;
		int button_continue;
		int button_export;

		
	public:
		int width, height;
		bool active;
		float FPS;
		int bg_r, bg_g, bg_b;

		//text
		int log;
		UI *ui;
		Level *level;
		SDL_Surface *screen;

		Engine(int width, int height, float FPS_max);
		~Engine();

		int initialize();
		int work();

		void renderSurface(int x, int y, SDL_Surface *source, SDL_Rect clip);
		void renderQuad(int x, int y, int w, int h, int r, int g, int b);

		bool putPixel(int x, int y, int r, int g, int b);
		Uint32 getPixel(int x, int y);
		void renderLine(int x1, int y1, int x2, int y2, int r, int g, int b);
		void renderText(int x, int y, char *text);

		


};


#endif