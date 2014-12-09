#include "UI.h"
#include "Engine.h"
#include "main.h"



UI::UI()
{
	this->renderFPS = 0;
	this->uitexts_num = 0;
	this->uitexts = (UIText**) malloc(sizeof(UIText*) * this->uitexts_num);

	this->uibuttons_num = 0;
	this->uibuttons = (UIButton**) malloc(sizeof(UIButton*) * this->uibuttons_num);

	this->uievents_num = 0;
	this->uievents_current = 0;
	this->uievents = (UIEvent**) malloc(sizeof(UIEvent*) * this->uievents_num);
}

UI::~UI()
{
	for(int i =0; i<this->uitexts_num; i++)
		delete this->uitexts[i];
	this->uitexts_num = 0;
	delete this->uitexts;

	for(int i =0; i<this->uibuttons_num; i++)
		delete this->uibuttons[i];
	this->uibuttons_num = 0;
	delete this->uibuttons;

}

int UI::addText(const char *text, int x, int y)
{
	this->uitexts_num++;
	this->uitexts = (UIText**) realloc(this->uitexts, sizeof(UIText*) * this->uitexts_num);
	this->uitexts[this->uitexts_num-1] = new UIText;
	//filling fields

	strcpy(this->uitexts[this->uitexts_num-1]->text, text);
	this->uitexts[this->uitexts_num-1]->x = x;
	this->uitexts[this->uitexts_num-1]->y = y;
	
	//id
	srand(SDL_GetTicks());
	bool found = 0;
	int id = 0;
	while(!found)
	{
		id = rand();
		found = 1;
		
		for(int i = 0; i<this->uitexts_num; i++)
		{
			if(this->uitexts[i]->id == id)
			{
				found = 0;
				break;
			}	
		}


	}
	this->uitexts[this->uitexts_num-1]->id = id;
	
	//done!
	return this->uitexts[this->uitexts_num-1]->id;


}

void UI::deleteText(int id)
{
	//search and destroy
	for(int i=0; i<this->uitexts_num; i++)
	{
		if(this->uitexts[i]->id == id)
		{
			delete this->uitexts[i];
			this->uitexts[i] = this->uitexts[this->uitexts_num-1];
			this->uitexts_num--;
			this->uitexts = (UIText**) realloc(this->uitexts, sizeof(UIText*) * this->uitexts_num);	
		}
	}
}

UIText *UI::getText(int id)
{
	//search
	for(int i=0; i<this->uitexts_num; i++)
	{
		if(this->uitexts[i]->id == id)
		{
			return this->uitexts[i];
		}
	}

	return NULL;
}

int UI::addButton(const char *text, int x, int y, int w, int h, int r, int g, int b)
{
	this->uibuttons_num++;
	this->uibuttons = (UIButton**) realloc(this->uibuttons, sizeof(UIButton*) * this->uibuttons_num);
	this->uibuttons[this->uibuttons_num-1] = new UIButton;
	//filling fields

	strcpy(this->uibuttons[this->uibuttons_num-1]->text, text);
	this->uibuttons[this->uibuttons_num-1]->x = x;
	this->uibuttons[this->uibuttons_num-1]->y = y;
	this->uibuttons[this->uibuttons_num-1]->w = w;
	this->uibuttons[this->uibuttons_num-1]->h = h;
	this->uibuttons[this->uibuttons_num-1]->r = r;
	this->uibuttons[this->uibuttons_num-1]->g = g;
	this->uibuttons[this->uibuttons_num-1]->b = b;
	
	//id
	srand(SDL_GetTicks());
	bool found = 0;
	int id = 0;
	while(!found)
	{
		id = rand();
		found = 1;
		
		for(int i = 0; i<this->uibuttons_num; i++)
		{
			if(this->uibuttons[i]->id == id)
			{
				found = 0;
				break;
			}	
		}


	}
	this->uibuttons[this->uibuttons_num-1]->id = id;
	
	//done!
	return this->uibuttons[this->uibuttons_num-1]->id;


}

void UI::deleteButton(int id)
{
	//search and destroy
	for(int i=0; i<this->uibuttons_num; i++)
	{
		if(this->uibuttons[i]->id == id)
		{
			delete this->uibuttons[i];
			this->uibuttons[i] = this->uibuttons[this->uibuttons_num-1];
			this->uibuttons_num--;
			this->uibuttons = (UIButton**) realloc(this->uibuttons, sizeof(UIButton*) * this->uibuttons_num);	
		}
	}

}

UIButton *UI::getButton(int id)
{
	//search
	for(int i=0; i<this->uibuttons_num; i++)
	{
		if(this->uibuttons[i]->id == id)
		{
			return this->uibuttons[i];
		}
	}

	return NULL;

}

void UI::setRenderFps(bool render)
{
	this->renderFPS = render;
}

void UI::render()
{
	

	//render texts
	for(int i =0; i<this->uitexts_num; i++)
	{
		mainEngine->renderText(this->uitexts[i]->x, this->uitexts[i]->y, this->uitexts[i]->text);
	}

	if(this->renderFPS)
	{
		char FPS[256] = {'\0'};
		sprintf(FPS, "FPS: %.3f", mainEngine->FPS);
		mainEngine->renderText(10, 10, FPS);

	}
	//render buttons?
	for(int i=0; i<this->uibuttons_num; i++)
	{
		if(this->uibuttons[i]->clicked)
			mainEngine->renderQuad(	this->uibuttons[i]->x, this->uibuttons[i]->y, 
									this->uibuttons[i]->w, this->uibuttons[i]->h, 
									this->uibuttons[i]->r + 60, this->uibuttons[i]->g + 60, this->uibuttons[i]->b + 60);
		else
			mainEngine->renderQuad(	this->uibuttons[i]->x, this->uibuttons[i]->y, 
									this->uibuttons[i]->w, this->uibuttons[i]->h, 
									this->uibuttons[i]->r, this->uibuttons[i]->g, this->uibuttons[i]->b);

		mainEngine->renderText( this->uibuttons[i]->x + this->uibuttons[i]->w/2 - strlen(this->uibuttons[i]->text)/2 * 6,
								this->uibuttons[i]->y + this->uibuttons[i]->h/2 - 4, this->uibuttons[i]->text);

	}

	//release all buttons in the end of pipeline
	for(int i =0; i<this->uibuttons_num; i++)
		this->uibuttons[i]->released = 0;
}

void UI::handle(SDL_Event *event)
{
	UIEvent uievent;
	//handle buttons
	switch(event->type)
	{
		case SDL_MOUSEMOTION:
			for(int i = 0; i<this->uibuttons_num; i++)
			{
				//AABB
				if(	this->uibuttons[i]->x < event->motion.x && this->uibuttons[i]->x + this->uibuttons[i]->w > event->motion.x 
					&& this->uibuttons[i]->y < event->motion.y && this->uibuttons[i]->y + this->uibuttons[i]->h > event->motion.y)
				{
					if(this->uibuttons[i]->over == 0)
					{
						uievent.id = this->uibuttons[i]->id;
						uievent.type = UIEVENT_BUTTONOVER;
					}
					this->uibuttons[i]->over = 1;
				}
				else
				{
					this->uibuttons[i]->over = 0;
					this->uibuttons[i]->clicked = 0;
				}
			}
		break;

		case SDL_MOUSEBUTTONDOWN:
			for(int i = 0; i<this->uibuttons_num; i++)
			{
				//Click
				if(this->uibuttons[i]->over)
				{
					this->uibuttons[i]->clicked = 1;
					uievent.id = this->uibuttons[i]->id;
					uievent.type = UIEVENT_BUTTONCLICKED;
				}
				else
					this->uibuttons[i]->clicked = 0;
			}
		break;
		case SDL_MOUSEBUTTONUP:
			for(int i = 0; i<this->uibuttons_num; i++)
			{
				//Click
				if(this->uibuttons[i]->clicked)
				{
					this->uibuttons[i]->clicked = 0;
					this->uibuttons[i]->released = 1;
					uievent.id = this->uibuttons[i]->id;
					uievent.type = UIEVENT_BUTTONRELEASED;
				}
			}

		break;


	}

	if(uievent.type != UIEVENT_NONE)
		this->addUIEvent(uievent);

}

void UI::addUIEvent(UIEvent &event)
{
	this->uievents_num++;
	this->uievents = (UIEvent**) realloc(this->uievents, sizeof(UIEvent*) * this->uievents_num);
	this->uievents[this->uievents_num-1] = new UIEvent();
	memcpy(this->uievents[this->uievents_num-1], &event, sizeof(UIEvent));

}

bool UI::pollEvent(UIEvent *event)
{
	if(this->uievents_num == 0)
		return 0;

	event->type = this->uievents[this->uievents_current]->type;
	event->id = this->uievents[this->uievents_current]->id;

	//last one
	this->uievents_current++;
	if(this->uievents_current > this->uievents_num-1)
	{
		this->uievents_current = 0;
		for(int i =0; i<this->uievents_num; i++)
			delete this->uievents[i];

		//shrink
		this->uievents_num = 0;
		this->uievents = (UIEvent**) realloc(this->uievents, sizeof(UIEvent*) * this->uievents_num);
	}

	return 1;
}