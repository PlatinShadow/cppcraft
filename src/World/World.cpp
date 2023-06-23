#include "World/World.h"
#include "Graphics/Renderer.h"
#include "Core/Game.h"
#include "Core/Service.h"

#include <fastnoise/FastNoiseLite.h>

namespace ZuneCraft {
	World::World() : m_Player(*this) {
		m_Renderer = Services::Get<Renderer>();
		m_ThreadPool = Services::Get<ThreadPool>();

		LoadChunks(glm::vec3(glm::vec3(16, 10, 16)), true);
	}

	World::~World() {

	}

	ChunkIndex World::ToChunkIndex(const WorldCoords& pos) {
		return ChunkIndex(
			floor(pos.x / (float)Chunk::WIDTH), 
			floor(pos.z / (float)Chunk::WIDTH)
		);
	};

	ChunkCoords World::ToChunkCoords(const WorldCoords& pos) {
		ChunkCoords cords;
		cords.x = (int)floor(pos.x) % Chunk::WIDTH;
		cords.y = floor(pos.y);
		cords.z = (int)floor(pos.z) % Chunk::WIDTH;

		if (cords.x < 0.0f) {
			cords.x += Chunk::WIDTH;
		}

		if (cords.z < 0.0f) {
			cords.z += Chunk::WIDTH;
		}

		return cords;
	}

	WorldCoords World::ToGrid(const WorldCoords& pos) {
		return glm::floor(pos);
	}


	Material World::GetVoxel(const WorldCoords& pos) {
		ChunkIndex chunkIndex = ToChunkIndex(pos);
		
		if (m_ChunkMap.find(chunkIndex) != m_ChunkMap.end()) {
			Chunk* chunk = m_ChunkMap[chunkIndex];
			ChunkCoords chunkCoords = ToChunkCoords(pos);
			return chunk->GetVoxel(chunkCoords);
		}

		ZC_WARN("Could not get chunk at X" << pos.x << " Y" << pos.y << " Z" << pos.z);
		return Material::AIR;
	}

	void World::Tick() {
		//m_Renderer->BeginFrame();

		m_Player.Tick();

		LoadChunks(m_Player.GetPos(), false);
	}

	void World::Render() {
		//m_Renderer->EndFrame();
		m_Renderer->Draw();
	}

	void World::LoadChunks(glm::vec3 pos, bool forceLoad) {
		static ChunkIndex lastChunk;
		
		pos.y = 0;

		ChunkIndex currentChunk = ToChunkIndex(m_Player.GetPos());
		if (currentChunk - lastChunk == glm::ivec2(0, 0) && !forceLoad) { return; }

		for (auto it = m_ChunkMap.begin(); it != m_ChunkMap.end();) {
			WorldCoords chunkPos = it->second->GetWorldPositionCentered();

			int distance = (int)glm::distance(chunkPos, pos);

			if (distance > Game::Get().GetConfig().RenderDistance * Chunk::WIDTH) {
				it->second->Unload();
				m_FreeChunks.push_back(it->second);
				it = m_ChunkMap.erase(it);
			}
			else 
				it++;
		}

		ChunkIndex chLoadStart = currentChunk - ChunkIndex(Game::Get().GetConfig().RenderDistance / 2);
		ChunkIndex chLoadEnd = currentChunk + ChunkIndex(Game::Get().GetConfig().RenderDistance / 2);
		ChunkIndex chunkToLoad(0);
		for (chunkToLoad.x = chLoadStart.x; chunkToLoad.x <= chLoadEnd.x; chunkToLoad.x++) {
			for (chunkToLoad.y = chLoadStart.y; chunkToLoad.y <= chLoadEnd.y; chunkToLoad.y++) {
				if (m_ChunkMap.find(chunkToLoad) != m_ChunkMap.end()) {
					continue;
				}

				Chunk* newChunk = nullptr;
				if (m_FreeChunks.empty()) {
					newChunk = new Chunk();
				}
				else {
					newChunk = m_FreeChunks.back();
					m_FreeChunks.pop_back();
				}
				
				newChunk->Load(chunkToLoad);
				m_ChunkMap[chunkToLoad] = newChunk;
			}
		}
			
		lastChunk = currentChunk;
	}
}