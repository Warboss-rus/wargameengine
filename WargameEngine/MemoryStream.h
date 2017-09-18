#pragma once
#include "IMemoryStream.h"
#include <vector>

namespace wargameEngine
{
class ReadMemoryStream : public IReadMemoryStream
{
public:
	ReadMemoryStream(const char* data);

	bool ReadBool() override;
	unsigned char ReadByte() override;
	short ReadShort() override;
	int ReadInt() override;
	unsigned ReadUnsigned() override;
	size_t ReadSizeT() override;
	float ReadFloat() override;
	double ReadDouble() override;
	std::string ReadString() override;
	std::wstring ReadWString() override;
	void* ReadPointer() override;
	void ReadData(void* data, size_t size) override;
	void Seek(size_t pos) override;

private:
	const char* m_data;
	size_t m_position;
};

class WriteMemoryStream : public IWriteMemoryStream
{
public:
	void WriteBool(bool value) override;
	void WriteByte(unsigned char value) override;
	void WriteInt(int value) override;
	void WriteUnsigned(unsigned value) override;
	void WriteSizeT(size_t value) override;
	void WriteFloat(float value) override;
	void WriteDouble(double value) override;
	void WriteString(std::string const& value) override;
	void WriteWString(std::wstring const& value) override;
	void WritePointer(void* value) override;

	const char* GetData() const;
	char* GetData();
	size_t GetSize() const;

private:
	std::vector<char> m_data;
};
}