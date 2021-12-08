
#include <glew.h>
#include <stdio.h>

#include "core\Application.h"

#include "entities\Entity.h"
#include "entities\terrain\Terrain.h"
#include "GUI\GUIImage.h"
#include "GUI\Text.h"
#include "graphics\graphicsComponents\lights\DirectionalLight.h"

#include "graphics\graphicsResources\Model.h"
#include "graphics\graphicsUtils\ModelLoading.h"
#include "animations\skeletalAnimation\Skeleton.h"
#include "animations\skeletalAnimation\Animation.h"

#include "utils\Batching.h"

#include "core\Debug.h"


using namespace lightEngine;
using namespace graphics;
using namespace modelLoading;


class TestEvent : public InputEvent
{
public:

	bool triggeredSceneSwitch = false;

	static double mouseX, mouseY;

	TestEvent() :
		InputEvent(INPUT_EVENT_TYPE__KEY | INPUT_EVENT_TYPE__CURSOR_POSITION)
	{
	}
	virtual ~TestEvent()
	{}

	// Reason, why these aren't pure virtual, is that we don't need to always write a definition 
	// for a callback if we aren't even going to do anything in that function
	virtual void keyCallback(int key, int scancode, int action, int mods) override
	{
		if (key == GLFW_KEY_R)
			triggeredSceneSwitch = true;

		if (key == GLFW_KEY_ESCAPE)
			Application::exit();
	}

	virtual void cursorPosCallback(double x, double y) override
	{
		const double& windowHeight = Window::get_height();
		//printf("mouse X = %f	mouse Y = %f\n", x, windowHeight - y);
		
		mouseX = x;
		mouseY = windowHeight - y;

	}
};

double TestEvent::mouseX = 0;
double TestEvent::mouseY = 0;


class TestEntity : Entity
{
public:

	TestEntity(graphics::Texture* texture, float startX, float startY)
	{
		// Make randomly flat colored entities..
		int useTexture = std::rand() % 3;
		graphics::Texture* usedTex = nullptr;
		if (useTexture != 1)
			usedTex = texture;

		gui::GUIImage* guiImg = new gui::GUIImage(usedTex);
		addComponent(guiImg);

		float w = 0.02125f;
		float h = 0.0218f;
		mml::Vec3 scale(w, h, 0);
		mml::Vec3 pos((-1.0f + w) + scale.x * 2 * startX, (1.0f - h) - scale.y * 2 * startY, 0);
		_transform.setPosition(pos);
		_transform.setScale(scale);

		guiImg->setColor(mml::Vec4(1, 1, 1, 1));
	}

	virtual void update() override
	{
		gui::GUIImage* img = _components_guiImage[0];

		float r = (float)(std::rand() % 100) * 0.01f;
		float g = (float)(std::rand() % 100) * 0.01f;
		float b = (float)(std::rand() % 100) * 0.01f;

		img->setColor(mml::Vec4(r, g, b, 1.0f));
	}
};



class TestScene : public Scene
{
public:

	ImageData* testImg = nullptr;
	graphics::Texture* testTexture = nullptr;

	TestEvent _testEvent;

	gui::Text* text_fps = nullptr;

	Entity* textEntity = nullptr;

	Entity* testModelEntity;

	SkinnedRenderable3D* skinnedRenderable;
	Entity* skinnedTestEntity;
	
	Mesh* testMesh3D;


	Entity* terrainTest;
	
	virtual ~TestScene()
	{
	}

	virtual void init() override 
	{
		_testEvent.addToInputHandler();
		
		testImg = ResourceManager::load_resource__image("res/TestImage.png", "TestImage");
		testTexture = ResourceManager::create_resource__texture(testImg, "TestImageTexture", graphics::TextureFilteringType::Linear, graphics::TextureImageMapping::Repeat);
		

		printf("TestScene initialized!\n");

		unsigned int width = 1;
		unsigned int height = 1;

		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				TestEntity* e = new TestEntity(testTexture, i, j);
				addToScene((Entity*)e);
			}
		}
		/*
		// Testing text rendering.. 
		// reading file holding a long string of text -> then we draw that text on screen
		
		FILE* f = nullptr;
		fopen_s(&f, "res/TestTextContent.txt", "rt");

		fseek(f, 0, SEEK_END);

		int fileLength = ftell(f);

		fseek(f, 0, SEEK_SET);

		int txtLength = fileLength;
		char* txt = new char[txtLength];
		memset(txt, 0, txtLength);
		fread_s(txt, txtLength, sizeof(char), txtLength, f);

		fclose(f);

		
		textEntity = new Entity;
		textEntity->getTransform().setPosition(mml::Vec3(20, 384 - (0 * 0.05f), 0));
		
		gui::Text* testText = new gui::Text(txt, 0.15625f, 100);
		textEntity->addComponent(testText);

		addToScene(textEntity);
		*/


		// FPS text
		Entity* entity_fpsText = new Entity;
		entity_fpsText->getTransform().setPosition(mml::Vec3(20, 25, 0));

		text_fps = new gui::Text("FPS :", 0.125f, mml::Vec4(1,1,0,1));
		entity_fpsText->addComponent(text_fps);

		addToScene(entity_fpsText);

		/*
		// pos, texCoord, normal
		float vertices[] =
		{
			-1.0f, 1.0f, 0,		0,1,	0,0,1,
			-1.0f, -1.0f, 0,	0,0,	0,0,1,
			1.0f, 1.0f, 0,		1,1,	0,0,1,
			1.0f, -1.0f, 0,		1,0,	0,0,1
		};

		unsigned int indices[] =
		{
			0,1,2,3
		};


		IndexBuffer* ib = IndexBuffer::create(indices, 4, BufferUsage::StaticDraw);
		VertexBuffer* vb_vertex = VertexBuffer::create(vertices, 32, BufferUsage::StaticDraw);
		VertexBuffer* vb_instanced = VertexBuffer::create(nullptr, 16 * 10, BufferUsage::DynamicDraw);

		VertexBufferLayout layout_vertexPositions(
			{
				{ ShaderDataType::Float3, "position" },
				{ ShaderDataType::Float2, "texCoord" },
				{ ShaderDataType::Float3, "normal" }
			}
		);

		VertexBufferLayout layout_instanced(
			{
				{ ShaderDataType::Float4, "transformationMatrix_row1", 1 },
				{ ShaderDataType::Float4, "transformationMatrix_row2", 1 },
				{ ShaderDataType::Float4, "transformationMatrix_row3", 1 },
				{ ShaderDataType::Float4, "transformationMatrix_row4", 1 }
			}
		);


		vb_vertex->setLayout(layout_vertexPositions);
		vb_instanced->setLayout(layout_instanced);

		testMesh3D = Mesh::create(ib, { vb_vertex, vb_instanced }, PrimitiveDrawType::TriangleStrip);

		Material* testMaterial = new Material(testTexture);
		testMesh3D->setMaterial(testMaterial);
		*/
		/*
		// Testing model loading.. and rendering of a loaded 3d model..
		Model* m = load_model("res/TestModel.obj",
			ModelLoading_PostProcessFlags::Triangulate,
			true
		);
		
		Model* m2 = load_model("res/TestModel2.fbx", 
			ModelLoading_PostProcessFlags::Triangulate | 
			ModelLoading_PostProcessFlags::FlipUVs |
			ModelLoading_PostProcessFlags::CalcTangentSpace,
			true
		);

		// Testing skinned mesh loading..
		Model* skinnedModelTest = load_model("res/AnimationTest.fbx",
			ModelLoading_PostProcessFlags::Triangulate |
			ModelLoading_PostProcessFlags::FlipUVs |
			ModelLoading_PostProcessFlags::CalcTangentSpace |
			ModelLoading_PostProcessFlags::JoinIdenticalVertices,
			true
		);
		*/

		Model* m = ResourceManager::load_resource__model(
			"res/MultiMaterialTest.fbx",
			"m",
			ModelLoading_PostProcessFlags::Triangulate |
			ModelLoading_PostProcessFlags::FlipUVs |
			ModelLoading_PostProcessFlags::CalcTangentSpace |
			ModelLoading_PostProcessFlags::JoinIdenticalVertices,
			true
		);
		
		Model* m2 = ResourceManager::load_resource__model(
			"res/TestModel2.fbx",
			"m2",
			ModelLoading_PostProcessFlags::Triangulate |
			ModelLoading_PostProcessFlags::FlipUVs |
			ModelLoading_PostProcessFlags::CalcTangentSpace,
			true
		);
		
		Model* skinnedModelTest = ResourceManager::load_resource__model(
			"res/AnimationTest.fbx",
			"skinnedModelTest",
			ModelLoading_PostProcessFlags::Triangulate |
			ModelLoading_PostProcessFlags::FlipUVs |
			ModelLoading_PostProcessFlags::CalcTangentSpace |
			ModelLoading_PostProcessFlags::JoinIdenticalVertices,
			true
		);
		
		float scale = 10.0f;
		// Testing multiple meshes per model...
		mml::Vec3 entityScale(scale, scale, scale);
		Entity* entity_ground = new Entity(Transform(mml::Vec3(0, -20, -scale), mml::Quat({1,0,0}, mml::to_radians(0)), mml::Vec3(2, 2, 2)));

		Renderable3D* testRenderable3D = new Renderable3D(m->getMeshes()[0]);
		Renderable3D* testRenderable23D = new Renderable3D(m->getMeshes()[1]);

		entity_ground->addComponent(testRenderable3D);
		entity_ground->addComponent(testRenderable23D);

		addToScene(entity_ground);


		// Another entity with a different Renderable3D component
		// draw cube on each bone of the skeleton..

		// Skinned renderable test
		skinnedTestEntity = new Entity(Transform(mml::Vec3(0, 0, 0), mml::Quat({ 1,0,0 }, mml::to_radians(-90)), { 1,1,1 }));

		skinnedRenderable = new SkinnedRenderable3D(skinnedTestEntity, skinnedModelTest->getMeshes()[0]);
		skinnedRenderable->setPose(0);
		skinnedTestEntity->addComponent(skinnedRenderable);
		addToScene(skinnedTestEntity);
	
		animation::Animation* testAnim_idle = new animation::Animation(1, 10, 1.0f);
		animation::Animation* testAnim_run = new animation::Animation(10, 20, 4.0f);

		skinnedRenderable->getSkeleton()->addAnimation(*testAnim_idle);
		skinnedRenderable->getSkeleton()->addAnimation(*testAnim_run);


		animation::Skeleton* origSkeleton = skinnedModelTest->getMeshes()[0]->getSkeletonPrototype();
		for (int i = 0; i < skinnedRenderable->getSkeleton()->getBoneCount(); i++)
		{
			animation::Bone* b = skinnedRenderable->getSkeleton()->getAllBones()[i];
			Transform& t = b->getTransform();
			
			Entity* be = new Entity(Transform(mml::Vec3(0,0,0), mml::Quat({0,1,0}, 0), { 0.1f,0.1f,0.1f }));
			be->getTransform().setParent(&t);
			
			Renderable3D* testModelRenderable = new Renderable3D(m2->getMeshes()[0]);
			be->addComponent(testModelRenderable);
			addToScene(be);
		}

		
		// Testing terrain..

		std::vector<float> heightmap =
		{
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0
		};

		terrainTest = new Terrain(heightmap, 10.0f);
		terrainTest->getTransform().setPosition({ 0, 0, 0 });
		terrainTest->getTransform().setScale({ 1, 1, 1 });
		addToScene(terrainTest);


		// Create directional light...
		mml::Vec3 dir(0, -0.5f, -0.5f);
		dir.normalize();
		mml::Vec3 dirLightColor(1,1,1);

		_directionalLight = new graphics::DirectionalLight(dir, dirLightColor);
	}


	virtual void update() override 
	{
		skinnedRenderable->getSkeleton()->loopAnimation(0);
		if(skinnedRenderable->getSkeleton()->getAllAnimations()[0].hasEnded())
			skinnedRenderable->getSkeleton()->loopAnimation(1);

		// update fps counter
		text_fps->set("FPS : " + std::to_string(Time::FPS) + "\nDELTA : " + std::to_string(Time::DELTA));

		/*mml::Quat origRot = skinnedTestEntity->getTransform().getGlobalRotation();
		mml::Quat rotation({ 0, 1, 0 }, mml::to_radians(glfwGetTime() * 0.1f));
		skinnedTestEntity->getTransform().setRotation(rotation * origRot);
		*/
		
		/*mml::Quat lightRotation({ 0, 1, 0 }, mml::to_radians(glfwGetTime() * 0.1f));
		mml::Vec3 lightDir = _directionalLight->getDirection();
		lightDir = lightRotation * lightDir;
		_directionalLight->setDirection(lightDir);*/

		//if (_testEvent.triggeredSceneSwitch)
			//Application::get_scene_manager()->prepareSceneSwitch(new TestScene);
	}
};


class Obj
{
public:

	int type = 0;
	std::string name = "";


	Obj(std::string n, int t) :
		name(n), type(t)
	{}

	~Obj()
	{
		Debug::log("Obj destructed!");
	}
};




bool batchFunc(Obj* a, Obj* b)
{
	return a->type == b->type;
}

int main(int* argc, char* argv[])
{
	// Testing BatchList_ptr

	/*const unsigned int count = 1000000;
	Obj* objects = new Obj[count];

	utils::BatchList_ptr<Obj*> batchList(batchFunc);

	for(int i = 0; i < 8; i++)
		batchList.insert(&objects[i]);
	
	for (std::vector<Obj*>& batch : batchList.getAllBatches())
	{
		for (Obj* elem : batch)
		{
			printf("%s\n", elem->name.c_str());
		}
		printf("\n");
	}
		

	delete[] objects;
	*/
	Application app("Test application", 1600, 900, false, false, 0, new TestScene);
	app.update();
	return 0;
}