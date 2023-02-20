#pragma once

#include "Core/Service.h"
#include "Core/Base.h"
#include "Graphics/DeviceResources.h"
#include "Data/Vertex.h"
#include "Graphics/VertexPool.h"
#include "Data/HandleStore.h"
#include "Graphics/RenderAPI.h"

#include <glm/glm.hpp>
#include <queue>
#include <vector>

namespace ZuneCraft {
	class Renderer : public IService {
	public:
		Renderer();
		~Renderer();

		void Draw();

		void SetResolution(int width, int height);
		void SetView(const glm::mat4& viewMat);
		void SetFlip(bool flipped);

		Id BatchSubmitMesh(std::vector<Vertex>& mesh, const glm::vec3& translation);
		void BatchDrawMesh(Id hMesh);
		void BatchFreeMesh(Id hMesh);

	private:
		Ref<RenderAPI> m_Device;

		struct BatchData {
			glm::vec3 Translation;
		};

		struct MeshInfo {
			VPNode* MemoryLocation;
			size_t BatchId;
			Id RenderCommand;
		};

		GLuint m_RenderWidth;
		GLuint m_RenderHeight;

		Pipeline* m_MeshPipeline;
		Pipeline* m_FXPipeline;
		Ref<GPUStorage> m_BatchDataBuffer;

		size_t m_BatchCurrentOffset;

		std::vector<BatchData> m_BatchData;
		VertexPool* m_BatchMesh;
		std::queue<size_t> m_BatchDataFreeSlots;

		HandleStore<MeshInfo> m_Meshes;
	};
}