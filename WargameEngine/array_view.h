#pragma once
#include <vector>
#include <array>

template<class T>
class array_view
{
public:
	array_view(T* data, size_t size) noexcept
		: m_data(data), m_size(size)
	{}
	array_view(const std::vector<T> & data) noexcept
		: m_data(data.data()), m_size(data.size())
	{}
	array_view(const std::initializer_list<T> & data) noexcept
		: m_data(data.begin()), m_size(data.size())
	{}
	template<size_t size>
	array_view(const std::array<T, size> & data) noexcept
		: m_data(data.data()), m_size(size)
	{}

	const T* begin() const noexcept
	{
		return m_data;
	}
	const T* end() const noexcept
	{
		return m_data + m_size;
	}
	const T& operator[](size_t index) const noexcept
	{
		return m_data[index];
	}

	size_t size() const noexcept
	{
		return m_size;
	}
	const T* data() const noexcept
	{
		return m_data;
	}
	bool empty() const noexcept
	{
		return m_size == 0;
	}
private:
	const T* m_data;
	size_t m_size;
};