#include "GLCore.h"
#include "CollisionDetection.h"

using namespace GLCore;

class CollisionDetectionApp : public Application
{
public:
	CollisionDetectionApp()
		: Application("CollisionDetection")
	{
		PushLayer(new CollisionDetection());
	}
};

int main()
{
	std::unique_ptr<CollisionDetectionApp> app = std::make_unique<CollisionDetectionApp>();
	app->Run();
}