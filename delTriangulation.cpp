#include "delTriangulation.h"
#include "main.h"

delTriangulation::delTriangulation()
{
	this->points_num = 0;
	this->points = (Point2d*) malloc(sizeof(Point2d) * this->points_num);

	this->current_mst = (bool*) malloc(sizeof(bool) * this->points_num);
	this->priority_queue_num = 0;
	this->priority_queue = (Point2d*) malloc(sizeof(Point2d) * this->priority_queue_num);

	this->mst_tree_num = 0;
	this->mst_tree = (Point2d*) malloc(sizeof(Point2d) * this->mst_tree_num);

	this->mainTree = new Triangulation();
	this->highest_level = 0;
	this->simpleTriangulations = 0;
}
delTriangulation::~delTriangulation()
{
	delete this->points;
	delete this->mainTree;
	delete this->current_mst;
	delete this->priority_queue;
	delete this->mst_tree;

}

void delTriangulation::addPoint(int x, int y)
{
	this->points_num++;
	this->points = (Point2d*) realloc(this->points, sizeof(Point2d) * this->points_num);
	this->points[this->points_num-1].x = x;
	this->points[this->points_num-1].y = y;
}

Vec2d Vec2d::operator+(const Vec2d &vec) const
{
	return Vec2d(x + vec.x, y + vec.y);
}

Vec2d Vec2d::operator-(const Vec2d &vec) const
{
	return Vec2d(x - vec.x, y - vec.y);
}

Vec2d Vec2d::operator*(float num) const
{
	return Vec2d(x*num, y*num);
}

float Vec2d::operator*(const Vec2d &vec) const
{
	return x*vec.x + y*vec.y;
}

int Point2d::operator*(const Point2d &pt) const
{
	return x*pt.x + y*pt.y;
}

void delTriangulation::generate()
{	
	//copy
	this->mainTree->pids_num = this->points_num;
	this->mainTree->pids = (int*) realloc(this->mainTree->pids, sizeof(int) * this->mainTree->pids_num);
	
	int most_left= 0, most_right= 0;
	
	for(int i =0; i<this->points_num; i++)
	{
		this->mainTree->pids[i] = i;
		if(this->points[i].x < this->points[most_left].x)
			most_left = i;
		if(this->points[i].x > this->points[most_right].x)
			most_right = i;
		
	}

	this->mainTree->most_left = most_left;
	this->mainTree->most_right = most_right;
}

Point2d delTriangulation::lowerTangent(Triangulation *left, Triangulation *right)
{
	//find most left and right global
	int xGlob = left->most_right;
	int yGlob = right->most_left;
	int x = 0;
	int y = 0;
	//localize for ++ and --
	for(int i =0; i<left->hull_num; i++)
	{
		if(left->hull[i] == xGlob)
		{
			x = i;
			break;
		}

	}

	for(int i =0; i<right->hull_num; i++)
	{
		if(right->hull[i] == yGlob)
		{
			y = i;
			break;
		}
	}

	//initials
	int l = (x-1);
	if(l < 0)
		l = left->hull_num + l; //limit
	int r = (y+1) % right->hull_num; //limit

	bool done = 0;
	while(!done)
	{
		if(ConvexCross(this->points[right->hull[y]], this->points[left->hull[x]], this->points[left->hull[l]]) > 0.0)
		{
			x--;
			if(x < 0)
				x = left->hull_num + x;
			l = (x-1);
			if(l < 0)
				l = left->hull_num + l; //limit
		}
		
		else
			if(ConvexCross(this->points[right->hull[y]], this->points[left->hull[x]], this->points[right->hull[r]]) > 0.0)
			{
				y = (y + 1) % right->hull_num;
				r = (y + 1) % right->hull_num;
			}
			else
				done = 1;

	}

	Point2d tangent; //local positions in hulls
	tangent.x = x;
	tangent.y = y;

	return tangent;
}

Point2d delTriangulation::upperTangent(Triangulation *left, Triangulation *right) //same as lower
{
	//find most left and right global
	int xGlob = left->most_right;
	int yGlob = right->most_left;
	int x = 0;
	int y = 0;
	//localize for ++ and --
	for(int i =0; i<left->hull_num; i++)
	{
		if(left->hull[i] == xGlob)
		{
			x = i;
			break;
		}

	}

	for(int i =0; i<right->hull_num; i++)
	{
		if(right->hull[i] == yGlob)
		{
			y = i;
			break;
		}
	}

	//initials
	int l = (x+1) % left->hull_num;

	int r = (y-1); //limit
	if(r <0)
		r = right->hull_num + r;

	bool done = 0;
	while(!done)
	{
		if(ConvexCross(this->points[right->hull[y]], this->points[left->hull[x]], this->points[left->hull[l]]) < 0.0)
		{
			x = (x+1) % left->hull_num;
			l = (x+1) % left->hull_num;
		}
		
		else
			if(ConvexCross(this->points[right->hull[y]], this->points[left->hull[x]], this->points[right->hull[r]]) < 0.0)
			{
				y--;
				if(y < 0)
					y = right->hull_num + y;
				r = (y-1);
				if(r < 0)
					r = right->hull_num + r;
			}
			else
				done = 1;

	}

	Point2d tangent; //local positions in hulls
	tangent.x = x;
	tangent.y = y;

	return tangent;
}


void delTriangulation::devide(Triangulation *tri)
{
	if(tri->pids_num <= 3)
	{
		//simple triangulation!
		tri->complete = 1;
		this->simpleTriangulations++;
		
		if(tri->pids_num>0)
		{
			//build simple convex hull + sort it CCW (counter clockwise)
			//find top most point
			int max_point = 0; //local id
			for(int i =0; i<tri->pids_num; i++)
			{
				if(this->points[tri->pids[i]].y > this->points[tri->pids[max_point]].y)
					max_point = i;
			}
			//add point
			tri->hull_num++;
			tri->hull = (int*) realloc(tri->hull, sizeof(int) * tri->hull_num);
			tri->hull[0] = tri->pids[max_point]; //global id
			//add other points exept max_point 
			for(int i =0; i<tri->pids_num; i++)
			{
				if(i == max_point)
					continue;
				tri->hull_num++;
				tri->hull = (int*) realloc(tri->hull, sizeof(int) * tri->hull_num);
				tri->hull[tri->hull_num-1] = tri->pids[i]; //global
			}
			
			//3 points CCW order fix!
			if(tri->pids_num == 3)
			{
				bool flip = 0;
				if(ConvexCross(this->points[tri->hull[0]], this->points[tri->hull[1]], this->points[tri->hull[2]]) < 0.0)
					flip = !flip;
				if(this->points[tri->hull[0]].x == this->points[tri->hull[1]].x && this->points[tri->hull[1]].x == this->points[tri->hull[2]].x &&
					this->points[tri->hull[1]].y < this->points[tri->hull[2]].y)
					flip = !flip;

				if(flip)
				{
					//swap
					int tmp = tri->hull[1];
					tri->hull[1] = tri->hull[2];
					tri->hull[2] = tmp;
				}
			}

			//Create edges 
			//connect top point using hull data, because it's sorted CCW
			for(int i =1; i<tri->pids_num; i++)
			{
				tri->edges_num++;
				tri->edges = (Point2d*) realloc(tri->edges, sizeof(Point2d) * tri->edges_num);
				tri->edges[tri->edges_num-1] = Point2d(tri->hull[0], tri->hull[i]);
			}
			if(tri->pids_num == 3)
			{
				tri->edges_num++;
				tri->edges = (Point2d*) realloc(tri->edges, sizeof(Point2d) * tri->edges_num);
				tri->edges[tri->edges_num-1] = Point2d(tri->hull[1], tri->hull[2]);
			}

		}

		return;
	}
	else
	{
		tri->left = new Triangulation();
		tri->left->side = 0; //left
		tri->right = new Triangulation();
		tri->right->side = 1;
		
		int border = this->points[tri->most_right].x/2 + this->points[tri->most_left].x/2;
		int most_left_left = -1, most_right_left = -1;
		int most_left_right = -1, most_right_right = -1;

		for(int i =0; i<tri->pids_num; i++)
		{
			if(this->points[tri->pids[i]].x <= border)
			{
				if(most_left_left == -1)
					most_left_left = tri->pids[i];
				if(most_right_left == -1)
					most_right_left = tri->pids[i];

				if(this->points[tri->pids[i]].x < this->points[most_left_left].x)
					most_left_left = tri->pids[i];
				
				if(this->points[tri->pids[i]].x > this->points[most_right_left].x)
					most_right_left = tri->pids[i];

				//add those points to next left layer
				if(tri->left->pids_num == 0)
				{
					tri->left->pids_num++;
					tri->left->pids = (int*) malloc(sizeof(int) * tri->left->pids_num);
					tri->left->pids[tri->left->pids_num-1] = tri->pids[i];
				}
				else
				{
					tri->left->pids_num++;
					tri->left->pids = (int*) realloc(tri->left->pids, sizeof(int) * tri->left->pids_num);
					tri->left->pids[tri->left->pids_num-1] = tri->pids[i];
				}
			}
			else
			{
				if(most_left_right == -1)
					most_left_right = tri->pids[i];
				if(most_right_right == -1)
					most_right_right = tri->pids[i];
				
				if(this->points[tri->pids[i]].x < this->points[most_left_right].x)
					most_left_right = tri->pids[i];
				
				if(this->points[tri->pids[i]].x > this->points[most_right_right].x)
					most_right_right = tri->pids[i];

				//add those points to next left layer
				if(tri->right->pids_num == 0)
				{
					tri->right->pids_num++;
					tri->right->pids = (int*) malloc(sizeof(int) * tri->right->pids_num);
					tri->right->pids[tri->right->pids_num-1] = tri->pids[i];
				}
				else
				{
					tri->right->pids_num++;
					tri->right->pids = (int*) realloc(tri->right->pids, sizeof(int) * tri->right->pids_num);
					tri->right->pids[tri->right->pids_num-1] = tri->pids[i];
				}
			}

		}
		tri->left->most_left = most_left_left;
		tri->left->most_right = most_right_left;
		tri->right->most_left = most_left_right;
		tri->right->most_right = most_right_right;

		this->devide(tri->left);
		this->devide(tri->right);
		if(tri->left->complete && tri->right->complete)
		{
			tri->lower_tan = this->lowerTangent(tri->left, tri->right);
			tri->upper_tan = this->upperTangent(tri->left, tri->right);
			this->merge(tri);
		}

		tri->devided = 1;

	}
	return;

}

void delTriangulation::merge(Triangulation *tri)
{
	Point2d lowerTan = this->lowerTangent(tri->left, tri->right);
	Point2d upperTan = this->upperTangent(tri->left, tri->right);

	//Convex Hull merging
	//take from left side first
	if(tri->left->hull_num > 0)
	{
		for(int p = upperTan.x; p!= lowerTan.x; p= (p+1)%tri->left->hull_num)
		{
			tri->hull_num++;
			tri->hull = (int*) realloc(tri->hull, sizeof(int) * tri->hull_num);
			tri->hull[tri->hull_num-1] = tri->left->hull[p]; //global
		}
		//add lowerTan left side
		tri->hull_num++;
		tri->hull = (int*) realloc(tri->hull, sizeof(int) * tri->hull_num);
		tri->hull[tri->hull_num-1] = tri->left->hull[lowerTan.x]; //global

	}
	//from right side
	if(tri->right->hull_num>0)
	{
		for(int p = lowerTan.y; p!= upperTan.y; p= (p+1)%tri->right->hull_num)
		{
			tri->hull_num++;
			tri->hull = (int*) realloc(tri->hull, sizeof(int) * tri->hull_num);
			tri->hull[tri->hull_num-1] = tri->right->hull[p]; //global
		}
		//final upperTan right side
		tri->hull_num++;
		tri->hull = (int*) realloc(tri->hull, sizeof(int) * tri->hull_num);
		tri->hull[tri->hull_num-1] = tri->right->hull[upperTan.y]; //global

	}

	//#Triangulation merge#
	
	Point2d baseEdge = Point2d(tri->left->hull[lowerTan.x], tri->right->hull[lowerTan.y]); //global
	Point2d upperEdge = Point2d(tri->left->hull[upperTan.x], tri->right->hull[upperTan.y]); //global

	tri->edges_num++;
	tri->edges = (Point2d*) realloc(tri->edges, sizeof(Point2d) * tri->edges_num);
	tri->edges[tri->edges_num-1] = baseEdge;

	bool done = 0;

	while(!done)
	{
	

		bool candidate_found = 0;

		//--------------------
		//Right side candidate
		//--------------------

		int right_candidate = -1;
		int right_next_candidate = -1;
		int right_candidate_edge = -1;
		Vec2d right_candidate_center;
		float right_candidate_radius_sq = 0.f;

		float cand_cot = 0.f;
		float next_cand_cot = 0.f;

		while(!candidate_found)
		{
			right_candidate = -1;
			right_next_candidate = -1;
			right_candidate_edge = -1;

			for(int i =0; i<tri->right->edges_num; i++)
			{
				int pt = 0;
				if(tri->right->edges[i].y == baseEdge.y)
					pt = tri->right->edges[i].x;
				else
					if(tri->right->edges[i].x == baseEdge.y)
						pt = tri->right->edges[i].y;
					else
						continue;  //not in current edge

				Point2d ptRel = this->points[pt];
				ptRel.x -= this->points[baseEdge.y].x;
				ptRel.y -= this->points[baseEdge.y].y;

				Point2d xRel = this->points[baseEdge.x];
				xRel.x -= this->points[baseEdge.y].x;
				xRel.y -= this->points[baseEdge.y].y;

				float cSin = CrossPoint2d(xRel, ptRel);
			
				//sould be going upward
				if(cSin > 0.0)
				{
					continue;
				}

				if(right_candidate == -1)
				{
					right_candidate = pt; //global
					cand_cot = xRel * ptRel / CrossPoint2d(xRel, ptRel);
					right_candidate_edge = i;
					continue;
				}

				if(cSin == 0.0)
				{
					if(right_next_candidate == -1) //final candidate
						right_next_candidate = pt;
					continue;
				}

				float cCos = xRel * ptRel;

				if(cCos / cSin < cand_cot)
				{
					right_next_candidate = right_candidate;
					next_cand_cot = cand_cot;
			
					cand_cot = cCos / cSin;
					right_candidate = pt; //global
					right_candidate_edge = i; //local
				}
				else
				{
					//next candidate
					if(right_next_candidate == -1)
					{
						right_next_candidate = pt;
						next_cand_cot = cCos / cSin;
					}
					else
						if(cCos / cSin < next_cand_cot)
						{
							right_next_candidate = pt;
							next_cand_cot = cCos / cSin;
						}
				}

			}

			//1 point fix
			if(right_candidate == -1 && tri->right->pids_num == 1)
			{
				//right_candidate = tri->right->pids[0];
				candidate_found = 1;
			}

			if(right_next_candidate == -1)
			{
				//right candidate
				candidate_found = 1;
			}
	
			//circumcicle check
			right_candidate_center = Circumcenter2d(this->points[baseEdge.x], this->points[baseEdge.y], this->points[right_candidate]);

			Vec2d next = Vec2d(this->points[right_next_candidate].x, this->points[right_next_candidate].y);
			Vec2d baseVec = Vec2d(this->points[baseEdge.y].x, this->points[baseEdge.y].y);
			baseVec = baseVec - right_candidate_center;
			next = next - right_candidate_center;
		
			right_candidate_radius_sq = baseVec * baseVec;
			if(right_candidate_radius_sq < next * next)
			{
				//right candidate
				candidate_found = 1;
			}

	
			//delete candidate edge
			if(tri->right->edges_num > 0 && !candidate_found)
			{
				tri->right->edges[right_candidate_edge] = tri->right->edges[tri->right->edges_num-1];
				tri->right->edges_num--;
				tri->right->edges = (Point2d*) realloc(tri->right->edges, sizeof(Point2d) * tri->right->edges_num);
			}

		}

		//-------------------
		//Left side candidate
		//-------------------

		candidate_found = 0;
		int left_candidate = -1;
		int left_next_candidate = -1;
		int left_candidate_edge = -1;
		Vec2d left_candidate_center;
		float left_candidate_radius_sq = 0.f;
	
		while(!candidate_found)
		{
			left_candidate = -1;
			left_next_candidate = -1;
			left_candidate_edge = -1;

			for(int i =0; i<tri->left->edges_num; i++)
			{
				int pt = 0;
				if(tri->left->edges[i].y == baseEdge.x)
					pt = tri->left->edges[i].x;
				else
					if(tri->left->edges[i].x == baseEdge.x)
						pt = tri->left->edges[i].y;
					else
						continue;  //not in current edge

				Point2d ptRel = this->points[pt];
				ptRel.x -= this->points[baseEdge.x].x;
				ptRel.y -= this->points[baseEdge.x].y;

				Point2d yRel = this->points[baseEdge.y];
				yRel.x -= this->points[baseEdge.x].x;
				yRel.y -= this->points[baseEdge.x].y;

				float cSin = CrossPoint2d(yRel, ptRel);
				
				//sould be going upward
				if(cSin < 0.0)
				{
					continue;
				}

				if(left_candidate == -1)
				{
					left_candidate = pt; //global
					cand_cot = yRel * ptRel / cSin;
					left_candidate_edge = i;
					continue;
				}

				if(cSin == 0.0)
				{
					if(left_next_candidate == -1) //final candidate
						left_next_candidate = pt;
					continue;
				}

				float cCos = yRel * ptRel;

				if(cCos / cSin > cand_cot)
				{
					left_next_candidate = left_candidate;
					next_cand_cot = cand_cot;
			
					cand_cot = cCos / cSin;
					left_candidate = pt; //global
					left_candidate_edge = i; //local
				}
				else
				{
					//next candidate
					if(left_next_candidate == -1)
					{
						left_next_candidate = pt;
						next_cand_cot = cCos / cSin;
					}
					else
						if(cCos / cSin > next_cand_cot)
						{
							left_next_candidate = pt;
							next_cand_cot = cCos / cSin;
						}
				}

			}

			//1 point fix
			if(left_candidate == -1 && tri->left->pids_num == 1)
			{
				//left_candidate = tri->left->pids[0];
				candidate_found = 1;
			}

			if(left_next_candidate == -1)
			{
				//left candidate
				candidate_found = 1;
			}
	
			//circumcicle check
			left_candidate_center = Circumcenter2d(this->points[baseEdge.x], this->points[baseEdge.y], this->points[left_candidate]);

			Vec2d next = Vec2d(this->points[left_next_candidate].x, this->points[left_next_candidate].y);
			Vec2d baseVec = Vec2d(this->points[baseEdge.x].x, this->points[baseEdge.x].y);
			baseVec = baseVec - left_candidate_center;
			next = next - left_candidate_center;

			left_candidate_radius_sq = baseVec * baseVec;
			if(left_candidate_radius_sq < next * next)
			{
				//right candidate
				candidate_found = 1;
			}

	
			//delete candidate edge
			if(tri->left->edges_num > 0 && !candidate_found)
			{
				tri->left->edges[left_candidate_edge] = tri->left->edges[tri->left->edges_num-1];
				tri->left->edges_num--;
				tri->left->edges = (Point2d*) realloc(tri->left->edges, sizeof(Point2d) * tri->left->edges_num);
			}

		}

		//------------------
		//Left or right/ Adding edges
		//------------------
		Vec2d rightVec = Vec2d(this->points[right_candidate].x, this->points[right_candidate].y) - left_candidate_center;
		if((right_candidate == -1 ||  left_candidate_radius_sq < rightVec * rightVec) && left_candidate != -1)
		{
			//add from left to right
			Point2d newEdge = Point2d(left_candidate, baseEdge.y);
			tri->edges_num++;
			tri->edges = (Point2d*) realloc(tri->edges, sizeof(Point2d) * tri->edges_num);
			tri->edges[tri->edges_num-1] = newEdge;
			baseEdge = newEdge; //next!

			if(newEdge.x == upperEdge.x && newEdge.y == upperEdge.y)
				done = 1;

		}
		else
		{
			if(right_candidate != -1)
			{
				//add from right to left
				Point2d newEdge = Point2d(baseEdge.x, right_candidate);
				tri->edges_num++;
				tri->edges = (Point2d*) realloc(tri->edges, sizeof(Point2d) * tri->edges_num);
				tri->edges[tri->edges_num-1] = newEdge;
				baseEdge = newEdge; //next!

				if(newEdge.x == upperEdge.x && newEdge.y == upperEdge.y)
					done = 1;
			}
			else
			{
				//add base!
				Point2d newEdge = Point2d(baseEdge.x, baseEdge.y);
				tri->edges_num++;
				tri->edges = (Point2d*) realloc(tri->edges, sizeof(Point2d) * tri->edges_num);
				tri->edges[tri->edges_num-1] = newEdge;
				baseEdge = newEdge; //next!
				
				//if(newEdge.x == upperEdge.x && newEdge.y == upperEdge.y) 
				//just exit
					done = 1;

			}
		}
	}
	//Done adding merge enges
	//Add existing ones
	for(int i = 0; i<tri->left->edges_num; i++)
	{
		tri->edges_num++;
		tri->edges = (Point2d*) realloc(tri->edges, sizeof(Point2d) * tri->edges_num);
		tri->edges[tri->edges_num-1] = tri->left->edges[i];
	}
	for(int i =0; i<tri->right->edges_num; i++)
	{
		tri->edges_num++;
		tri->edges = (Point2d*) realloc(tri->edges, sizeof(Point2d) * tri->edges_num);
		tri->edges[tri->edges_num-1] = tri->right->edges[i];
	}


	//tri->left_candidate = left_candidate;
	//tri->right_candidate = right_candidate;

	tri->complete = 1;
}

int queue(Point2d *a, Point2d *b)
{
	if(a->y > b->y)
		return 1;
	if(a->y < b->y)
		return -1;
	return 0;
}

int delTriangulation::step()
{
	//devide all poinst recursivly and build a delaunay
	this->devide(this->mainTree);
	
	//build an MST 
	//#REMOVE IN ACTUAL GAME!!!!#
	if(this->points_num > 0)
	{
		this->current_mst = (bool*) realloc(this->current_mst, sizeof(bool) * this->points_num); //resize
		int current_point = 0; //starting point
		bool completed = 0;
		memset(this->current_mst, 0, sizeof(bool) * this->points_num);
		this->current_mst[0] = 1; //added it

		while(!completed)
		{
			//find all edges and add them
			for(int i =0; i<this->mainTree->edges_num; i++)
			{
				if(this->mainTree->edges[i].x == current_point)
				{
					if(!this->current_mst[this->mainTree->edges[i].y])
					{
						//add edge
						this->priority_queue_num++;
						this->priority_queue = (Point2d*) realloc(this->priority_queue, sizeof(Point2d) * this->priority_queue_num);
						Point2d edge = this->points[this->mainTree->edges[i].y];
						edge.x -= this->points[this->mainTree->edges[i].x].x;
						edge.y -= this->points[this->mainTree->edges[i].x].y;

						this->priority_queue[this->priority_queue_num-1] = Point2d(i, edge*edge);
					}
				}
				if(this->mainTree->edges[i].y == current_point)
				{
					if(!this->current_mst[this->mainTree->edges[i].x])
					{
						//add edge
						this->priority_queue_num++;
						this->priority_queue = (Point2d*) realloc(this->priority_queue, sizeof(Point2d) * this->priority_queue_num);
						Point2d edge = this->points[this->mainTree->edges[i].y];
						edge.x -= this->points[this->mainTree->edges[i].x].x;
						edge.y -= this->points[this->mainTree->edges[i].x].y;
						this->priority_queue[this->priority_queue_num-1] = Point2d(i, edge*edge);
					}
				}

			}
			//clean priority queue
			for(int i =0; i<this->priority_queue_num; i++)
			{
				if(this->current_mst[this->mainTree->edges[this->priority_queue[i].x].x] && this->current_mst[this->mainTree->edges[this->priority_queue[i].x].y])
				{
					this->priority_queue[i] = this->priority_queue[this->priority_queue_num-1];
					this->priority_queue_num --;
					this->priority_queue = (Point2d*) realloc(this->priority_queue, sizeof(Point2d) * this->priority_queue_num);
					i--;
				}
			}

			//add minimal to MST
			int min_entry = 0;
			for(int i =0; i<this->priority_queue_num; i++)
				if(this->priority_queue[i].y < this->priority_queue[min_entry].y)
					min_entry = i;
			//add
			this->mst_tree_num++;
			this->mst_tree = (Point2d*) realloc(this->mst_tree, sizeof(Point2d) * this->mst_tree_num);
			this->mst_tree[this->mst_tree_num-1] = this->mainTree->edges[this->priority_queue[min_entry].x];

			//next point is not already added one
			if(!this->current_mst[this->mst_tree[this->mst_tree_num-1].x])
				current_point = this->mst_tree[this->mst_tree_num-1].x;
			else
				current_point = this->mst_tree[this->mst_tree_num-1].y;


			//make sure points are added
			this->current_mst[this->mst_tree[this->mst_tree_num-1].x] = 1;
			this->current_mst[this->mst_tree[this->mst_tree_num-1].y] = 1;

			//end of loop
			completed = this->current_mst[0];
			for(int i =0; i<this->points_num; i++)
				completed = completed && this->current_mst[i];


		}
		//MST building done!
	}
	return 1;
}

void delTriangulation::render(Triangulation *tri)
{
	if(tri->complete)
	{
		for(int i =0; i < tri->edges_num; i++)
		{
			int x1 = this->points[tri->edges[i].x].x * 8;
			int y1 = this->points[tri->edges[i].x].y * 8;
			int x2 = this->points[tri->edges[i].y].x * 8;
			int y2 = this->points[tri->edges[i].y].y * 8;
			
			mainEngine->renderLine(x1, y1, x2, y2, 240, 240, 0);
		}

		for(int i =0; i < tri->hull_num; i++)
		{
			int x1 = this->points[tri->hull[i]].x * 8;
			int y1 = this->points[tri->hull[i]].y * 8;
			int x2 = 0;
			int y2 = 0;
			mainEngine->renderQuad(x1-1, y1-1, 3, 3, 0, 240, 0);
			
			if(i == tri->hull_num-1)
			{
				x2 = this->points[tri->hull[0]].x * 8;
				y2 = this->points[tri->hull[0]].y * 8;
			}
			else
			{
				x2 = this->points[tri->hull[i+1]].x * 8;
				y2 = this->points[tri->hull[i+1]].y * 8;
			}

			mainEngine->renderLine(x1, y1, x2, y2, 240, 240, 240);
		}
		return;
	}
	else
	{
		this->render(tri->left);
		this->render(tri->right);
		return;
	}
	return;
}

void delTriangulation::renderMst()
{
	for(int i =0; i<this->mst_tree_num; i++)
	{
		int x1 = this->points[this->mst_tree[i].x].x * 8;
		int y1 = this->points[this->mst_tree[i].x].y * 8;
		int x2 = this->points[this->mst_tree[i].y].x * 8;
		int y2 = this->points[this->mst_tree[i].y].y * 8;
			
		mainEngine->renderLine(x1, y1, x2, y2, 240, 240, 0);

	}

	for(int i =0; i<this->points_num; i++)
	{
		int x1 = this->points[i].x * 8;
		int y1 = this->points[i].y * 8;
		mainEngine->renderQuad(x1 -1, y1 -1, 3, 3, 0, 240, 0);
	}
	
}

float ConvexCross(Point2d p1, Point2d p2, Point2d p3)
{
	return (p2.x - p1.x)*(p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

Vec2d Circumcenter2d(Point2d a, Point2d b, Point2d c)
{
	//pure math!
	Vec2d center;
	float dA = a.x * a.x + a.y * a.y;
	float dB = b.x * b.x + b.y * b.y;
	float dC = c.x * c.x + c.y * c.y;

	float bt = 2*(a.x * (c.y - b.y) + b.x * (a.y - c.y) + c.x * (b.y - a.y));
	center.x = (dA * (c.y - b.y) + dB * (a.y - c.y) + dC * (b.y - a.y))/bt;
	center.y = -(dA * (c.x - b.x) + dB * (a.x - c.x) + dC * (b.x - a.x))/bt;

	return center;
}

float CrossPoint2d(Point2d p1, Point2d p2)
{
	return (p1.x * p2.y - p1.y * p2.x);
}