#include "MemoryStream.h"
#include "Utils.h"
#include <string.h>

namespace wargameEngine
{
ReadMemoryStream::ReadMemoryStream(const char* data)
	: m_data(data)
	, m_position(0)
{
}

bool ReadMemoryStream::ReadBool()
{
	return m_data[m_position++] > 0 ? true : false;
}

unsigned char ReadMemoryStream::ReadByte()
{
	return m_data[m_position++];
}

short ReadMemoryStream::ReadShort()
{
	int16_t result;
	memcpy(&result, &m_data[m_position], sizeof(int16_t));
	m_position += sizeof(int16_t);
	return result;
}

int ReadMemoryStream::ReadInt()
{
	int32_t result;
	memcpy(&result, &m_data[m_position], sizeof(int32_t));
	m_position += sizeof(int32_t);
	return result;
}

unsigned ReadMemoryStream::ReadUnsigned()
{
	uint32_t result;
	memcpy(&result, &m_data[m_position], sizeof(uint32_t));
	m_position += sizeof(uint32_t);
	return result;
}

size_t ReadMemoryStream::ReadSizeT()
{
	return ReadUnsigned();
}

float ReadMemoryStream::ReadFloat()
{
	float result;
	memcpy(&result, &m_data[m_position], sizeof(float));
	m_position += sizeof(float);
	return result;
}

double ReadMemoryStream::ReadDouble()
{
	double result;
	memcpy(&result, &m_data[m_position], sizeof(double));
	m_position += sizeof(double);
	return result;
}

std::string ReadMemoryStream::ReadString()
{
	size_t size = ReadSizeT();
	std::string result(&m_data[m_position], size);
	m_position += size;
	return result;
}

std::wstring ReadMemoryStream::ReadWString()
{
	size_t size = ReadSizeT();
	std::string result(&m_data[m_position], size);
	m_position += size;
	return Utf8ToWstring(result);
}

void* ReadMemoryStream::ReadPointer()
{
	uint64_t result;
	memcpy(&result, &m_data[m_position], sizeof(uint64_t));
	m_position += sizeof(uint64_t);
	return reinterpret_cast<void*>(result);
}

void ReadMemoryStream::ReadData(void* data, size_t size)
{
	memcpy(data, m_data + m_position, size);
	m_position += size;
}

void ReadMemoryStream::Seek(size_t pos)
{
	m_position = pos;
}

void WriteMemoryStream::WriteBool(bool value)
{
	m_data.push_back(value ? 1 : 0);
}

void WriteMemoryStream::WriteByte(unsigned char value)
{
	m_data.push_back(value);
}

void WriteMemoryStream::WriteInt(int value)
{
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + sizeof(int32_t));
	auto normalized = static_cast<int32_t>(value);
	memcpy(&m_data[oldSize], &normalized, sizeof(int32_t));
}

void WriteMemoryStream::WriteSizeT(size_t value)
{
	WriteUnsigned(static_cast<unsigned>(value));
}

void WriteMemoryStream::WriteUnsigned(unsigned value)
{
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + sizeof(uint32_t));
	auto normalized = static_cast<uint32_t>(value);
	memcpy(&m_data[oldSize], &normalized, sizeof(uint32_t));
}

void WriteMemoryStream::WriteFloat(float value)
{
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + sizeof(float));
	memcpy(&m_data[oldSize], &value, sizeof(float));
}

void WriteMemoryStream::WriteDouble(double value)
{
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + sizeof(double));
	memcpy(&m_data[oldSize], &value, sizeof(double));
}

void WriteMemoryStream::WriteString(std::string const& value)
{
	WriteSizeT(value.size());
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + value.size());
	memcpy(&m_data[oldSize], value.c_str(), value.size());
}

void WriteMemoryStream::WriteWString(std::wstring const& value)
{
	WriteString(WStringToUtf8(value));
}

void WriteMemoryStream::WritePointer(void* value)
{
	size_t oldSize = m_data.size();
	m_data.resize(oldSize + sizeof(uint64_t));
	memcpy(&m_data[oldSize], &value, sizeof(uint64_t));
}

char* WriteMemoryStream::GetData()
{
	return m_data.data();
}

const char* WriteMemoryStream::GetData() const
{
	return m_data.data();
}

size_t WriteMemoryStream::GetSize() const
{
	return m_data.size();
}
}