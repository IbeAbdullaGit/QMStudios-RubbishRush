#pragma once
#include <glad/glad.h>

#include "Graphics/GlEnums.h"
#include "Utils/Macros.h"

#include "Graphics/IGraphicsResource.h"

/// <summary>
/// This is our abstract base class for all our OpenGL buffer types
/// </summary>
class IBuffer : public IGraphicsResource
{
public:
	DEFINE_RESOURCE(IBuffer);

	/// <summary>
	/// Virtual destructor to allow child buffers to overload it when needed
	/// </summary>
	virtual ~IBuffer();

	/// <summary>
	/// Loads data into this buffer, using the bindless method glNamedBufferData
	/// </summary>
	/// <param name="data">The data that you want to load into the buffer</param>
	/// <param name="elementSize">The size of a single element, in bytes</param>
	/// <param name="elementCount">The number of elements to upload</param>
	virtual void LoadData(const void* data, uint32_t elementSize, uint32_t elementCount);

	virtual void UpdateData(const void* data, uint32_t elementSize, uint32_t elementCount, bool allowResize = true);

	/// <summary>
	/// Loads an array of data into this buffer, using the bindless method glNamedBufferData
	/// </summary>
	/// <typeparam name="T">The type of data you are uploading</typeparam>
	/// <param name="data">A pointer to the firest element in the array</param>
	/// <param name="count">The number of elements in the array to upload</param>
	template <typename T>
	void LoadData(const T* data, uint32_t count) {
		IBuffer::LoadData((const void*)(data), sizeof(T), count);
	}

	/// <summary>
	/// Returns the number of elements that are loaded into this buffer
	/// </summary>
	uint32_t GetElementCount() const { return _elementCount; }
	/// <summary>
	/// Returns the size in bytes of a single element in this buffer
	/// </summary>
	uint32_t GetElementSize() const { return _elementSize; }
	/// <summary>
	/// Returns the total size in bytes that this buffer occupies
	/// </summary>
	uint32_t GetTotalSize() const { return _size; }
	/// <summary>
	/// Returns the type of buffer (ex GL_ARRAY_BUFFER, GL_ARRAY_ELEMENT_BUFFER, etc...)
	/// </summary>
	BufferType GetType() const { return _type; }
	/// <summary>
	/// Returns the usage hint for this buffer (ex GL_STATIC_DRAW, GL_DYNAMIC_DRAW)
	/// </summary>
	BufferUsage GetUsage() const { return _usage; }

	/// <summary>
	/// Binds this buffer for use to the slot returned by GetType()
	/// </summary>
	virtual void Bind() const;
	/// <summary>
	/// Unbinds the buffer bound to the slot given by type
	/// </summary>
	/// <param name="type">The type or slot of buffer to unbind (ex: GL_ARRAY_BUFFER, GL_ARRAY_ELEMENT_BUFFER)</param>
	/// <param name="slot">The buffer slot to unbind, for the vast majority of cases this should be 0</param>
	static void UnBind(BufferType type);
	/// <summary>
	/// Unbinds the indexed buffers bound to the slot given by type and slot
	/// </summary>
	/// <param name="type">The type or slot of buffer to unbind (ex: GL_UNIFORM_BUFFER)</param>
	/// <param name="slot">The buffer slot to unbind</param>
	static void UnBind(BufferType type, int slot);

	// Inherited from IGraphicsResource
	virtual GlResourceType GetResourceClass() const override;

protected:
	/// <summary>
	/// Creates a new buffer with the given type and usage. Note that this is protected so only derived classes can call this
	/// </summary>
	/// <param name="type">The type of buffer (EX: GL_ARRAY_BUFFER, GL_ARRAY_ELEMENT_BUFFER)</param>
	/// <param name="usage">The usage hint for the buffer (EX: GL_STATIC_DRAW, GL_DYNAMIC_DRAW)</param>
	IBuffer(BufferType type, BufferUsage usage);
	
	uint32_t _elementSize; // The size or stride of our elements
	uint32_t _elementCount; // The number of elements in the buffer
	uint32_t _size; // The size of the buffer in bytes
	BufferUsage _usage; // The buffer usage mode (GL_STATIC_DRAW, GL_DYNAMIC_DRAW)
	BufferType _type; // The buffer type (ex GL_ARRAY_BUFFER, GL_ARRAY_ELEMENT_BUFFER)
};
