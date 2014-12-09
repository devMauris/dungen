#include "Level.h"
#include "main.h"
#include "Engine.h"



Level::Level()
{
	this->lvlchunks_num = 0;
	this->lvlchunks = (LVLChunk**) malloc(sizeof(LVLChunk*) * this->lvlchunks_num);
	this->onRoomGeneration = 0;
	this->onRoomGenerationFinished = 0;
	this->onRoomSeparation = 0;
	this->onRoomSeparationFinished = 0;
	this->onRoomClearing = 0;
	this->onRoomClearingFinished = 0;
	this->onDelaunay = 0;
	this->onDelaunayFinished = 0;
	this->onCorridor = 0;
	this->onCorridorFinished = 0;

	this->max_rooms = 0;
	this->rands = (double*) malloc(sizeof(double) * this->max_rooms);

	this->triangulation = new delTriangulation();
}

Level::~Level()
{
	for(int i =0; i<this->lvlchunks_num; i++)
		delete this->lvlchunks[i];
	delete this->lvlchunks;

	delete this->rands;

}

void Level::clear()
{
	for(int i=0; i<this->lvlchunks_num; i++)
		delete this->lvlchunks[i];
	this->lvlchunks_num = 0;
	this->lvlchunks = (LVLChunk**) realloc(this->lvlchunks, sizeof(LVLChunk*) * this->lvlchunks_num);
}

int Level::addChunk(int x, int y, int w, int h, int z)
{
	this->lvlchunks_num++;
	this->lvlchunks = (LVLChunk**) realloc(this->lvlchunks, sizeof(LVLChunk*) * this->lvlchunks_num);
	this->lvlchunks[this->lvlchunks_num-1] = new LVLChunk();

	this->lvlchunks[this->lvlchunks_num-1]->x = x;
	this->lvlchunks[this->lvlchunks_num-1]->y = y;
	this->lvlchunks[this->lvlchunks_num-1]->w = w;
	this->lvlchunks[this->lvlchunks_num-1]->h = h;
	this->lvlchunks[this->lvlchunks_num-1]->z = z;

	//id
	srand(SDL_GetTicks());
	bool found = 0;
	int id = 0;
	while(!found)
	{
		id = rand();
		found = 1;
		
		for(int i = 0; i<this->lvlchunks_num; i++)
		{
			if(this->lvlchunks[i]->id == id)
			{
				found = 0;
				break;
			}	
		}


	}
	this->lvlchunks[this->lvlchunks_num-1]->id = id;
	
	//done!
	return this->lvlchunks[this->lvlchunks_num-1]->id;

}

void Level::deleteChunk(int id)
{
	for(int i =0; i<this->lvlchunks_num; i++)
	{
		if(this->lvlchunks[i]->id == id)
		{
			delete this->lvlchunks[i];
			this->lvlchunks[i] = this->lvlchunks[this->lvlchunks_num-1];
			this->lvlchunks_num--;
			this->lvlchunks = (LVLChunk**) realloc(this->lvlchunks, sizeof(LVLChunk*) * this->lvlchunks_num);
		}
	}
}

void Level::generate(int rooms, int radius)
{
	this->clear();
	this->onRoomGeneration = 1;
	this->current_room = 0;
	this->onRoomSeparation = 0;
	this->onRoomGenerationFinished = 0;
	this->onRoomSeparationFinished = 0;
	this->onRoomClearing = 0;
	this->onRoomClearingFinished = 0;
	this->onDelaunay = 0;
	this->onDelaunayFinished = 0;
	this->onCorridor = 0;
	this->onCorridorFinished = 0;
	this->current_corridor = 0;
	this->corridor_tiles = 0;
	
	this->separation_steps = 0;
	this->deleted_rooms = 0;

	this->max_rooms = rooms;
	this->rands = (double*) realloc(this->rands, sizeof(double) * this->max_rooms);
	this->randMax = -1.f;
	this->max_radius = radius-1;
	srand(SDL_GetTicks());
	for(int i =0; i<rooms; i++)
	{
		this->rands[i] = abs(normalRand());
		if(this->randMax < this->rands[i] || i == 0)
			this->randMax = this->rands[i];
	}
	//normalize
	for(int i =0; i<rooms; i++)
	{
		this->rands[i] /= randMax;
	}

	this->animation.t_Start();

	delete this->triangulation;
	this->triangulation = new delTriangulation();
}

void Level::step()
{
	if(this->onRoomGeneration && this->animation.t_GetTime() > 25)
	{
		this->animation.t_Start();
		//add 1 new room
		
		//use circle
		float angle = float(rand())/RAND_MAX * 2 * M_PI;
		float radius = float(rand())/RAND_MAX * 25;

		int x = 50 + cos(angle) * radius;
		int y = 30 + sin(angle) * radius;
		
		
		//use normal distribution
		int w = this->rands[this->current_room] * this->max_radius + 1;
		int room_wide = rand()%2;

		double max_ratio = 0.8, min_ratio = 0.3;
		double ratio = double(rand())/RAND_MAX * (max_ratio - min_ratio) + min_ratio;
		
		int h = w * ratio + 1;
		
		if(room_wide)
		{
			int tmp = w;
			w = h;
			h = tmp;
		}	
		
		int z = 1;
		this->addChunk(x, y, w, h, z);

		this->current_room++;

		if(this->current_room >= this->max_rooms)
		{
			this->onRoomGeneration = 0; //finish
			this->animation.t_Stop();
			this->onRoomGenerationFinished = 1;
			sprintf(mainEngine->ui->getText(mainEngine->log)->text + strlen(mainEngine->ui->getText(mainEngine->log)->text), "Generated %i rooms\n", this->max_rooms);

		}
	}

	if(this->onRoomSeparation && this->animation.t_GetTime() > 25)
	{
		//animate
		this->animation.t_Start();
		//separate rooms
		bool finish = 1;
		for(int i =0; i<this->lvlchunks_num; i++)
		{
			for(int j = i+1; j<this->lvlchunks_num; j++)
			{
				if(i == j)
					continue;
				if(AABB(this->lvlchunks[i]->x, this->lvlchunks[i]->y, this->lvlchunks[i]->w, this->lvlchunks[i]->h, 
					this->lvlchunks[j]->x, this->lvlchunks[j]->y, this->lvlchunks[j]->w, this->lvlchunks[j]->h))
				{
					
					finish = 0;
					int IcenterX = this->lvlchunks[i]->x + this->lvlchunks[i]->w/2;
					int JcenterX = this->lvlchunks[j]->x + this->lvlchunks[j]->w/2;
					int IcenterY = this->lvlchunks[i]->y + this->lvlchunks[i]->h/2;
					int JcenterY = this->lvlchunks[j]->y + this->lvlchunks[j]->h/2;
					
					if(IcenterX < JcenterX)
						this->lvlchunks[i]->xvel = -1;
					else
						this->lvlchunks[i]->xvel = 1;
					if(IcenterY < JcenterY)
						this->lvlchunks[i]->yvel = -1;
					else
						this->lvlchunks[i]->yvel = 1;


					this->lvlchunks[j]->xvel -= this->lvlchunks[i]->xvel;
					this->lvlchunks[j]->yvel -= this->lvlchunks[i]->yvel;
					
				}
				//out of bounds?


			}
		}
		this->separation_steps++;
		for(int i =0; i<this->lvlchunks_num; i++)
		{
			//normalize movement
			if(this->lvlchunks[i]->xvel < 0)
				this->lvlchunks[i]->xvel = -1;
			if(this->lvlchunks[i]->xvel > 0)
				this->lvlchunks[i]->xvel = 1;

			this->lvlchunks[i]->x += this->lvlchunks[i]->xvel;
			this->lvlchunks[i]->xvel = 0;

			if(this->lvlchunks[i]->yvel < 0)
				this->lvlchunks[i]->yvel = -1;
			if(this->lvlchunks[i]->yvel > 0)
				this->lvlchunks[i]->yvel = 1;


			this->lvlchunks[i]->y += this->lvlchunks[i]->yvel;
			this->lvlchunks[i]->yvel = 0;

		}
		if(finish)
		{
			this->onRoomSeparation = 0;
			this->onRoomSeparationFinished = 1;
			this->animation.t_Stop();
			sprintf(mainEngine->ui->getText(mainEngine->log)->text + strlen(mainEngine->ui->getText(mainEngine->log)->text), "Separated rooms in %i steps\n", this->separation_steps);

		}

	}

	if(this->onRoomClearing && this->animation.t_GetTime() > 10)
	{
		this->animation.t_Start();
		bool found = 0;
		for(int i =0; i<this->lvlchunks_num; i++)
		{
			if(this->lvlchunks[i]->w < this->max_radius/2 || this->lvlchunks[i]->h < this->max_radius/2)
			{
				this->deleteChunk(this->lvlchunks[i]->id);
				found = 1;
				this->deleted_rooms++;
				break;
			}
		}
		if(!found)
		{
			this->onRoomClearing = 0;
			this->onRoomClearingFinished = 1;
			this->animation.t_Stop();
			sprintf(mainEngine->ui->getText(mainEngine->log)->text + strlen(mainEngine->ui->getText(mainEngine->log)->text), "Deleted %i rooms\n", this->deleted_rooms);
			
		}

	}

	if(this->onDelaunay && this->animation.t_GetTime() > 25)
	{
		if(this->triangulation->step())
		{
			//finish
			this->onDelaunay = 0;
			this->onDelaunayFinished = 1;
			this->animation.t_Stop();
			sprintf(mainEngine->ui->getText(mainEngine->log)->text + strlen(mainEngine->ui->getText(mainEngine->log)->text), "Tree built with %i final edges\n\n", this->triangulation->mst_tree_num);
		}

	}
	
	if(this->onCorridor && this->animation.t_GetTime() > 25)
	{
		if(this->current_corridor >= this->triangulation->mst_tree_num)
		{
			//finish
			this->onCorridor = 0;
			this->onCorridorFinished = 1;
			this->animation.t_Stop();
			sprintf(mainEngine->ui->getText(mainEngine->log)->text + strlen(mainEngine->ui->getText(mainEngine->log)->text), "Generated %i corridor tiles\n\n", this->corridor_tiles);
		}
		else
		{
			Point2d cr_vec, xroom_center;
			xroom_center.x = this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->x + this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->w/2;
			xroom_center.y = this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->y + this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->h/2;
			
			cr_vec.x = this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->x + this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->w/2;
			cr_vec.y = this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->y + this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->h/2;
			cr_vec.x -= xroom_center.x;
			cr_vec.y -= xroom_center.y;
			
			//X part of corridor
			Point2d cursor = xroom_center;
			
			if(cr_vec.x > 0)
				for(int i =0; i<cr_vec.x; i++)
				{
					if(!AABB(this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->x, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->y, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->w, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->h, cursor.x, cursor.y, 1, 1)
						&& !AABB(this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->x, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->y, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->w, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->h, cursor.x, cursor.y, 1, 1))
					{
						this->addChunk(cursor.x, cursor.y, 1, 1, 2);
						this->corridor_tiles++;
					}
					cursor.x++;
				}
			if(cr_vec.x < 0)
				for(int i =0; i<-cr_vec.x; i++)
				{
					if(!AABB(this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->x, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->y, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->w, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->h, cursor.x, cursor.y, 1, 1)
						&& !AABB(this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->x, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->y, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->w, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->h, cursor.x, cursor.y, 1, 1))
					{
						this->addChunk(cursor.x, cursor.y, 1, 1, 2);
						this->corridor_tiles++;
					}
					cursor.x--;

				}
			
			if(cr_vec.y > 0)
				for(int i =0; i<cr_vec.y; i++)
				{
					if(!AABB(this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->x, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->y, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->w, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->h, cursor.x, cursor.y, 1, 1)
						&& !AABB(this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->x, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->y, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->w, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->h, cursor.x, cursor.y, 1, 1))
					{
						this->addChunk(cursor.x, cursor.y, 1, 1, 2);
						this->corridor_tiles++;
					}
					cursor.y++;
				}
			if(cr_vec.y < 0)
				for(int i =0; i<-cr_vec.y; i++)
				{
					if(!AABB(this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->x, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->y, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->w, this->lvlchunks[this->triangulation->mst_tree[current_corridor].x]->h, cursor.x, cursor.y, 1, 1)
						&& !AABB(this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->x, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->y, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->w, this->lvlchunks[this->triangulation->mst_tree[current_corridor].y]->h, cursor.x, cursor.y, 1, 1))
					{
						this->addChunk(cursor.x, cursor.y, 1, 1, 2);
						this->corridor_tiles++;
					}
					cursor.y--;
				}

			this->current_corridor++; //next corridor
		}
	}

}

void Level::nextStep()
{
	if(this->onRoomGenerationFinished && !this->onRoomSeparation && !this->onRoomSeparationFinished)
	{
		//animate
		this->animation.t_Start();
		this->onRoomSeparation = 1;
		sprintf(mainEngine->ui->getText(mainEngine->log)->text + strlen(mainEngine->ui->getText(mainEngine->log)->text), "\n");

	}
	if(this->onRoomSeparationFinished && !this->onRoomClearing && !this->onRoomClearingFinished)
	{
		//animate
		this->animation.t_Start();
		this->onRoomClearing = 1;
	}

	if(this->onRoomClearingFinished && !this->onDelaunay && !this->onDelaunayFinished)
	{
		//animate
		this->animation.t_Start();
		this->onDelaunay = 1;
		for(int i =0; i<this->lvlchunks_num; i++)
		{
			int x = this->lvlchunks[i]->x + this->lvlchunks[i]->w/2;
			int y = this->lvlchunks[i]->y + this->lvlchunks[i]->h/2;
			this->triangulation->addPoint(this->lvlchunks[i]->x + this->lvlchunks[i]->w/2, this->lvlchunks[i]->y + this->lvlchunks[i]->h/2);
		}
		this->triangulation->generate();
		sprintf(mainEngine->ui->getText(mainEngine->log)->text + strlen(mainEngine->ui->getText(mainEngine->log)->text), "\n");
		
	}

	if(this->onDelaunayFinished && !this->onCorridor && !this->onCorridorFinished)
	{
		//animate
		this->animation.t_Start();
		this->onCorridor = 1;

	}

}



void Level::render()
{
	for(int i =0; i<this->lvlchunks_num; i++)
	{
		int x = this->lvlchunks[i]->x * 8;
		int y = this->lvlchunks[i]->y * 8;
		int w = this->lvlchunks[i]->w * 8;
		int h = this->lvlchunks[i]->h * 8;
		if(this->lvlchunks[i]->z == 1)
			mainEngine->renderQuad(x, y, w+1, h+1, 32, 127, 255);
		if(this->lvlchunks[i]->z == 2)
			mainEngine->renderQuad(x, y, w+1, h+1, 211, 187, 93);

		//mainEngine->renderQuad(x+1, y+1, w-1, h-1, 192, 170, 0);
		mainEngine->renderQuad(x+1, y+1, w-1, h-1, mainEngine->bg_r, mainEngine->bg_g, mainEngine->bg_b);

		
		//each vertical
		for(int j = 1; j<this->lvlchunks[i]->w; j++)
		{
			mainEngine->renderQuad(x + j*8, y + 1, 1, h - 1, 167, 191, 211); 
		}
		//each horizontal
		for(int j = 1; j<this->lvlchunks[i]->h; j++)
		{
			mainEngine->renderQuad(x + 1, y + j*8, w-1, 1, 167, 191, 211); 
		}
		
		if(this->onDelaunayFinished && !this->onCorridorFinished)
		{
			//this->triangulation->render(this->triangulation->mainTree);
			//+ render MST in green
			this->triangulation->renderMst();
		}
	}

}