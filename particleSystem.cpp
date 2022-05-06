#pragma warning(disable : 4786)

#include "particleSystem.h"


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

#include "modelerdraw.h"

std::vector<Vec4f*> particle_spawn;

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
	for (auto it = particles.begin(); it != particles.end(); it++)
		for (auto itt = (*it).begin(); itt != (*it).end(); itt++)
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

	for (auto it = particles.begin(); it != particles.end(); it++)
		for (auto itt = (*it).begin(); itt != (*it).end(); itt++)
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
	if (!particles.empty())
	{
		std::vector<Particle*>& p = particles.back();
		for (auto it = p.begin(); it != p.end(); it++)
		{
			Particle* old_p = *it;
			if (t - old_p->t < 2.5f)
			{
				Particle* new_p = new Particle(*old_p);
				new_p->f = Vec3f(0.0f, 0.0f, 0.0f);
				pa.push_back(new_p);
			}
		}
	}

	// Spawn new particles
	for (auto it = particle_spawn.begin(); it != particle_spawn.end(); it++)
	{
		if (!*it)
			continue;
		
		int r = rand() % 5;
		for (int i = 0; i < r; i++)
		{
			float r_x = (float)rand() / RAND_MAX * 0.002f - 0.001f;
			float r_y = (float)rand() / RAND_MAX * 0.002f - 0.001f;
			float r_z = (float)rand() / RAND_MAX * 0.002f - 0.001f;

			Vec4f pos = **it + Vec4f(r_x, r_y, r_z, 1.0f);
			
			Particle* pp = new Particle;
			pp->p = Vec3f( pos[0], pos[1], pos[2] );
			pp->v = Vec3f( float(rand()) / RAND_MAX * 0.5f - 0.25f, float(rand()) / RAND_MAX *  0.7f + 1.0f, -2.0f );
			pp->f = Vec3f( 0.0f, 0.0f, 0.0f );
			pp->m = 1.0f;
			pp->t = t;
			pa.push_back(pp);
		}
	}

	// Accumulate Force
	for (auto it = pa.begin(); it != pa.end(); it++)
	{
		for (auto func = forces.begin(); func != forces.end(); func++)
			// (*it)->f += Vec3f( 0.0f, -1.0f, 0.0f);
			(*it)->f += (*func)((*it)->p);
	}

	// Calculate Position
	for (auto it = pa.begin(); it != pa.end(); it++)
	{
		(*it)->p += (*it)->v * (1.0f / bake_fps);
		(*it)->v += (*it)->f * (1.0f / bake_fps) / (*it)->m;
	}

	particles.push_back(pa);
}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{
	int time = int(t * 30.0f) / (30.0f / bake_fps);
	if (time >= particles.size())
		return;
	std::vector<Particle*>& p = particles[time];
	if (p.empty())
		return;

	float currentColor[4];
	glGetFloatv(GL_CURRENT_COLOR,currentColor);
	for (auto it = p.begin(); it != p.end(); it++)
	{
		Particle* p = *it;
		glPushMatrix();

		setDiffuseColor((t - p->t) / 3.0f, (t - p->t) / 3.0f, 1.0f);
		glTranslatef(p->p[0], p->p[1], p->p[2]);
		drawSphere(p->m * 0.02f);

		glPopMatrix();
	}
	setDiffuseColor(currentColor[0], currentColor[1], currentColor[2]);
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

void ParticleSystem::appendForce(std::function<Vec3f(Vec3f)> func)
{
	forces.push_back(func);	
}