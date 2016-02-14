#pragma once
#include "IMemoryStream.h"
#include <vector>

class CReadMemoryStream : public IReadMemoryStream
{
public:
	CReadMemoryStream(const char* data, size_t size);

	virtual bool ReadBool() override;
	virtual unsigned char ReadByte() override;
	virtual int ReadInt() override;
	virtual size_t ReadSizeT() override;
	virtual float ReadFloat() override;
	virtual double ReadDouble() override;
	virtual std::string ReadString() override;
	virtual void* ReadPointer() override;
private:
	const char* m_data;
	size_t m_size;
	size_t m_position;
};

class CWriteMemoryStream : public IWriteMemoryStream
{
public:
	virtual void WriteBool(bool value) override;
	virtual void WriteByte(unsigned char value) override;
	virtual void WriteInt(int value) override;
	virtual void WriteSizeT(size_t value) override;
	virtual void WriteFloat(float value) override;
	virtual void WriteDouble(double value) override;
	virtual void WriteString(std::string const& value) override;
	virtual void WritePointer(void* value) override;

	const char * GetData() const;
	char * GetData();
	size_t GetSize() const;
private:
	std::vector<char> m_data;
};