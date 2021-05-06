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

	struct FilterGroup
	{
		enum Enum
		{
			ACTOR0		= (1 << 0),
			ACTOR1		= (1 << 1),
			ACTOR2		= (1 << 2),
			GOLFBALL    = (1 << 3),
			PUTTER      = (1 << 4)
		};
	};

	///A customised collision class, implemneting various callbacks
	class MySimulationEventCallback : public PxSimulationEventCallback
	{
	public:
		//an example variable that will be checked in the main simulation loop
		bool trigger;

		MySimulationEventCallback() : trigger(false) {}

		///Method called when the contact with the trigger object is detected.
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) 
		{
			//you can read the trigger information here
			for (PxU32 i = 0; i < count; i++)
			{
				//filter out contact with the planes
				if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
				{
					//check if eNOTIFY_TOUCH_FOUND trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						int scoreCounter = 0;
						if (std::strcmp(pairs[i].otherActor->getName(), "golfBall") == 0)
						{
							if (std::strcmp(pairs[i].triggerActor->getName(), "BOUNDS") == 0)
							{
								cerr << "The ball has fallen out of bounds." << endl;
							}

							if (std::strcmp(pairs[i].triggerActor->getName(), "holeTrigger") == 0)
							{
								cerr << "Nice Shot! *wii sports music plays*" << endl;
							}
						}
					}
					//check if eNOTIFY_TOUCH_LOST trigger
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						cerr << "onTrigger::eNOTIFY_TOUCH_LOST" << endl;
						trigger = false;
					}
				}
			}
		}

		///Method called when the contact by the filter shader is detected.
		virtual void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) 
		{
			cerr << "Contact found between " << pairHeader.actors[0]->getName() << " " << pairHeader.actors[1]->getName() << endl;

			//check all pairs
			for (PxU32 i = 0; i < nbPairs; i++)
			{
				//check eNOTIFY_TOUCH_FOUND
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					cerr << "onContact::eNOTIFY_TOUCH_FOUND" << endl;
				}
				//check eNOTIFY_TOUCH_LOST
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					cerr << "onContact::eNOTIFY_TOUCH_LOST" << endl;
				}
			}
		}

		virtual void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) {}
		virtual void onWake(PxActor **actors, PxU32 count) {}
		virtual void onSleep(PxActor **actors, PxU32 count) {}
#if PX_PHYSICS_VERSION >= 0x304000
		virtual void onAdvance(const PxRigidBody *const *bodyBuffer, const PxTransform *poseBuffer, const PxU32 count) {}
#endif
	};

	//A simple filter shader based on PxDefaultSimulationFilterShader - without group filtering
	static PxFilterFlags CustomFilterShader( PxFilterObjectAttributes attributes0,	PxFilterData filterData0,
		PxFilterObjectAttributes attributes1,	PxFilterData filterData1,
		PxPairFlags& pairFlags,	const void* constantBlock,	PxU32 constantBlockSize)
	{
		// let triggers through
		if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		//enable continous collision detection
		//pairFlags |= PxPairFlag::eCCD_LINEAR;

		// trigger the contact callback for pairs (A,B) where 
		// the filtermask of A contains the ID of B and vice versa.
		if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			//trigger onContact callback for this pair of objects
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
//			pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
		}

		return PxFilterFlags();
	};

	// pyramid vertices
	static PxVec3 pyramid_verts[] = {	PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1) };

	// pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	// vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = { 1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1 };

	// pyramid ball class.
	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose = PxTransform(PxIdentity), PxReal density = 1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts), end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose = PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts), end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs), end(pyramid_trigs)), pose)
		{
		}
	};

	// wedge vertex coordinates.
	static PxVec3 wedge_verts[] = { PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1),
									PxVec3(0,-1,0), PxVec3(-1,0,0), PxVec3(1,0,0), PxVec3(0,0,-1), PxVec3(0,0,1),
									PxVec3(1,1,1), PxVec3(1,1,0), PxVec3(0,0,0), PxVec3(0,1,1), PxVec3(0,0,-1) };

	// wedge ball class.
	class Wedge : public ConvexMesh
	{
	public:
		Wedge(PxTransform pose = PxTransform(PxIdentity), PxReal density = 1.f) :
			ConvexMesh(vector<PxVec3>(begin(wedge_verts), end(wedge_verts)), pose, density)
		{
		}
	};

	// flagpole object class.
	class Flag
	{
	public:
		Cloth* cloth;
		Box* flagPole;
		PxVec3* flagPosition;

		Flag(PxVec3* flagPos)
		{
			flagPosition = flagPos;

			flagPole = new Box(PxTransform(PxVec3(flagPosition->x, flagPosition->y, flagPosition->z)), PxVec3(.25f, 10.0f, .25f), 1.0f);
			flagPole->Color(color_palette[0]);
			flagPole->SetKinematic(true);

			cloth = new Cloth(PxTransform(PxVec3(flagPosition->x, flagPosition->y + 1.f, flagPosition->z), PxQuat(PxPi / 2, PxVec3(0.0f, 0.0f, 1.0f))), PxVec2(8.f, 8.f), 40, 40);
			((PxCloth*)cloth->Get())->setExternalAcceleration(PxVec3(25.0f, 0.0f, 0.0f));

			cloth->Color(color_palette[4]);
			cloth->Name("FLAG");
		}
		void AddtoScene(Scene* scene)
		{
			scene->Add(flagPole);
			scene->Add(cloth);
		}
	};

	// basic putter shape object class.
	class BasicPutter : public DynamicActor
	{
	public:
		BasicPutter(const PxTransform& pose = PxTransform(PxIdentity), PxReal density = 1.f, PxVec3 secondShape = PxVec3()) : DynamicActor(pose)
		{
			// create putter shapes and set local pose.
			CreateShape(PxBoxGeometry(PxVec3(.7f, .1f, .05f)), density);
			CreateShape(PxBoxGeometry(PxVec3(.05f, 2.f, .05f)), density);
			CreateShape(PxBoxGeometry(PxVec3(.08f, .5f, .08f)), density);
			GetShape(1)->setLocalPose(PxTransform(PxVec3(.2f, 2.f, .0f)));
			GetShape(2)->setLocalPose(PxTransform(PxVec3(.2f, 3.5f, .0f)));
		}
	};

	// basic putter shape object class.
	class ClubPutter : public DynamicActor
	{
	public:
		ClubPutter(const PxTransform& pose = PxTransform(PxIdentity), PxReal density = 1.f, PxVec3 secondShape = PxVec3()) : DynamicActor(pose)
		{
		}
	};

	// class called to create specific instances of track (specify in constructor dimensions).
	class Track
	{
		DynamicActor* trackStraight;
		PxMaterial* trackMat;
		PxMaterial* wallMaterial;

	public:

		DynamicActor* trackWalls;
		PxVec3 trackPosition;

		Track(PxVec3* dimensions, PxVec3 trackPos, PxVec3 rotation, PxReal density = 1.f)
		{
			// create materials.
			trackMat = CreateMaterial(0.7f, 1.9f, 0.01f);				// track material.
			wallMaterial = CreateMaterial(0.7f, 1.9f, 0.01f);			// wall material.
			wallMaterial->setRestitution(0.9f);							// set bounciness.


			PxReal wallThickness = 1.f;			// wall thickness.
			trackPosition = trackPos;			// track position from function call.
			PxVec3* Dimensions = dimensions;	// track floor dimensions from function call.
			PxQuat Rotation;					// track rotation
			Rotation = PxQuat(1.0f, rotation);	

			// instantiate objects.
			trackStraight = new DynamicActor(PxTransform(trackPosition.x, wallThickness, trackPosition.z));
			trackWalls = new DynamicActor(PxTransform(trackPosition.x, wallThickness, trackPosition.z));

			// track straight.
			trackStraight->CreateShape(PxBoxGeometry(Dimensions->x, wallThickness, Dimensions->z), 1.0f);

			// track walls.
			trackWalls->CreateShape(PxBoxGeometry(Dimensions->x / 6, wallThickness * 1.25, Dimensions->z), 1.0f);
			trackWalls->CreateShape(PxBoxGeometry(Dimensions->x / 6, wallThickness * 1.25, Dimensions->z), 1.0f);
		
			// position walls at the side of the track.
			trackWalls->GetShape(0)->setLocalPose(PxTransform(trackPosition.x - Dimensions->x / 1, wallThickness, 0.0f));
			trackWalls->GetShape(1)->setLocalPose(PxTransform(trackPosition.x + Dimensions->x / 1, wallThickness, 0.0f));

			// set track straight to non-movable, set material and colour.
			trackStraight->SetKinematic(true);
			trackStraight->Material(trackMat);
			trackStraight->Color(color_palette[0]);

			// set track walls to non-movable, set material and colour.
			trackWalls->SetKinematic(true);
			trackWalls->Material(wallMaterial);
			trackWalls->Color(color_palette[4]);
		}

		// add objects to scene.
		void AddToScene(Scene* scene)
		{
			scene->Add(trackStraight);
			scene->Add(trackWalls);
		}
	};

	class TrackCorner
	{
		// track corner materials
		PxMaterial* trackMat;
		PxMaterial* wallMat;

	public:
		// declare track corner objects.
		DynamicActor* trackStraight;
		PxVec3 trackPosition;
		DynamicActor* trackCornerWall;

		TrackCorner(PxVec3* dimensions, PxVec3 trackPos, PxQuat(wallRotation), PxReal density = 1.f)
		{
			// create track corner materials.
			trackMat = CreateMaterial(0.7f, 1.9f, 0.01f);
			wallMat = CreateMaterial(0.7f, 1.9f, 0.01f);
			wallMat->setRestitution(1.f);

			PxReal thickness = 1.f;
			PxVec3* Dimensions = dimensions;
			PxQuat cornerRotation = PxQuat(PxPi / 4, PxVec3(.0f, -1.f, .0f));
			PxQuat wall_rotation = wallRotation;
			trackPosition = trackPos;

			// instantiate track corner objects.
			trackStraight = new DynamicActor(PxTransform(trackPosition.x, thickness, trackPosition.z));
			trackCornerWall = new DynamicActor(PxTransform(trackPosition.x, thickness, trackPosition.z));

			// sort out track straight objects.
			trackStraight->CreateShape(PxBoxGeometry(Dimensions->x, thickness, Dimensions->z), 1.0f);
			trackStraight->Color(color_palette[0]);
			trackStraight->SetKinematic(true);

			// sort out track corner objects.
			trackCornerWall->CreateShape(PxBoxGeometry(Dimensions->x / 6, thickness * 1.25, Dimensions->z), 1.0f);
			trackCornerWall->CreateShape(PxBoxGeometry(Dimensions->x, thickness * 1.25, Dimensions->z / 6), 1.0f);
			trackCornerWall->CreateShape(PxBoxGeometry(Dimensions->x / 8, thickness * 1.25, Dimensions->z / 2), 1.0f);

			// set local pose of track walls.
			trackCornerWall->GetShape(0)->setLocalPose(PxTransform(trackPosition.x - Dimensions->x / 1, thickness, 0.0f, wall_rotation));
			trackCornerWall->GetShape(1)->setLocalPose(PxTransform(trackPosition.x, thickness, -Dimensions->z, wall_rotation));
			trackCornerWall->GetShape(2)->setLocalPose(PxTransform(trackPosition.x - Dimensions->x / 1.5, thickness, -Dimensions->z / 1.5, cornerRotation * wall_rotation));

			// set walls to non-movable and set colour.
			trackCornerWall->SetKinematic(true);
			trackCornerWall->Color(color_palette[4]);
		}

		// add objects to scene.
		void AddtoScene(Scene* scene)
		{
			scene->Add(trackStraight);
			scene->Add(trackCornerWall);

		}
	};

	class Ramp
	{
	public:
		DynamicActor* ramp;
		PxVec3 TrackPos;

		Ramp(PxVec3* dimensions, PxVec3 trackPos, PxQuat rotation)
		{
			TrackPos = trackPos;
			PxVec3* Dimensions = dimensions;
			PxQuat rampRotation = rotation;

			// instantiate ramp object.
			ramp = new DynamicActor(PxTransform(TrackPos.x, TrackPos.y, TrackPos.z));

			// create a cuboid that has the same width and depth as track. y is arbitrary.
			ramp->CreateShape(PxBoxGeometry(Dimensions->x, Dimensions->y, Dimensions->z), 1.0f);
			ramp->GetShape(0)->setLocalPose((PxTransform(TrackPos.x, TrackPos.y, TrackPos.z, rampRotation)));
			ramp->Color(color_palette[3]);
			ramp->SetKinematic(true);
		}

		void AddToScene(Scene* scene)
		{
			scene->Add(ramp);
		}
	};

	class Hole
	{
		DynamicActor* holeFloor;
		PxMaterial* trackMat;
		PxMaterial* wallMat;

	public:
		PxVec3 trackPosition;
		DynamicActor* trackWalls;

		Hole(PxVec3* dimensions, PxVec3 trackPos, PxVec3 rotation, PxReal density = 1.f)
		{
			// set materials.
			trackMat = CreateMaterial(0.7f, 1.9f, 0.01f);
			wallMat = CreateMaterial(0.7f, 1.9f, 0.01f);
			wallMat->setRestitution(0.8f);

			// local variables.
			PxReal thickness = 1.f;
			PxVec3* Dimensions = dimensions;
			PxQuat Rotation;
			Rotation = PxQuat(-90.0f, rotation);
			trackPosition = trackPos;

			// instantiate hole and wall objects.
			holeFloor = new DynamicActor(PxTransform(trackPosition.x, thickness, trackPosition.z));
			trackWalls = new DynamicActor(PxTransform(trackPosition.x, thickness, trackPosition.z));

			// create and set wall shapes.
			trackWalls->CreateShape(PxBoxGeometry(Dimensions->x / 6, thickness * 1.25, Dimensions->z), 1.0f);
			trackWalls->CreateShape(PxBoxGeometry(Dimensions->x / 6, thickness * 1.25, Dimensions->z), 1.0f);
			trackWalls->CreateShape(PxBoxGeometry(Dimensions->x, thickness * 1.25, Dimensions->z / 6), 1.0f);

			trackWalls->GetShape(0)->setLocalPose(PxTransform(-Dimensions->x / 1, thickness, 0.0f));
			trackWalls->GetShape(1)->setLocalPose(PxTransform(Dimensions->x / 1, thickness, 0.0f));
			trackWalls->GetShape(2)->setLocalPose(PxTransform(0.0f, thickness, -10.0f));

			trackWalls->SetKinematic(true);
			trackWalls->Material(wallMat);
			trackWalls->Color(color_palette[1]);

			// create and set hole floor shapes.
			holeFloor->CreateShape(PxBoxGeometry(Dimensions->x / 2.8, thickness, Dimensions->z), 1.0f);			// create a floor tile 3d size of whole square.
			holeFloor->GetShape(0)->setLocalPose(PxTransform(Dimensions->x - 5, 0.0f, 0.0f));					// move tile over to fit in square.

			holeFloor->CreateShape(PxBoxGeometry(Dimensions->x / 2.8, thickness, Dimensions->z), 1.0f);			// create a floor tile 3rd size of whole square.
			holeFloor->GetShape(1)->setLocalPose(PxTransform(Dimensions->x - 15, 0.0f, 0.0f));					// move tile over to fit in square.

			holeFloor->CreateShape(PxBoxGeometry(Dimensions->x, thickness, Dimensions->z / 2.5), 1.0f);			// create a floor tile 2.5 times smaller than the size of the whole square.
			holeFloor->GetShape(2)->setLocalPose(PxTransform(0.0f, 0.0f, Dimensions->z - 4.0f));				// reposition tile to fit in square.

			holeFloor->CreateShape(PxBoxGeometry(Dimensions->x, thickness, Dimensions->z / 2.5), 1.0f);			// create a floor tile 2.5 times smaller than the size of the whole square.
			holeFloor->GetShape(3)->setLocalPose(PxTransform(0.0f, 0.0f, Dimensions->z - 15.0f));				// reposition tile to fit in square.

			// set additionals.
			holeFloor->SetKinematic(true);
			holeFloor->Material(trackMat);
			holeFloor->Color(color_palette[0]);
			trackWalls->Color(color_palette[4]);
		}

		void AddtoScene(Scene* scene)
		{
			scene->Add(holeFloor);
			scene->Add(trackWalls);
		}
	};

	// bounce pad object class.
	class BouncePad
	{
		vector<DistanceJoint*> trampolineSprings;
		Box* bottom, * top;
		PxMaterial* bouncePadMat;

	public:
		BouncePad(PxVec3* dimensions, PxVec3 bouncePos, PxReal stiffness = 1.0f, PxReal damping = 0.0f)
		{
			PxVec3 BouncePosition = bouncePos;
			PxVec3* Dimensions = dimensions;
			PxReal thickness = .1f;
			bouncePadMat = CreateMaterial(1.f, 1.f, 1.25f);

			// create bounce pad parts.
			bottom = new Box(PxTransform(PxVec3(BouncePosition.x, thickness, BouncePosition.z)), PxVec3(Dimensions->x, thickness, Dimensions->z));
			top = new Box(PxTransform(PxVec3(BouncePosition.x, Dimensions->y + thickness / 2, BouncePosition.z)), PxVec3(Dimensions->x - 0.1f, thickness, Dimensions->z - 0.1f));
			top->Material(bouncePadMat);
			top->Color(color_palette[4]);
			top->Name("Tramp_top");

			// create springs.
			trampolineSprings.resize(4);
			trampolineSprings[0] = new DistanceJoint(bottom, PxTransform(PxVec3(Dimensions->x, thickness, Dimensions->z)), top, PxTransform(PxVec3(Dimensions->x, -Dimensions->y / 2, Dimensions->z)));
			trampolineSprings[1] = new DistanceJoint(bottom, PxTransform(PxVec3(Dimensions->x, thickness, -Dimensions->z)), top, PxTransform(PxVec3(Dimensions->x, -Dimensions->y / 2, -Dimensions->z)));
			trampolineSprings[2] = new DistanceJoint(bottom, PxTransform(PxVec3(-Dimensions->x, thickness, Dimensions->z)), top, PxTransform(PxVec3(-Dimensions->x, -Dimensions->y / 2, Dimensions->z)));
			trampolineSprings[3] = new DistanceJoint(bottom, PxTransform(PxVec3(-Dimensions->x, thickness, -Dimensions->z)), top, PxTransform(PxVec3(-Dimensions->x, -Dimensions->y / 2, -Dimensions->z)));
			
			// set the stifness and damping of each spring.
			for (unsigned int i = 0; i < trampolineSprings.size(); i++)
			{
				trampolineSprings[i]->Stiffness(stiffness);
				trampolineSprings[i]->Damping(damping);
			}

			bottom->SetKinematic(true);
		}

		void AddToScene(Scene* scene)
		{
			scene->Add(bottom);
			scene->Add(top);
		}

		~BouncePad()
		{
			for (unsigned int i = 0; i < trampolineSprings.size(); i++)
				delete trampolineSprings[i];
		}
	};

	// Scene Class Object.
	class MyScene : public Scene
	{
		// basic objects.
		Plane* plane;
		Cloth* cloth;
		Box* box;
		
		// callbacks.
		MySimulationEventCallback* my_callback;
		
		// object for cycling actors.
		DynamicActor* currentActor;

		// club swing speed.
		PxReal clubSpeed = 0;

		// triggers.
		bool swingBack = false;
		
	public:
		// putter
		BasicPutter* putter;
		RevoluteJoint* putterJoint;
		PxTransform putterJointBoxLocation;
		Box* putterJointBox;

		// balls
		Sphere* golfBall;
		Pyramid* pyramid;
		Wedge* wedge;

		// track pieces.
		Track* Track1;
		Track* Track2;
		Track* Track3;
		Track* Track4;
		Track* Track5;
		Hole* Track_End;
		Ramp* ramp;
		Box* trampolineBox;
		BouncePad* trampoline;
		TrackCorner* Corner_1;
		TrackCorner* Corner_2;
		TrackCorner* Corner_4;

		// materials.
		PxMaterial* golfBallMaterial;

		// flag
		Flag* flag;

		// Hole Trigger.
		StaticBox* holeTrigger;

		// Score.
		int totalNumberSwings = 0;

		// scene contructor, custom filter shading.
		MyScene() : Scene(CustomFilterShader) {};

		
		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

			//cloth visualisation
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_HORIZONTAL, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_VERTICAL, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_BENDING, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_SHEARING, 1.0f);

			// joints
			px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
			px_scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);

			///Initialise and set the customised event callback
			my_callback = new MySimulationEventCallback();
			px_scene->setSimulationEventCallback(my_callback);

			GameStartObjects();
		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{
			// get local pose of the putter joint box location every update.
			putterJointBoxLocation = ((PxRigidBody*)putterJointBox->Get())->getGlobalPose();
			((PxRigidDynamic*)putter->Get())->wakeUp();

			// when the ball enters the hole trigger box, display to the user.
			if (my_callback->trigger)
			{
				cerr << "You've Won!" << endl;
			}
		}

		/// An example use of key presse handling
		void ChangeToGolfBall()
		{
			// if the current actor isnt the golf ball,
			if (currentActor != golfBall)
			{
				Add(golfBall);				// spawn a golf ball.
				Remove(currentActor);		// remove the current actor.
				currentActor = golfBall;	// assign the golf ball to current actor.
			}
			else
			{
				;	// do nothing.
			}
		}

		void ChangeToPyramidBall()
		{
			// if the current actor isnt the pyramid ball,
			if (currentActor != pyramid)
			{
				pyramid = new Pyramid(PxTransform(0.0f, 3.0f, 0.0f), 1.0f);
				Add(pyramid);			// spawn a pyramid ball.
				Remove(currentActor);	// remove the current actor.
				currentActor = pyramid;	// assign the pyramid ball to current actor.
			}
			else
			{
				;	// do nothing
			}
		}

		void ChangeToWedgeBall()
		{
			// if the current actor isnt the wedge ball,
			if (currentActor != wedge)
			{
				
				wedge = new Wedge(PxTransform(0.0f, 3.0f, 0.0f), 1.0f);
				Add(wedge);				// spawn wedge 'ball'.
				Remove(currentActor);	// remove current actor.
				currentActor = wedge;	// assign the wedge to ball to current actor.
			}
			else
			{
				;	// do nothing.
			}
		}

		// instantiate all objects that needed to be create on game load.
		void GameStartObjects()
		{
			plane = new Plane();
			plane->Color(PxVec3(210.f / 255.f, 210.f / 255.f, 210.f / 255.f));
			plane->SetTrigger("OUT OF BOUNDS");
			plane->Name("BOUNDS");
			Add(plane);

			// putter joint box.
			putterJointBox = new Box(PxTransform(PxVec3(0.f, 11.25f, 5.0f)));
			putterJointBox->SetKinematic(true);
			Add(putterJointBox);

			// putter object.
			putter = new BasicPutter(PxTransform(PxVec3(.1f, 5.f, .0f)));
			putter->Color(color_palette[0]);
			putter->Name("PUTTER");
			((PxRigidBody*)putter->Get())->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
			((PxRigidBody*)putter->Get())->setMass(0.25f); 
			Add(putter);

			// putter joint.
			putterJoint = new RevoluteJoint(putterJointBox, PxTransform(PxVec3(0.f, -4.f, 0.f), PxQuat(PxPi / 2, PxVec3(1.0f, 0.0f, 0.f))), putter, PxTransform(PxVec3(0.f, 5.f, 0.f)));
			putterJoint->SetLimits(-PxPi / 1.4f, -PxPi / 3.5f);

			// instantiate new golf ball object.
			golfBall = new Sphere(PxTransform(0.0f, 7.5f, -5.0f), 0.5f, 0.5f);
			golfBall->Name("golfBall");
			((PxRigidBody*)golfBall->Get())->setMass(0.5f);
			((PxRigidBody*)golfBall->Get())->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
			((PxRigidDynamic*)golfBall->Get())->setLinearDamping(.25f);								
			Add(golfBall);
			currentActor = golfBall;

			// straight track piece 1. (track dimensions, track position, track rotation, 1.0f)
			Track1 = new Track(new PxVec3(10.0f, 3.0f, 30.0f), PxVec3(0.0f, 10.0f, 0.0f), PxVec3(0.0f, 1.0f, 0.0f), 1.0f);
			Track1->trackWalls->GetShape(1)->setLocalPose(PxTransform(PxVec3(10.0f, 1.0f, 0.0f)));
			Track1->AddToScene(this);

			// ramp piece 1.
			ramp = new Ramp(new PxVec3(10.0f, 3.0f, 1.0f), PxVec3(0.0f, .3f, -14.0f), PxQuat(-45.0f, PxVec3(1.0f, 0.0f, 0.0f)));
			ramp->AddToScene(this);

			// ramp piece 2.
			ramp = new Ramp(new PxVec3(10.0f, 3.0f, 1.0f), PxVec3(0.0f, .3f, -31.3f), PxQuat(45.0f, PxVec3(1.0f, 0.0f, 0.0f)));
			ramp->AddToScene(this);
			
			// straight track piece 2.
			Track2 = new Track(new PxVec3(10.0f, 3.0f, 30.0f), PxVec3(0.0f, 10.0f, -90.0f), PxVec3(0.0f, 1.0f, 0.0f), 1.0f);
			Track2->trackWalls->GetShape(0)->setLocalPose(PxTransform(PxVec3(10.0f, 1.0f, 0.0f)));
			Track2->trackWalls->GetShape(1)->setLocalPose(PxTransform(PxVec3(-10.0f, 1.0f, 0.0f)));
			Track2->AddToScene(this);

			// corner track piece 1.
			Corner_1 = new TrackCorner(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(0.0f, 3.0f, -130.0f), PxQuat(PxPi * 2, PxVec3(0.0f, 1.0f, 0.0f)), 1.0f);
			Corner_1->AddtoScene(this);

			// Straight track piece 3.
			Track3 = new Track(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(18.35f, 3.0f, -130.0f), PxVec3(0.0f, 1.0f, 0.0f), 1.0f);
			Track3->trackWalls->GetShape(0)->setLocalPose(PxTransform(PxVec3(0.0f, 1.0f, 10.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Track3->trackWalls->GetShape(1)->setLocalPose(PxTransform(PxVec3(0.0f, 1.0f, -10.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Track3->AddToScene(this);

			// Straight track piece 4.
			Track3 = new Track(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(38.0f, 3.0f, -130.0f), PxVec3(0.0f, 1.0f, 0.0f), 1.0f);
			Track3->trackWalls->GetShape(0)->setLocalPose(PxTransform(PxVec3(0.0f, 1.0f, 10.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Track3->trackWalls->GetShape(1)->setLocalPose(PxTransform(PxVec3(0.0f, 1.0f, -10.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Track3->AddToScene(this);

			// corner track piece 3.
			Corner_2 = new TrackCorner(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(58.0f, 3.0f, -130.0f), PxQuat(PxPi * 2, PxVec3(0.0f, -1.0f, 0.0f)), 1.0f);
			Corner_2->trackCornerWall->GetShape(0)->setLocalPose(PxTransform(PxVec3(0.0f, 1.0f, 10.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Corner_2->trackCornerWall->GetShape(1)->setLocalPose(PxTransform(PxVec3(10.0f, 1.0f, 0.0f), PxQuat(PxPi / 2, PxVec3(0.0f, 1.0f, 0.0f))));
			Corner_2->trackCornerWall->GetShape(2)->setLocalPose(PxTransform(PxVec3(6.0f, 1.0f, 6.0f), PxQuat(-PxPi / 4, PxVec3(0.0f, 1.0f, 0.0f))));
			Corner_2->AddtoScene(this);

			// straight track piece 5.
			Track5 = new Track(new PxVec3(10.0f, 3.0f, 30.0f), PxVec3(58.0f, 3.0f, -168.3f), PxVec3(0.0f, 1.0f, 0.0f), 1.0f);
			Track5->trackWalls->GetShape(0)->setLocalPose(PxTransform(PxVec3(10.0f, 1.0f, 0.0f)));
			Track5->trackWalls->GetShape(1)->setLocalPose(PxTransform(PxVec3(-10.0f, 1.0f, 0.0f)));
			Track5->AddToScene(this);

			// create a trampoline.
			trampoline = new BouncePad(new PxVec3(13.0f, 13.0f, 13.0f), PxVec3(58.0f, 0.0f, -211.5), 70.0f, 1.0f);
			trampoline->AddToScene(this);

			// track end.
			Track_End = new Hole(new PxVec3(10.0f, 3.0f, 10.0f), PxVec3(58.0f, -10.0f, -235.0f), PxVec3(0.0f, 1.0f, 0.0f), 10.0f);
			Track_End->AddtoScene(this);

			// create flag.
			flag = new Flag(new PxVec3(58.0f, 12.0f, -234.5f));
			flag->AddtoScene(this);

			// hole trigger.
			holeTrigger = new StaticBox(PxTransform(PxVec3(57.9f, 0.0f, -234.5f)), PxVec3(1.0f, .1f, 1.0f));
			holeTrigger->SetTrigger(true);
			holeTrigger->Name("holeTrigger");
			Add(holeTrigger);

		}

		// function to set club swing speed.
		void SetClubSpeed()
		{
			if (clubSpeed <= 25.0f)
			{
				clubSpeed++;
			}
			else
			{
				clubSpeed = 5;
			}
		}

		// function to phsyically swing club.
		void SwingClub()
		{
			swingBack = false;
			((PxRigidBody*)putter->Get())->addForce(PxVec3(.0f, .0f, -clubSpeed), PxForceMode::eIMPULSE);
		}

		// function to pull club backwards ready for swing.
		void SwingBack()
		{
			((PxRigidBody*)putter->Get())->addForce(PxVec3(.0f, .0f, 1.f), PxForceMode::eIMPULSE);
			swingBack = true;
			clubSpeed = 0;
		}

		// function to increment score counter.
		void Score()
		{
			cout << " Number of Swings: " << totalNumberSwings << "\n";
			totalNumberSwings++;
		}

		// putter movement functions. 
		void PutterForward() const
		{
			// move the putter joint box to move the whole object.
			((PxRigidBody*)putterJointBox->Get())->setGlobalPose(PxTransform(PxVec3(putterJointBoxLocation.p.x, putterJointBoxLocation.p.y, putterJointBoxLocation.p.z - .05f), PxQuat(putterJointBoxLocation.q.x, putterJointBoxLocation.q.y, putterJointBoxLocation.q.z, putterJointBoxLocation.q.w)));
		}

		void PutterBack() const
		{
			((PxRigidBody*)putterJointBox->Get())->setGlobalPose(PxTransform(PxVec3(putterJointBoxLocation.p.x, putterJointBoxLocation.p.y, putterJointBoxLocation.p.z + .05f), PxQuat(putterJointBoxLocation.q.x, putterJointBoxLocation.q.y, putterJointBoxLocation.q.z, putterJointBoxLocation.q.w)));
		}

		void PutterLeft() const
		{
			((PxRigidBody*)putterJointBox->Get())->setGlobalPose(PxTransform(PxVec3(putterJointBoxLocation.p.x - .05f, putterJointBoxLocation.p.y, putterJointBoxLocation.p.z), PxQuat(putterJointBoxLocation.q.x, putterJointBoxLocation.q.y, putterJointBoxLocation.q.z, putterJointBoxLocation.q.w)));
		}

		void PutterRight() const
		{
			((PxRigidBody*)putterJointBox->Get())->setGlobalPose(PxTransform(PxVec3(putterJointBoxLocation.p.x + .05f, putterJointBoxLocation.p.y, putterJointBoxLocation.p.z), PxQuat(putterJointBoxLocation.q.x, putterJointBoxLocation.q.y, putterJointBoxLocation.q.z, putterJointBoxLocation.q.w)));
		}

		void PutterUp() const
		{
			((PxRigidBody*)putterJointBox->Get())->setGlobalPose(PxTransform(PxVec3(putterJointBoxLocation.p.x, putterJointBoxLocation.p.y + .05f, putterJointBoxLocation.p.z), PxQuat(putterJointBoxLocation.q.x, putterJointBoxLocation.q.y, putterJointBoxLocation.q.z, putterJointBoxLocation.q.w)));
		}

		void PutterDown() const
		{
			((PxRigidBody*)putterJointBox->Get())->setGlobalPose(PxTransform(PxVec3(putterJointBoxLocation.p.x, putterJointBoxLocation.p.y - .05f, putterJointBoxLocation.p.z), PxQuat(putterJointBoxLocation.q.x, putterJointBoxLocation.q.y, putterJointBoxLocation.q.z, putterJointBoxLocation.q.w)));
		}

		void RotateLeft() const
		{
			// rotate 45 degress to the left in the x-axis.
			((PxRigidBody*)putterJointBox->Get())->setGlobalPose(PxTransform(PxVec3(putterJointBoxLocation.p.x, putterJointBoxLocation.p.y, putterJointBoxLocation.p.z), PxQuat(putterJointBoxLocation.q.y + .45f, PxVec3(0.f, 1.f, .0f))));
		}

		void RotateRight() const
		{
			// rotate 45 degress to the right in the x-axis. 
			((PxRigidBody*)putterJointBox->Get())->setGlobalPose(PxTransform(PxVec3(putterJointBoxLocation.p.x, putterJointBoxLocation.p.y, putterJointBoxLocation.p.z), PxQuat(putterJointBoxLocation.q.y + -.45f, PxVec3(0.f, 1.f, .0f))));
		}

		void RotationReset()
		{
			((PxRigidBody*)putterJointBox->Get())->setGlobalPose(PxTransform(PxVec3(putterJointBoxLocation.p.x, putterJointBoxLocation.p.y, putterJointBoxLocation.p.z), PxQuat(0.f, PxVec3(0.f, 1.f, .0f))));
		}
	};
}
