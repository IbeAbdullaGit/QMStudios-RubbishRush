#include "IBuffer.h"
#include "Logging.h"

IBuffer::IBuffer(BufferType type, BufferUsage usage) :
	_elementCount(0),
	_elementSize(0),
	_size(0),
	_handle(0)
{
	_type = type;
	_usage = usage;
	glCreateBuffers(1, &_handle);
}

IBuffer::~IBuffer() {
	if (_handle != 0) {
		glDeleteBuffers(1, &_handle);
		_handle = 0;
	}
}

void IBuffer::LoadData(const void* data, size_t elementSize, size_t elementCount) {
	// Note, this is part of the bindless state access stuff added in 4.5
	glNamedBufferData(_handle, elementSize * elementCount, data, (GLenum)_usage);

	_elementCount = elementCount;
	_elementSize = elementSize;
	_size = elementCount * elementSize;
}

void IBuffer::UpdateData(const void* data, size_t elementSize, size_t elementCount, bool allowResize /*= true*/)
{
	if (elementSize * elementCount > _size) {
		if (allowResize) {
			glNamedBufferData(_handle, elementSize * elementCount, data, (GLenum)_usage);

			LOG_INFO("Expanding buffer from {} bytes to {} bytes", _size, elementCount * elementSize);

			_elementCount = elementCount;
			_elementSize = elementSize;
			_size = elementCount * elementSize;
		}
		else {
			LOG_ASSERT(false, "Attempting to write beyond the end of the buffer!");
		}
	}
	else {
		if (_size == 0) {
			glNamedBufferData(_handle, elementSize * elementCount, data, (GLenum)_usage);
			_size = elementCount * elementSize;
		}
		else {
			glNamedBufferSubData(_handle, 0, elementSize * elementCount, data);
		}
		_elementCount = elementCount;
		_elementSize = elementSize;

	}
}

void IBuffer::Bind() const {
	glBindBuffer((GLenum)_type, _handle);
}

void IBuffer::UnBind(BufferType type) {
	glBindBuffer((GLenum)type, 0);
}

void IBuffer::UnBind(BufferType type, int slot) {
	glBindBufferBase((GLenum)type, slot, 0);
}
