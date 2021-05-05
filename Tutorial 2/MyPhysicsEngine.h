#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	// a list of colours: Circus Palette
	static const PxVec3 color_palette[] = 
	{
		PxVec3(46.f/255.f,9.f/255.f,39.f/255.f),
		PxVec3(217.f/255.f,0.f/255.f,0.f/255.f),
		PxVec3(255.f/255.f,45.f/255.f,0.f/255.f),
		PxVec3(255.f/255.f,140.f/255.f,54.f/255.f),
		PxVec3(4.f/255.f,117.f/255.f,111.f/255.f)
	};

	// pyramid vertices
	static PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
	// pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	// vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = {1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1};

	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose=PxTransform(PxIdentity), PxReal density=1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose=PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs),end(pyramid_trigs)), pose)
		{

		}
	};

	// class to create a specific compound object.
	class GolfClub : public DynamicActor
	{

	public:
		// constructor must match dynamic actor constructor.
		GolfClub(const PxTransform& pose) : DynamicActor(pose)
		{
			// create individual shapes in the shape of a club.
			CreateShape(PxBoxGeometry(PxVec3(0.5f, 5.0f, 0.5f)), .1f);
			CreateShape(PxBoxGeometry(PxVec3(2.5f, 0.6f, 0.5f)), .1f);

			// set local pose of different shapes.
			GetShape(1)->setLocalPose(PxTransform(PxVec3(-1.0f, -4.25f, 0.0f)));
			// set colour of random shapes.
			time_t t;
			std::srand((unsigned)time(&t));
			int random_number = std::rand() % 5;
			Color(color_palette[random_number]);
		}
	};

	class MyBox : public DynamicActor
	{
	public:
		MyBox(const PxTransform& pose) : DynamicActor(pose)
		{
			PxVec3 boxDimensions = PxVec3(1.f, 1.f, 1.f);
			CreateShape(PxBoxGeometry(boxDimensions), .1f);
		}
	};


	///Custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		Box* box;
		Sphere* sphere;
		GolfClub* golfClub;
		MyBox* myBox;



	public:
		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f);			// enable actor axis visual debugging aspect.
			px_scene->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, 2.0f);	// enbale linear velocity visual debugging aspect.
		}

		//Custom scene initialisation
		virtual void CustomInit()
		{
			SetVisualisation();

			GetMaterial()->setDynamicFriction(.2f);

			// create a plane and add it to scene.
			plane = new Plane();
			plane->Color(PxVec3(210.f / 255.f, 210.f / 255.f, 210.f / 255.f));
			Add(plane);

			// create golfclub object and add it to scene.
			golfClub = new GolfClub(PxTransform(PxVec3(2.5, 2.5, 2.5f)));
			Add(golfClub);

			// create 20 boxes in a row apart from each other.
			float xpos = 0.0f;
			for (int i = 0; i < 21; i++)
			{
				float ypos = 0.0f;
				// create box object and add it to scene.
				myBox = new MyBox(PxTransform(PxVec3(xpos, ypos, 1.f), PxQuat(45.f, PxVec3(.0f, 1.0f, .0f))));
				Add(myBox);
				xpos += 5.f;
				ypos += 5.0f;
			}
			

		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{

		}
	};

		

}
