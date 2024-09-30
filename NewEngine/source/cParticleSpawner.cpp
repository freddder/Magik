#include "cParticleSpawner.h"

#include "Player.h"

#include "cLinearCongruentialGenerator.h"
#include <time.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

cParticleSpawner::cParticleSpawner(glm::vec3 position, cRenderModel _model, unsigned int _maxParticles)
{
	spawnPosition = position;
	isPositionPlayerRelative = false;

	particles.reserve(_maxParticles);
	maxParticles = _maxParticles; // if -1, no limit
	spawnRate = 0.1f; // particles spawned interval
	timer = 0.f;

	model = _model;

	// do the random
	srand((int)time(0));
	unsigned int init_seed = (rand() % 100) + 1;
	lcgPosX = cLinearCongruentialGenerator(init_seed);

	init_seed = (rand() % 100) + 1;
	lcgPosY = cLinearCongruentialGenerator(init_seed);

	init_seed = (rand() % 100) + 1;
	lcgPosZ = cLinearCongruentialGenerator(init_seed);

	// Create buffer for particle data (position + timer)
	glGenBuffers(1, &particleBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, particleBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * _maxParticles, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

cParticleSpawner::~cParticleSpawner()
{
	particles.clear();

	glDeleteBuffers(1, &particleBufferId);
}

bool cParticleSpawner::SpawnParticle()
{
	if (particles.size() >= particles.capacity()) return false;

	double randomPosX;
	double randomPosY;
	double randomPosZ;

	lcgPosX.get_uniform_draw(randomPosX);
	lcgPosY.get_uniform_draw(randomPosY);
	lcgPosZ.get_uniform_draw(randomPosZ);

	//NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin
	glm::vec3 newParticlePosOffset = glm::vec3(
		(((float)randomPosX * (maxPositionOffset.x - minPositionOffset.x)) / (1 - 0)) + minPositionOffset.x,
		(((float)randomPosY * (maxPositionOffset.y - minPositionOffset.y)) / (1 - 0)) + minPositionOffset.y,
		(((float)randomPosZ * (maxPositionOffset.z - minPositionOffset.z)) / (1 - 0)) + minPositionOffset.z
	);

	// TODO: add speed offset as well

	particles.emplace_back();
	sParticle& newParticle = particles.back();
	newParticle.position = newParticlePosOffset + spawnPosition;
	newParticle.speed = spawnSpeed;
	newParticle.timer = 0.f;

	if (isPositionPlayerRelative)
	{
		newParticle.position += Player::GetPlayerPosition();
	}

	return true;
}

void cParticleSpawner::SpawnParticles(unsigned int numToSpawn)
{
	for (unsigned int i = 0; i < numToSpawn; i++)
	{
		SpawnParticle();
	}
}

void cParticleSpawner::Update(float deltaTime)
{
	if (spawnRate > 0.f)
	{
		timer += deltaTime;

		if (timer > spawnRate && SpawnParticle())
		{
			timer -= spawnRate;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, particleBufferId);

	for (int i = 0; i < particles.size(); i++)
	{
		particles[i].timer += deltaTime;

		if (particles[i].timer > particleLifeTime)
		{
			particles.erase(particles.begin() + i);
			i--;

			continue;
		}

		// Update position
		particles[i].position += particles[i].speed * deltaTime;

		// TODO: update speed for gravity stuff

		// update buffer
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * i, sizeof(glm::vec4), glm::value_ptr(glm::vec4(particles[i].position, particles[i].timer)));
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
