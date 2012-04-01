/*
 *  Simulator.cpp
 *  BasicPhyx
 *
 *  Created by Karen Liu on 12/11/10.
 *  Copyright 2010 GA Tech. All rights reserved.
 *
 */
#include <ctime>

#ifndef __SIMULATOR_H__
#include "Simulator.h"
#endif

#include "UserAllocator.h"
#include "Timing.h"
#include "Utilities.h"
#include "DrawObjects.h"
#include "Actors.h"
#include "windows.h"

//Bird launching vars
NxActor *bird;
time_t clickDownTime;
int x,y;

//Win conditions
NxActor *ball;

NxVec3 Simulator::ApplyForceToActor(NxActor *actor, const NxVec3& forceDir)
{
	NxVec3 forceVec = mForceStrength * forceDir;
	
	if (mForceMode)
		actor->addForce(forceVec);
	else 
		actor->addTorque(forceVec);
	
	return forceVec;
}

void Simulator::ProcessKeys(const bool *keys)
{
	if(keys['i'] == true){
		mForceVec = ApplyForceToActor(mActors->mSelectedActor, NxVec3(0,0,1));
	}else if(keys['k'] == true){
		mForceVec = ApplyForceToActor(mActors->mSelectedActor, NxVec3(0,0,-1));
	}else if(keys['j'] == true){
		mForceVec = ApplyForceToActor(mActors->mSelectedActor, NxVec3(1,0,0));
	}else if(keys['l'] == true){
		mForceVec = ApplyForceToActor(mActors->mSelectedActor, NxVec3(-1,0,0));
	}else if(keys['u'] == true){
		mForceVec = ApplyForceToActor(mActors->mSelectedActor, NxVec3(0,1,0));
	}else if(keys['m'] == true){
		mForceVec = ApplyForceToActor(mActors->mSelectedActor, NxVec3(0,-1,0));
	}else{
		mForceVec.zero();
	}
}

void Simulator::RenderActors()
{

	if(!goal && ball->getGlobalPosition().y < 1)
	{
		mScene->releaseActor(*ball);
		goal = true;
	}

    // Render all the actors in the scene
    int nActor = mScene->getNbActors();
    NxActor** actors = mScene->getActors();
    while (nActor--){
        NxActor* actor = *actors++;
        DrawActor(actor);
    }
}

void Simulator::RenderForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color)
{
	// draw only if the force is large enough
	NxReal force = forceVec.magnitude();
	if (force < 0.1f)  return;
	
	forceVec = 3 * forceVec/force;
	
	NxVec3 pos = actor->getCMassGlobalPosition();
	DrawArrow(pos, pos + forceVec, color);
}

void Simulator::RenderScene()
{
	RenderActors();
	if(mActors->mSelectedActor)
		RenderForce(mActors->mSelectedActor, mForceVec, NxVec3(1, 0, 0));

	//Victory, display "YOU WIN" in the sky
	if (goal)
	{
		//Y
		mActors->CreateTower(NxVec3(-3,10,10),6,NxVec3(.2,.2,.2),100.0);
		mActors->CreateBox(NxVec3(-3,12.4,10),NxVec3(1.2,.2,.2),1.0);
		mActors->CreateBox(NxVec3(-2,12.8,10),NxVec3(.2,1.2,.2),1.0);
		mActors->CreateBox(NxVec3(-4,12.8,10),NxVec3(0.2,1.2,.2),1.0);

		//O
		mActors->CreateBox(NxVec3(-6,10,10),NxVec3(1.2,.2,.2),1.0);
		mActors->CreateBox(NxVec3(-5,10.4,10),NxVec3(.2,1.2,.2),1.0);
		mActors->CreateBox(NxVec3(-7,10.4,10),NxVec3(.2,1.2,.2),1.0);
		mActors->CreateBox(NxVec3(-6,12.8,10),NxVec3(1.2,.2,.2),1.0);

		//U
		mActors->CreateBox(NxVec3(-9,10,10),NxVec3(1.2,.2,.2),1.0);
		mActors->CreateBox(NxVec3(-8,10.4,10),NxVec3(.2,1.4,.2),1.0);
		mActors->CreateBox(NxVec3(-10,10.4,10),NxVec3(.2,1.4,.2),1.0);

		//W
	}
}

bool Simulator::InitNx()
{
	if(mAllocator == NULL)
		mAllocator = new UserAllocator;
	
	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	mSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, mAllocator, NULL, desc, &errorCode);
	if(mSDK == NULL){
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}
	
	// Set the physics parameters
	mSDK->setParameter(NX_SKIN_WIDTH, 0.005f);
	
	// Set the debug visualization parameters
	mSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	mSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	mSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);
	mSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);


    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity = NxVec3(0, -9.8, 0);
    mScene = mSDK->createScene(sceneDesc);
	if(mScene == NULL){
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}
	return true;
}

void Simulator::CreateScene()
{	
	mActors = new Actors(mSDK, mScene);
	NxMaterial *defaultMaterial = mScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);
	
	// Create the objects in the scene
	mObjects.push_back(mActors->CreateGroundPlane());

	// create pendulum
	NxActor *capsule1 = mActors->CreateCapsule(NxVec3(1.4, 5, 0), 1.1, 0.25, 11);
	NxActor *capsule2 = mActors->CreateCapsule(NxVec3(1.4, 3.2, 0), 1.2, 0.35, 10.7);
	NxActor *capsule3 = mActors->CreateCapsule(NxVec3(1.4, 1.6, 0), 0.8, 0.45, 11.5);
	capsule1->setLinearDamping(0.2);
	capsule2->setLinearDamping(0.2);
	capsule3->setLinearDamping(0.2);
	
	// create joints
	NxVec3 globalAnchor1 = NxVec3(1.4,7,0);
	NxVec3 globalAnchor2 = NxVec3(1.4,5,0);
	NxVec3 globalAnchor3 = NxVec3(1.4,3,0);
	NxVec3 globalAxis = NxVec3(0, -1, 0);
	mActors->CreateSphericalJoint(NULL, capsule1, globalAnchor1, globalAxis);
	mActors->CreateSphericalJoint(capsule1, capsule2, globalAnchor2, globalAxis);
	mActors->CreateSphericalJoint(capsule2, capsule3, globalAnchor3, globalAxis);
	
	//TOWER OF POWER
	mActors->CreateBox(NxVec3(-25,0,0),NxVec3(0.3, 5, 3),0.01);
	mActors->CreateBox(NxVec3(-30,0,0),NxVec3(0.3, 5, 3),0.01);
	mActors->CreateBox(NxVec3(-27.5,6,0),NxVec3(10, 0.3, 5), 0.0001);

	ball = mActors->CreateBall(NxVec3(-27.5, 12, 0),0.5,0.01);

	//mActors->CreateStack(NxVec3(-25, 0, 0), NxVec3(4,1,1), NxVec3(1,1,1), 0.001);
	//mActors->CreateStack(NxVec3(-25, 2, 0), NxVec3(3,1,1), NxVec3(1,1,1), 0.001);
	//mActors->CreateStack(NxVec3(-25, 4, 0), NxVec3(2,1,1), NxVec3(1,1,1), 0.001);
	//mActors->CreateStack(NxVec3(-25, 6, 0), NxVec3(1,10,1), NxVec3(1,1,1), 0.001);



	// Right side wall -- immutable
	//mActors->CreateBox(NxVec3(-25,0,0),NxVec3(0.3, 10, 3),0.0);

	// Create platform/base -- immutable
	mActors->CreateStack(NxVec3(0, 0, 0), NxVec3(2, 1, 2), NxVec3(0.2, 0.2, 0.2), 0.0);

	//Create launcher!
	//mActors->CreateTower(NxVec3(0, .3, 0),20,NxVec3(0.2, 0.2, 0.2),0.001);
	//mActors->CreateStack(NxVec3(0, i*0.8, 0),NxVec3(2, 1, 2),NxVec3(0.2,0.2,0.2),0.001);
	mActors->CreateBox(NxVec3(0, .3, 0),NxVec3(0.2, 1, 0.2),0.0);
	
	//Launch stuff!




	//mActors->CreateTower(NxVec3(-2,2.4,-10),6,NxVec3(.2,.2,.2),1.0);
	//mActors->CreateTower(NxVec3(-4,2.4,-10),6,NxVec3(.2,.2,.2),1.0);

	//capsule3->addForce(NxVec3(-10000, 0, 0));

	//Destroy! -- this will knock over anything
	//NxActor *b1 = mActors->CreateBall(NxVec3(-2.0, 3, 0),1.0,1000);
	//b1->addForce(NxVec3(-500000, 50000, 0));
	
	goal = false;
	getElapsedTime();
}


void Simulator::ReleaseNx()
{
	if(mSDK != NULL)
	{
		if(mScene != NULL) mSDK->releaseScene(*mScene);
		mScene = NULL;
		NxReleasePhysicsSDK(mSDK);
		mSDK = NULL;
	}
	
	if (mAllocator)
	{
		delete mAllocator;
		mAllocator = NULL;
	}
}

void Simulator::Reset()
{
	ReleaseNx();
	if (!InitNx()) exit(0);
}


void Simulator::RunPhysics()
{
	// Update the time step
	NxReal deltaTime = getElapsedTime();
	//NxReal deltaTime = 0.0005;

	// Run collision and dynamics for delta time since the last frame
	mScene->simulate(deltaTime);	
	mScene->flushStream();
	mScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

void Simulator::launch(int mx, int my, bool fire) 
{
	if (!fire) 
	{
		bird = mActors->CreateBall(NxVec3(0, 3, 0),0.5,0.01);
		clickDownTime = time (NULL);
		x = mx;
		y = my;
	}

	if (fire) 
	{
		int dx = mx - x;
		int dy = my - y;
		time_t timeDiff = time(NULL) - clickDownTime;
		if (timeDiff >= 1 && timeDiff < 2)
			bird->addForce(NxVec3(-20, 4.1, 0));
		else if (timeDiff >= 2 && timeDiff < 3)
			bird->addForce(NxVec3(-30, 4.1, 0));
		else if (timeDiff >= 3 && timeDiff < 4)
			bird->addForce(NxVec3(-40, 4.1, 0));
		else
			bird->addForce(NxVec3(-1, 4.1, 0));
	}
}

Simulator::Simulator()
{
	mSDK = NULL;
	mScene = NULL;	
	mActors = NULL;
	mAllocator = NULL;
	
	mForceVec = NxVec3(0, 0, 0);
	mForceStrength = 10.0;
	mForceMode = true;
	mSelectedObject = NULL;
}

Simulator::~Simulator()
{
	
	int nObject = mObjects.size();
	for(int i = 0; i < nObject; i++)
		mScene->releaseActor(*mObjects[i]);
	
	mObjects.clear(); 
}