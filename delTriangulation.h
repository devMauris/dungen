#ifndef DELTRIANGULATION_H
#define DELTRIANGULATION_H

#include <stdlib.h>

struct Point2d
{
	int x, y;
	Point2d(int px = 0, int py = 0)
	{
		x = px;
		y = py;
	}
	int operator * (const Point2d &pt) const;
};

struct Vec2d
{
	float x,y;
	Vec2d(float px = 0.f, float py = 0.f)
	{
		x = px;
		y = py;
	}
	Vec2d operator + (const Vec2d &vec) const;
	Vec2d operator - (const Vec2d &vec) const;
	Vec2d operator * (float num) const;
	float operator * (const Vec2d &vec) const; //dot
};

extern float ConvexCross(Point2d p1, Point2d p2, Point2d p3);
extern float CrossPoint2d(Point2d p1, Point2d p2);
extern Vec2d Circumcenter2d(Point2d a, Point2d b, Point2d c);

struct Triangulation
{
	//points
	int pids_num;
	int *pids;

	int hull_num;
	int *hull;

	int edges_num;
	Point2d *edges;

	bool side; //left 0 / right 1
	bool complete;
	bool devided;
	int lowest_point;
	int most_left, most_right;

	Triangulation *left, *right;
	
	Point2d lower_tan;
	Point2d upper_tan;
	int left_candidate, right_candidate;
	
	Triangulation()
	{
		pids_num = 0;
		pids = (int*) malloc(sizeof(int) * pids_num);
		hull_num = 0;
		hull = (int*) malloc(sizeof(int) * hull_num);
		edges_num = 0;
		edges = (Point2d*) malloc(sizeof(Point2d) * edges_num);
		side = 0;
		lowest_point = 0;
		devided = 0;
		left = NULL;
		right = NULL;
		complete = 0;
		most_left = 0;
		most_right = 0;

		left_candidate = 0;
		right_candidate = 0;

	}
	~Triangulation() //do not forget
	{
		delete pids;
		delete hull;
		delete edges;
		delete left;
		delete right;
	}
};

class delTriangulation
{
	private:
		int points_num;
		Point2d *points; //small enough

		bool *current_mst; //array of added points
		
		int priority_queue_num; //priority queue
		Point2d *priority_queue; // x - edge id, y - weight (sq length)


		int highest_level;
		void devide(Triangulation *tri);
		void merge(Triangulation *tri);
		Point2d lowerTangent(Triangulation *left, Triangulation *right);
		Point2d upperTangent(Triangulation *left, Triangulation *right);



	public:
		int mst_tree_num; //array of final edges!
		Point2d *mst_tree;

		Triangulation *mainTree;
		int simpleTriangulations;

		delTriangulation();
		~delTriangulation();
		void addPoint(int x, int y);
		void generate();
		int step();
		void render(Triangulation *tri);
		void renderMst();

};


#endif