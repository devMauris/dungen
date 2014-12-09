#ifndef LEVEL_H
#define LEVEL_H

#include "timer.h"
#include "delTriangulation.h"

//as rooms for now
struct LVLChunk
{
	int x, y, w, h, z;
	int xvel, yvel;
	int id;
	LVLChunk()
	{
		x = 0;
		y = 0;
		xvel = 0;
		yvel = 0;
		w = 0;
		h = 0;
		z = 0;
		id = -1;
	}
};


class Level
{
	private:
		int lvlchunks_num;
		LVLChunk **lvlchunks;

		delTriangulation *triangulation;
		
		timer animation;
		void clear();
		int addChunk(int x, int y, int w, int h, int z);
		void deleteChunk(int id);

	public:
		Level();
		~Level();

		void generate(int rooms, int radius);
		void step();
		void nextStep();
		void render();

		//generation
		bool onRoomGeneration;
		bool onRoomGenerationFinished;

		bool onRoomSeparation;
		bool onRoomSeparationFinished;
		
		bool onRoomClearing;
		bool onRoomClearingFinished;

		bool onDelaunay;
		bool onDelaunayFinished;

		bool onCorridor;
		bool onCorridorFinished;
		int current_corridor;
		int corridor_tiles;


		double *rands;
		double randMax;
		int current_room, max_rooms, max_radius;

		int separation_steps;
		int deleted_rooms;
};



#endif