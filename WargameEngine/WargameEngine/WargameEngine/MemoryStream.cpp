#include "MemoryStream.h"
#include "Utils.h"
#include <string.h>

CReadMemoryStream::CReadMemoryStream(const char* data) :m_data(data), m_position(0)
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

short CReadMemoryStream::ReadShort()
{
	int16_t result;
	memcpy(&result, &m_data[m_position], sizeof(int16_t));
	m_position += sizeof(int16_t);
	return result;
}

int CReadMemoryStream::ReadInt()
{
	int32_t result;
	memcpy(&result, &m_data[m_position], sizeof(int32_t));
	m_position += sizeof(int32_t);
	return result;
}

unsigned CReadMemoryStream::ReadUnsigned()
{
	uint32_t result;
	memcpy(&result, &m_data[m_position], sizeof(uint32_t));
	m_position += sizeof(uint32_t);
	return result;
}

size_t CReadMemoryStream::ReadSizeT()
{
	return ReadUnsigned();
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
	size_t size = ReadUnsigned();
	std::string result(&m_data[m_position], size);
	m_position += size;
	return result;
}

std::wstring CReadMemoryStream::ReadWString()
{
	size_t size = ReadUnsigned();
	std::string result(&m_data[m_position], size);
	m_position += size;
	return Utf8ToWstring(result);
}

void* CReadMemoryStream::ReadPointer()
{
	uint32_t result;
	memcpy(&result, &m_data[m_position], sizeof(uint32_t));
	m_position += sizeof(uint32_t);
	return reinterpret_cast<void*>(result);
}

void CReadMemoryStream::ReadData(void* data, size_t size)
{
	memcpy(data, m_data + m_position, size);
	m_position += size;
}

void CReadMemoryStream::Seek(size_t pos)
{
	m_position = pos;
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
	WriteUnsigned(static_cast<unsigned>(value));
}

void CWriteMemoryStream::WriteUnsigned(unsigned value)
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
	WriteUnsigned(value.size());
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + value.size());
	memcpy(&m_data[oldSize], value.c_str(), value.size());
}

void CWriteMemoryStream::WriteWString(std::wstring const& value)
{
	WriteString(WStringToUtf8(value));
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
