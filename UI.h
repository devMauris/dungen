#ifndef UI_H
#define UI_H

#define UIEVENT_NONE 0x0
#define UIEVENT_BUTTONOVER 0x1
#define UIEVENT_BUTTONCLICKED 0x2
#define UIEVENT_BUTTONRELEASED 0x3

#include <SDL.h>

struct UIText
{
	char text[512]; //512 will be enough
	int x, y;

	int id;
	
	UIText()
	{
		x = 0;
		y = 0;
		id = -1;
	}

};

struct UIButton
{
	char text[512]; //super huge buttons
	int x, y, w, h;
	int r, g, b;
	int id;
	bool over, clicked, released;

	UIButton()
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		r = 0;
		g = 0;
		b = 0;
		over = 0;
		clicked = 0;
		released = 0;
		id = -1;
	}


};

struct UIEvent
{
	int type;
	int id;

	UIEvent()
	{
		type = UIEVENT_NONE;
		id = -1;
	}
};

class UI
{
	private:
		bool renderFPS;
		int uitexts_num;
		UIText **uitexts;

		int uibuttons_num;
		UIButton **uibuttons;

		int uievents_num, uievents_current;
		UIEvent **uievents;
		void addUIEvent(UIEvent &event);

	public:
		void render();
		void handle(SDL_Event *event);
		void setRenderFps(bool render);

		int addText(const char *text, int x, int y);
		UIText *getText(int id);
		void deleteText(int id);

		int addButton(const char *text, int x, int y, int w, int h, int r, int g, int b);
		UIButton *getButton(int id);
		void deleteButton(int id);

		bool pollEvent(UIEvent *event);

		UI();
		~UI();

};


#endif