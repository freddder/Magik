#include "cSceneManager.h"
#include "cOverworldPokemon.h"
#include "cMapManager.h"
#include "cRenderManager.h"

cSceneManager* cSceneManager::singleton = NULL;

cSceneManager::cSceneManager()
{
	currWeather = NONE;

	fogDensity = 0.f;
	fogGradient = 0.1f;
	windSpeed = 0.25f;

}

cSceneManager::~cSceneManager()
{
	for (int i = 0; i < particleSpawners.size(); i++)
	{
		delete particleSpawners[i];
	}

	roamingWildPokemon.clear();
}

void cSceneManager::SetWeather(eEnvironmentWeather newWeather)
{
	if (newWeather == currWeather) return;

	if ((currWeather == SNOW || currWeather == HAIL || currWeather == SNOWSTORM) // snow transition
		&& (newWeather == SNOW || newWeather == HAIL || newWeather == SNOWSTORM))
	{
		if (newWeather == SNOW)
		{

		}
		else if (newWeather == HAIL)
		{

		}
	}
	else if ((currWeather == RAIN || currWeather == HEAVYRAIN) // rain transition
		&& (newWeather == RAIN || newWeather == HEAVYRAIN))
	{



	}
	else // no transition
	{
		if (weatherParticleSpawner)
		{
			delete weatherParticleSpawner;
			weatherParticleSpawner = nullptr;
		}

		if (newWeather == NONE)
		{
			fogDensity = 0.f;
			fogGradient = 0.1f;
		}
		else if (newWeather == SNOW)
		{
			fogDensity = 0.057f;
			fogGradient = 0.57f;
			fogColor = glm::vec3(0.89f, 0.89f, 0.89f);

			cRenderModel prtcl;
			prtcl.meshName = "SpriteHolder.obj";
			prtcl.shaderName = "snow";
			prtcl.textureName = "SnowFlake3.png";
			prtcl.scale = glm::vec3(0.3f);

			weatherParticleSpawner = new cParticleSpawner(glm::vec3(0.f, 13.f, 0.f), prtcl, 500);
			weatherParticleSpawner->minPositionOffset = glm::vec3(-20.f, 0.f, -20.f);
			weatherParticleSpawner->maxPositionOffset = glm::vec3(20.f, 0.f, 20.f);
			weatherParticleSpawner->isPositionPlayerRelative = true;
			weatherParticleSpawner->spawnSpeed = glm::vec3(0.f, -3.f, 0.f);
			weatherParticleSpawner->spawnRate = 0.05f;
			weatherParticleSpawner->particleLifeTime = 10.f;
		}
		else if (newWeather == HAIL)
		{
			fogDensity = 0.022f;
			fogGradient = 0.48f;
			fogColor = glm::vec3(0.89f, 0.89f, 0.89f);


		}
		else if (newWeather == SNOWSTORM)
		{

		}
		else if (newWeather == RAIN)
		{

		}
		else if (newWeather == HEAVYRAIN)
		{

		}
		else if (newWeather == SANDSTORM)
		{

		}
		else if (newWeather == LEAVES)
		{

		}
	}

	currWeather = newWeather;
}

void cSceneManager::LoadSpawnData(const int nationalDexId, const int minLevel, const int maxLevel, const int spawnChance, const std::string formName)
{
	for (int i = 0; i < loadedSpawnData.size(); i++)
	{
		if (loadedSpawnData[i].nationalDexNumber == nationalDexId && loadedSpawnData[i].formName == formName)
		{
			return; // Already loaded
		}
	}

	Pokemon::sSpeciesData specieData;
	Pokemon::LoadSpecieData(nationalDexId, specieData);

	// Load overworld sprite texture
	cRenderManager* renderManager = cRenderManager::GetInstance();
	renderManager->LoadOverworldPokemonSpriteSheet(nationalDexId, formName);

	// Load female varient if there is one
	if (specieData.isSpriteGenderBased && formName == "")
	{
		renderManager->LoadOverworldPokemonSpriteSheet(nationalDexId, "f");
	}

	Pokemon::sSpawnData spawnData;
	spawnData.nationalDexNumber = nationalDexId;
	spawnData.formName = formName;
	spawnData.minLevel = minLevel;
	spawnData.maxLevel = maxLevel;
	spawnData.spawnChance = spawnChance;
	spawnData.genderRatio = specieData.genderRatio;
	spawnData.isStatsGenderBased = specieData.isStatsGenderBased;
	spawnData.isSpriteGenderBased = specieData.isSpriteGenderBased;

	loadedSpawnData.push_back(spawnData);
}

std::shared_ptr<cOverworldPokemon> cSceneManager::CreateRoamingWildPokemon(/*const Pokemon::sSpawnData& spawnData*/ int dataId, glm::vec3 location)
{
	// TODO: consider moving OWPokemon/Character constructor code here

	const Pokemon::sSpawnData& spawnData = loadedSpawnData[dataId];

	// Check if location is available
	sTile* tile = cMapManager::GetInstance()->GetTile(location);
	if (!tile) return nullptr;
	
	std::shared_ptr<cOverworldPokemon> newWildPokemon = std::make_shared<cOverworldPokemon>(location, "678_s.png");
	roamingWildPokemon.push_back(newWildPokemon);

	tile->entity = newWildPokemon.get();

	return newWildPokemon;
}

void cSceneManager::ChangeScene()
{
	// Things this should do (not ordered):
	// - unload current map model
	// - unload map textures
	// - unload map data
	// - despawn entities
	// - unload despawn data
	// - move player and follower to appropriate place
	// - remove render objects from vector
}

void cSceneManager::Process(float deltaTime)
{
	if (weatherParticleSpawner)
	{
		weatherParticleSpawner->Update(deltaTime);
	}

	for (int i = 0; i < particleSpawners.size(); i++)
	{
		particleSpawners[i]->Update(deltaTime);
	}
}
