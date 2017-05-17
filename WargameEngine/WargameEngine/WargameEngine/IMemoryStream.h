#pragma once
#include <string>

namespace wargameEngine
{
class IReadMemoryStream
{
public:
	virtual ~IReadMemoryStream() {}

	virtual bool ReadBool() = 0;
	virtual unsigned char ReadByte() = 0;
	virtual short ReadShort() = 0;
	virtual int ReadInt() = 0;
	virtual unsigned ReadUnsigned() = 0;
	virtual size_t ReadSizeT() = 0;
	virtual float ReadFloat() = 0;
	virtual double ReadDouble() = 0;
	virtual std::string ReadString() = 0;
	virtual std::wstring ReadWString() = 0;
	virtual void* ReadPointer() = 0;
	virtual void ReadData(void* data, size_t size) = 0;
	virtual void Seek(size_t pos) = 0;
};

class IWriteMemoryStream
{
public:
	virtual ~IWriteMemoryStream() {}

	virtual void WriteBool(bool value) = 0;
	virtual void WriteByte(unsigned char value) = 0;
	virtual void WriteInt(int value) = 0;
	virtual void WriteUnsigned(unsigned value) = 0;
	virtual void WriteSizeT(size_t value) = 0;
	virtual void WriteFloat(float value) = 0;
	virtual void WriteDouble(double value) = 0;
	virtual void WriteString(std::string const& value) = 0;
	virtual void WriteWString(std::wstring const& value) = 0;
	virtual void WritePointer(void* value) = 0;
};
}