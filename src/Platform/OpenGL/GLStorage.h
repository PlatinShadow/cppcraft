#pragma once
#include "Core/Base.h"
#include "Graphics/DeviceResources.h"
#include "Graphics/RenderAPI.h"
#include <string>

namespace ZuneCraft {
	//TODO: Rename size into count to make it more clear that it is an index
	//		instead of size in bytes
	class GLVBOStorage : public GPUStorage {
	public:
		GLVBOStorage(GLenum type, GLuint initialCount, GLuint stride, GLenum usage, void* initialData);
		~GLVBOStorage();
		
		void Upload(uint32_t size, uint32_t offset, void* data) override;
		void Bind() override;
		size_t GetMaxCount() override;
		uint32_t GetNativeHandle() override;
		void Resize(uint32_t newSize) override;

	private:
		GLuint m_Handle;
		GLuint m_MaxCount;
		GLuint m_Stride;
		GLenum m_Type;
		GLenum m_Usage;
	};

	class GLUniformStorage : public GPUStorage {
	public:
		GLUniformStorage(Ref<Shader>& shader, const std::string& location, StorageFormat format, GLuint initialSize);
		void Upload(uint32_t size, uint32_t offset, void* data) override;
		void Bind() override;
		size_t GetMaxCount() override;
		uint32_t GetNativeHandle() override;
		void Resize(uint32_t newSize) override;

	private:
		Ref<Shader> m_Shader;
		GLuint m_MaxSize;
		GLuint m_Location;
		std::string m_UniformName;
		StorageFormat m_Format;
	};
}