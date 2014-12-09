#include "main.h"


Engine *mainEngine = new Engine(800, 480, -1.f);

int main( int argc, char* args[] )
{	
	if(mainEngine->initialize())
	{
		printf("Initialization failed!\n");
		return 1;
	}
	return mainEngine->work();
}