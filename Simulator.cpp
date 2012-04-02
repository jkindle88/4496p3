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
int winCount = 0;
int stage = 0;
int numShots = 0;

//global constants
const int TOTAL_STAGE_COUNT = 5;



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
	/*if(keys['i'] == true){
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
	}*/
}

void Simulator::RenderActors()
{
	//check if win conditions met
	if(!goal && ball!=NULL && ball->getGlobalPosition().y < 1)
	{
		mScene->releaseActor(*ball);
		goal = true;
	}

	/*if (stage >= TOTAL_STAGE_COUNT)
	{
		goal = true;
	}*/

	//destroy 'bird' once it touches ground
	/*if(bird != NULL && bird->getGlobalPosition().y < 1)
	{
		mScene->releaseActor(*bird);
		bird = NULL;
	}*/

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

void Simulator::ResetScene()
{
	int nActor = mScene->getNbActors();
    NxActor** actors = mScene->getActors();
    while (nActor--){
        NxActor* actor = *actors++;
		mScene->releaseActor(*actor);
		actor = NULL;
    }
}


void Simulator::RenderScene()
{
	RenderActors();
	if(mActors->mSelectedActor)
		RenderForce(mActors->mSelectedActor, mForceVec, NxVec3(1, 0, 0));

	//win, progress through levels
	if(goal) {stage++; ResetScene(); CreateScene(stage);}
	
	//Victory, display "YOU WIN" in the sky
	if(stage > TOTAL_STAGE_COUNT && (winCount == 0 || winCount > 3000) && winCount < 3020)
	{
		stage++;
		//Y
		mActors->CreateTower(NxVec3(-3,10,10),6,NxVec3(.2,.2,.2),0.001);
		mActors->CreateBox(NxVec3(-3,12.4,10),NxVec3(1.2,.2,.2),0.001);
		mActors->CreateBox(NxVec3(-2,12.8,10),NxVec3(.2,1.2,.2),0.001);
		mActors->CreateBox(NxVec3(-4,12.8,10),NxVec3(0.2,1.2,.2),0.001);

		//O
		mActors->CreateBox(NxVec3(-6,10,10),NxVec3(1.2,.2,.2),0.001);
		mActors->CreateBox(NxVec3(-5,10.4,10),NxVec3(.2,1.2,.2),0.001);
		mActors->CreateBox(NxVec3(-7,10.4,10),NxVec3(.2,1.2,.2),0.001);
		mActors->CreateBox(NxVec3(-6,12.8,10),NxVec3(1.2,.2,.2),0.001);

		//U
		mActors->CreateBox(NxVec3(-9,10,10),NxVec3(1.2,.2,.2),0.001);
		mActors->CreateBox(NxVec3(-8,10.4,10),NxVec3(.2,1.4,.2),0.001);
		mActors->CreateBox(NxVec3(-10,10.4,10),NxVec3(.2,1.4,.2),0.001);

		//W
		mActors->CreateBox(NxVec3(-13,10,10),NxVec3(1.2,.2,.2),0.001);
		mActors->CreateBox(NxVec3(-12,10.4,10),NxVec3(.2,1.6,.2),0.001);
		mActors->CreateBox(NxVec3(-13,10.4,10),NxVec3(.2,1.6,.2),0.001);
		mActors->CreateBox(NxVec3(-14,10.4,10),NxVec3(.2,1.6,.2),0.001);

		//I 
		mActors->CreateTower(NxVec3(-15,10,10),7,NxVec3(.2,.2,.2),0.001);

		//N
		mActors->CreateTower(NxVec3(-16,10,10),7,NxVec3(.2,.2,.2),0.001);
		mActors->CreateBox(NxVec3(-18,10,10),NxVec3(.2,1.2,.2),0.001);
		mActors->CreateBox(NxVec3(-17.2,12.4,10),NxVec3(1.0,.2,.2),0.001);
	}
	if (stage > TOTAL_STAGE_COUNT)
		winCount++;
	else if (winCount > 3021)
		winCount = 3021;

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

void Simulator::CreateScene(int st)
{	
	mActors = new Actors(mSDK, mScene);
	NxMaterial *defaultMaterial = mScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);
	
	// Create the objects in the scene
	mObjects.push_back(mActors->CreateGroundPlane());
	
	buildLevel(st);

	// Create platform and launcher -- immutable
	mActors->CreateStack(NxVec3(0, 0, 0), NxVec3(2, 1, 2), NxVec3(0.2, 0.2, 0.2), 0.0);
	mActors->CreateBox(NxVec3(0, .3, 0),NxVec3(0.2, 1, 0.2),0.0);
	
	goal = false;
	getElapsedTime();
}

/**NOTE -- the following line must be included in EACH of the levels, as
* this determines the winstate.  The game will crash with a runtime error
* if it is not included
*/
void Simulator::buildLevel(int s)
{
	if (s==0) //scene 1
	{
		//TOWER OF POWER
		mActors->CreateBox(NxVec3(-25,0,0),NxVec3(0.3, 5, 3),0.01);
		mActors->CreateBox(NxVec3(-30,0,0),NxVec3(0.3, 5, 3),0.01);
		mActors->CreateBox(NxVec3(-27.5,6,0),NxVec3(10, 0.3, 5), 0.0001);

		ball = mActors->CreateBall(NxVec3(-27.5, 12, 0),0.5,0.01);
	}
	else if (s==1) //scene 2
	{
		/*// create pendulum
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
		mActors->CreateSphericalJoint(capsule2, capsule3, globalAnchor3, globalAxis);*/

		mActors->CreateStack(NxVec3(-25,0,0),NxVec3(5,5,3),NxVec3(.5,1,.5),0.01);
		mActors->CreateTower(NxVec3(-26,10,0),10,NxVec3(2,.1,2),0.01);
		mActors->CreateBox(NxVec3(-25,12,0),NxVec3(.2, .2, 1),0.1);
		mActors->CreateBox(NxVec3(-27,12,0),NxVec3(.2, .2, 1),0.1);
		mActors->CreateBox(NxVec3(-26,12,1),NxVec3(.5, .2, .2),0.1);
		mActors->CreateBox(NxVec3(-26,12,-1),NxVec3(.5, .2, .2),0.1);

		ball = mActors->CreateBall(NxVec3(-26, 12, 0),0.5,0.01);
	}
	else if (s==2)
	{
		mActors->CreateTower(NxVec3(-28,0,0),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-23,0,0),5,NxVec3(1,.5,1),0.01);

		mActors->CreateBox(NxVec3(-25.5,5,0),NxVec3(4,.1,1),0.01);
		mActors->CreateBox(NxVec3(-25.5,5.1,0),NxVec3(1,.1,3),0.01);

		mActors->CreateTower(NxVec3(-28,5.1,0),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-23,5.1,0),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-25.5,5.1,-2),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-25.5,5.1,2),5,NxVec3(1,.5,1),0.01);

		ball = mActors->CreateBall(NxVec3(-26.5, 5, 0),0.5,0.01);
	}
	else if (s==3)
	{
		mActors->CreateTower(NxVec3(-27,0,3),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-27,0,-3),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-24,0,3),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-24,0,-3),5,NxVec3(1,.5,1),0.01);

		mActors->CreateBox(NxVec3(-25.5,6,0),NxVec3(3,.1,4),0.01);

		mActors->CreateTower(NxVec3(-26,6,0),5,NxVec3(1.5,.5,1.5),0.01);

		mActors->CreateBox(NxVec3(-25,11.5,0),NxVec3(.2, .2, 1),0.1);
		mActors->CreateBox(NxVec3(-27,11.5,0),NxVec3(.2, .2, 1),0.1);
		mActors->CreateBox(NxVec3(-26,11.5,1),NxVec3(.5, .2, .2),0.1);
		mActors->CreateBox(NxVec3(-26,11.5,-1),NxVec3(.5, .2, .2),0.1);

		ball = mActors->CreateBall(NxVec3(-26, 12, 0),0.5,0.01);
	}
	else if (s==4) //Challenge round
	{
		mActors->CreateTower(NxVec3(-27,0,3),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-27,0,-3),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-24,0,3),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-24,0,-3),5,NxVec3(1,.5,1),0.01);

		mActors->CreateBox(NxVec3(-25.5,5.7,0),NxVec3(3,.1,4),0.01);

		mActors->CreateBox(NxVec3(-25,6,0),NxVec3(.2, .2, 1),0.1);
		mActors->CreateBox(NxVec3(-27,6,0),NxVec3(.2, .2, 1),0.1);
		mActors->CreateBox(NxVec3(-26,6,1),NxVec3(.5, .2, .2),0.1);
		mActors->CreateBox(NxVec3(-26,6,-1),NxVec3(.5, .2, .2),0.1);

		ball = mActors->CreateBall(NxVec3(-26, 6, 0),0.5,0.01);
	}
	else if (s==5) //The real challenge, this is pretty much impossible.
	{
		mActors->CreateTower(NxVec3(-27,0,3),5,NxVec3(1,.5,1),0.03);
		mActors->CreateTower(NxVec3(-27,0,-3),5,NxVec3(1,.5,1),0.03);
		mActors->CreateTower(NxVec3(-24,0,3),5,NxVec3(1,.5,1),0.03);
		mActors->CreateTower(NxVec3(-24,0,-3),5,NxVec3(1,.5,1),0.03);

		mActors->CreateBox(NxVec3(-25.5,5.7,0),NxVec3(3,.1,4),0.01);

		mActors->CreateTower(NxVec3(-27,6,3),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-27,6,-3),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-24,6,3),5,NxVec3(1,.5,1),0.01);
		mActors->CreateTower(NxVec3(-24,6,-3),5,NxVec3(1,.5,1),0.01);

		mActors->CreateBox(NxVec3(-25.5,10.5,0),NxVec3(3,.1,4),0.01);
		
		mActors->CreateTower(NxVec3(-27,10.6,0),5,NxVec3(.8,.3,1.2),0.03);
		mActors->CreateTower(NxVec3(-24,10.6,0),5,NxVec3(.8,.3,1.2),0.03);
		mActors->CreateTower(NxVec3(-27,10.6,3),5,NxVec3(1.2,.3,1.2),0.03);
		mActors->CreateTower(NxVec3(-27,10.6,-3),5,NxVec3(1.2,.3,1.2),0.03);
		mActors->CreateTower(NxVec3(-24,10.6,3),5,NxVec3(1.2,.3,1.2),0.03);
		mActors->CreateTower(NxVec3(-24,10.6,-3),5,NxVec3(1.2,.3,1.2),0.03);

		ball = mActors->CreateBall(NxVec3(-25.5,12, 0),0.5,0.01);
	}
	else //hack to handle the issue mentioned in the note at the top of this method
	{
		//ball = mActors->CreateBall(NxVec3(-27.5, 12, 0),0.5,0.0);
		ball = NULL;
	}
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
		mScene->releaseActor(*bird);
		bird = mActors->CreateBall(NxVec3(0, 3, 0),0.5,0.01);
		clickDownTime = time (NULL);
		x = mx;
		y = my;
	}

	if (fire) 
	{
		double dx = mx - x;
		double dy = my - y;

		dx = dx*(1.0/2.0);
		dy = dy*(1.0/4.0);

		if (dx<-29) dx = -29;
		else if (dx>=0) dx = -1;
		if (dy>20) dy = 20;
		else if (dy<0) dy = 0;

		time_t timeDiff = time(NULL) - clickDownTime;
		/*if (timeDiff >= 1 && timeDiff < 2)
			bird->addForce(NxVec3(-20, 4.1, 0));
		else if (timeDiff >= 2 && timeDiff < 3)
			bird->addForce(NxVec3(-30, 4.1, 0));
		else if (timeDiff >= 3 && timeDiff < 4)
			bird->addForce(NxVec3(-40, 4.1, 0));
		else
			bird->addForce(NxVec3(-1, 4.1, 0));*/
		bird->addForce(NxVec3(dx, dy, 0));
	}
}

//Getters and Setters
void Simulator::setGoal(bool g){ goal = g;}
bool Simulator::getGoal() { return goal; }
void Simulator::setStage(int s) { stage = s; }
int Simulator::getStage() { return stage; }

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