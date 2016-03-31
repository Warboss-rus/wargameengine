#include "MemoryStream.h"

CReadMemoryStream::CReadMemoryStream(const char* data, size_t size) :m_data(data), m_size(size), m_position(0)
{
}

bool CReadMemoryStream::ReadBool()
{
	return m_data[m_position++] > 0 ? true : false;
}

unsigned char CReadMemoryStream::ReadByte()
{
	return m_data[m_position++];
}

int CReadMemoryStream::ReadInt()
{
	int32_t result;
	memcpy(&result, &m_data[m_position], sizeof(int32_t));
	m_position += sizeof(int32_t);
	return result;
}

size_t CReadMemoryStream::ReadSizeT()
{
	uint32_t result;
	memcpy(&result, &m_data[m_position], sizeof(uint32_t));
	m_position += sizeof(uint32_t);
	return result;
}

float CReadMemoryStream::ReadFloat()
{
	float result;
	memcpy(&result, &m_data[m_position], sizeof(float));
	m_position += sizeof(float);
	return result;
}

double CReadMemoryStream::ReadDouble()
{
	double result;
	memcpy(&result, &m_data[m_position], sizeof(double));
	m_position += sizeof(double);
	return result;
}

std::string CReadMemoryStream::ReadString()
{
	size_t size = ReadSizeT();
	std::string result(&m_data[m_position], size);
	m_position += size;
	return std::move(result);
}

void* CReadMemoryStream::ReadPointer()
{
	uint32_t result;
	memcpy(&result, &m_data[m_position], sizeof(uint32_t));
	m_position += sizeof(uint32_t);
	return reinterpret_cast<void*>(result);
}

void CWriteMemoryStream::WriteBool(bool value)
{
	m_data.push_back(value ? 1 : 0);
}

void CWriteMemoryStream::WriteByte(unsigned char value)
{
	m_data.push_back(value);
}

void CWriteMemoryStream::WriteInt(int value)
{
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + sizeof(int32_t));
	auto normalized = static_cast<int32_t>(value);
	memcpy(&m_data[oldSize], &normalized, sizeof(int32_t));
}

void CWriteMemoryStream::WriteSizeT(size_t value)
{
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + sizeof(uint32_t));
	auto normalized = static_cast<uint32_t>(value);
	memcpy(&m_data[oldSize], &normalized, sizeof(uint32_t));
}

void CWriteMemoryStream::WriteFloat(float value)
{
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + sizeof(float));
	memcpy(&m_data[oldSize], &value, sizeof(float));
}

void CWriteMemoryStream::WriteDouble(double value)
{
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + sizeof(double));
	memcpy(&m_data[oldSize], &value, sizeof(double));
}

void CWriteMemoryStream::WriteString(std::string const& value)
{
	WriteSizeT(value.size());
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + value.size());
	memcpy(&m_data[oldSize], value.c_str(), value.size());
}

void CWriteMemoryStream::WritePointer(void* value)
{
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + sizeof(uint32_t));
	memcpy(&m_data[oldSize], &value, sizeof(uint32_t));
}

char * CWriteMemoryStream::GetData()
{
	return m_data.data();
}

const char * CWriteMemoryStream::GetData() const
{
	return m_data.data();
}

size_t CWriteMemoryStream::GetSize() const
{
	return m_data.size();
}