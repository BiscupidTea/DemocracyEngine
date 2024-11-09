#include "BaseGame/BaseGame.h"
#include "Entities/Sprite.h"
#include "Entities/Triangle.h"
#include "Entities/Square.h"


using namespace DemoEngine_BaseGame;
using namespace DemoEngine_Entities;

class EarthGame final : public BaseGame
{
private:
	vec3 Sposition;
	vec3 Sscale;
	vec3 Srotation;
	vec4 Scolor;

	vec3 Tposition;
	vec3 Tscale;
	vec3 Trotation;
	vec4 Tcolor;

	Square* square;

	Triangle* triangle;

	Sprite* image;

	DemoEngine_Animations::DemoTimer* timer;

public:
	EarthGame();
	~EarthGame();

	void Init() override;
	void Update() override;
	void DeInit() override;
};