#pragma warning(disable : 4786)

#include "particleSystem.h"


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

#include "modelerdraw.h"

/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem()
: bake_fps(0.0f), bake_start_time(0.0f), bake_end_time(0.0f), simulate(false), dirty(false)
{
}





/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
	for (auto it : particles)
		for (auto itt = it.second.begin(); itt != it.second.end(); itt++)
			delete *itt;
}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
	// These values are used by the UI ...
	// -ve bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	simulate = true;
	dirty = true;

	std::cout << "Start Simulation" << std::endl;
}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
	// These values are used by the UI
	simulate = false;
	dirty = true;

	bake_end_time = t;
	std::cout << "Stop Simulation" << std::endl;
}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
	// These values are used by the UI
	simulate = false;
	dirty = true;

	for (auto it : particles)
		for (auto itt = it.second.begin(); itt != it.second.end(); itt++)
			delete *itt;
}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{
	// TODO
	if (!simulate)
		return;
		
	std::vector<Particle*> pa;
	int time = int(t * 30.0f) / (30.0f / bake_fps);

	// Copy old particles
	std::vector<Particle*>& p = particles[time - 1];
	for (auto it = p.begin(); it != p.end(); it++)
	{
		Particle* old_p = *it;
		if (t - old_p->t < 5.0f)
		{
			Particle* new_p = new Particle(*old_p);
			new_p->f = Vec3f( 0.0f, 0.0f, 0.0f );
			pa.push_back(new_p);
		}
	}

	// Spawn new particles
	int r = rand() % 5;
	for (int i = 0; i < r; i++)
	{
		Particle* pp = new Particle;
		pp->p = Vec3f( float(rand()) / RAND_MAX * 2 - 1, 1.0f, float(rand()) / RAND_MAX * 2 - 1);
		pp->v = Vec3f( 0.0f, float(rand()) / RAND_MAX * 2 - 1, 0.0f );
		pp->f = Vec3f( 0.0f, 0.0f, 0.0f );
		pp->m = 1.0f;
		pp->t = t;
		pa.push_back(pp);
	}

	// Accumulate Force
	for (auto it = pa.begin(); it != pa.end(); it++)
	{
		(*it)->f += Vec3f( 0.0f, 1.0f, 0.0f);
	}

	// Calculate Position
	for (auto it = pa.begin(); it != pa.end(); it++)
	{
		(*it)->p += (*it)->v * (1.0f / bake_fps);
		(*it)->v += (*it)->f * (1.0f / bake_fps) / (*it)->m;
	}

	particles[time] = pa;
}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{
	int time = int(t * 30.0f) / (30.0f / bake_fps);
	for (auto it = particles[time].begin(); it != particles[time].end(); it++)
	{
		Particle* p = *it;
		glPushMatrix();

		glTranslatef(p->p[0], p->p[1], p->p[2]);
		drawSphere(0.1f);

		glPopMatrix();
	}
}





/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{

	// TODO
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{

	// TODO
}