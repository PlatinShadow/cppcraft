#include "Core/Base.h"
#include "World/Chunk.h"
#include "Graphics/GL.h"
#include "Geometry/Mesher.h"
#include "Graphics/Renderer.h"
#include "Build/PlatformDefines.h"

#include <time.h>
#include <fastnoise/FastNoiseLite.h>

namespace ZuneCraft {
	Chunk::Chunk(const glm::ivec2& index) {
		SetIndex(index);
		GenTerrain();
	}

	void Chunk::SetIndex(const glm::ivec2& index) {
		m_Index = index;
		m_WorldPostion = glm::vec3(index.x * CHUNK_WIDTH, 0, index.y * CHUNK_WIDTH);
	}

	const glm::ivec2& Chunk::GetIndex() const{
		return m_Index;
	}

	void Chunk::GenTerrain() {
		memset(&m_Voxels.Data[0], 0, CHUNK_SIZE_QUBED);
		FastNoiseLite noise;

		noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noise.SetFractalType(FastNoiseLite::FractalType_FBm);
		noise.SetFractalOctaves(8.0f);
		noise.SetFractalLacunarity(2.0f);
		noise.SetFractalGain(0.5f);
		noise.SetFrequency(0.005f);

		//noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		
		for (int x = 0; x < CHUNK_WIDTH; x++) {
			for (int z = 0; z < CHUNK_WIDTH; z++) {

	
				float height = noise.GetNoise((x + m_WorldPostion.x), (z + m_WorldPostion.z));
				//height = powf(height, 2.5f);


				int blockHeight = CHUNK_HEIGHT/2 + height * CHUNK_HEIGHT/2;

				//blockHeight = 5;

				for (int y = 0; y < blockHeight && y < CHUNK_HEIGHT; y++) {

					if (y == (int)blockHeight - 1) {
						m_Voxels.Data[x][y][z] = (uint8_t)BlockType::GRASS;
					}
					else {
						m_Voxels.Data[x][y][z] = (uint8_t)BlockType::DIRT;
					}
					
				}

				m_Voxels.Data[x][0][z] = (uint8_t)BlockType::GRASS;
			}
		}
	}

	Chunk::~Chunk() {

	}

	const glm::vec3& Chunk::GetWorldPosition() const {
		return m_WorldPostion;
	}

	const glm::vec3 Chunk::GetWorldPositionCentered() const {
		return m_WorldPostion + glm::vec3(CHUNK_WIDTH / 2, 0, CHUNK_WIDTH / 2);
	}

	uint8_t Chunk::TryGet(int x, int y, int z) {
		if (x >= CHUNK_WIDTH || x < 0) {
			return (uint8_t)BlockType::AIR;
		}

		if (y >= CHUNK_HEIGHT || y < 0) {
			return (uint8_t)BlockType::AIR;
		}

		if (z >= CHUNK_WIDTH || z < 0) {
			return (uint8_t)BlockType::AIR;
		}

		return m_Voxels.Data[x][y][z];

	}

	bool Chunk::IsVoidAt(int x, int y, int z) {
		return m_Voxels.Data[x][y][z] == (uint8_t)BlockType::AIR;
	}

	void Chunk::Update() {
		std::vector<Vertex> mesh;
		Mesher::VoxelToGreedy(m_Voxels, 2, &mesh);
		Renderer::BatchSubmitMesh(mesh, GetWorldPosition());
	}
}