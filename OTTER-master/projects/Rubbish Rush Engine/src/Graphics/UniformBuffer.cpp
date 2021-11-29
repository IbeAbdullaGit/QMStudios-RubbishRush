#include "UniformBuffer.h"
#include "Logging.h"

AbstractUniformBuffer::~AbstractUniformBuffer() {
	delete[] _rawData;
}

AbstractUniformBuffer::AbstractUniformBuffer(size_t sizeInBytes, BufferUsage usage /*= BufferUsage::DynamicDraw*/) :
	IBuffer(BufferType::Uniform, usage),
	_rawData(nullptr)
{
	_rawData = new uint8_t[sizeInBytes];
	_size = sizeInBytes;
	memset(_rawData, 0, sizeInBytes);
	glNamedBufferData(_handle, _size, _rawData, (GLenum)_usage);
}

void AbstractUniformBuffer::LoadData(const void* data, size_t elementSize, size_t elementCount) {
	size_t dataSize = elementCount * elementSize;
	LOG_ASSERT(dataSize <= _size, "Data exceeds the bounds of this UBO");
	// Copy data from the data given to our internal buffer
	memcpy(_rawData, data, dataSize);
	// Upload data to the OpenGL buffer
	glNamedBufferSubData(_handle, 0, dataSize, _rawData);
}

void AbstractUniformBuffer::Bind() const {
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, _handle);
}

void AbstractUniformBuffer::Bind(int slot) const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, slot, _handle);
}

