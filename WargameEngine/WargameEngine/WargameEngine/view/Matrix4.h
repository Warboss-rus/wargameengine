#pragma once

template<class T>
class Matrix4T
{
public:
	Matrix4T()
		: m_items{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}
	{}

	Matrix4T(const T * val)
	{
		memcpy(m_items, val, sizeof(m_items));
	}

	operator T*()
	{
		return m_items;
	}

	operator const T* () const
	{
		return m_items;
	}

	Matrix4T& operator *= (Matrix4T<T> const& n)
	{
		T * m = *this;
		float temp[16] = { m[0] * n[0] + m[4] * n[1] + m[8] * n[2] + m[12] * n[3], m[1] * n[0] + m[5] * n[1] + m[9] * n[2] + m[13] * n[3], m[2] * n[0] + m[6] * n[1] + m[10] * n[2] + m[14] * n[3], m[3] * n[0] + m[7] * n[1] + m[11] * n[2] + m[15] * n[3],
			m[0] * n[4] + m[4] * n[5] + m[8] * n[6] + m[12] * n[7], m[1] * n[4] + m[5] * n[5] + m[9] * n[6] + m[13] * n[7], m[2] * n[4] + m[6] * n[5] + m[10] * n[6] + m[14] * n[7], m[3] * n[4] + m[7] * n[5] + m[11] * n[6] + m[15] * n[7],
			m[0] * n[8] + m[4] * n[9] + m[8] * n[10] + m[12] * n[11], m[1] * n[8] + m[5] * n[9] + m[9] * n[10] + m[13] * n[11], m[2] * n[8] + m[6] * n[9] + m[10] * n[10] + m[14] * n[11], m[3] * n[8] + m[7] * n[9] + m[11] * n[10] + m[15] * n[11],
			m[0] * n[12] + m[4] * n[13] + m[8] * n[14] + m[12] * n[15], m[1] * n[12] + m[5] * n[13] + m[9] * n[14] + m[13] * n[15], m[2] * n[12] + m[6] * n[13] + m[10] * n[14] + m[14] * n[15], m[3] * n[12] + m[7] * n[13] + m[11] * n[14] + m[15] * n[15] };
		memcpy(m_items, temp, sizeof(temp));
		return *this;
	}

	Matrix4T Invert()
	{
		float inv[16], det;
		int i;

		inv[0] = m_items[5] * m_items[10] * m_items[15] -
			m_items[5] * m_items[11] * m_items[14] -
			m_items[9] * m_items[6] * m_items[15] +
			m_items[9] * m_items[7] * m_items[14] +
			m_items[13] * m_items[6] * m_items[11] -
			m_items[13] * m_items[7] * m_items[10];

		inv[4] = -m_items[4] * m_items[10] * m_items[15] +
			m_items[4] * m_items[11] * m_items[14] +
			m_items[8] * m_items[6] * m_items[15] -
			m_items[8] * m_items[7] * m_items[14] -
			m_items[12] * m_items[6] * m_items[11] +
			m_items[12] * m_items[7] * m_items[10];

		inv[8] = m_items[4] * m_items[9] * m_items[15] -
			m_items[4] * m_items[11] * m_items[13] -
			m_items[8] * m_items[5] * m_items[15] +
			m_items[8] * m_items[7] * m_items[13] +
			m_items[12] * m_items[5] * m_items[11] -
			m_items[12] * m_items[7] * m_items[9];

		inv[12] = -m_items[4] * m_items[9] * m_items[14] +
			m_items[4] * m_items[10] * m_items[13] +
			m_items[8] * m_items[5] * m_items[14] -
			m_items[8] * m_items[6] * m_items[13] -
			m_items[12] * m_items[5] * m_items[10] +
			m_items[12] * m_items[6] * m_items[9];

		inv[1] = -m_items[1] * m_items[10] * m_items[15] +
			m_items[1] * m_items[11] * m_items[14] +
			m_items[9] * m_items[2] * m_items[15] -
			m_items[9] * m_items[3] * m_items[14] -
			m_items[13] * m_items[2] * m_items[11] +
			m_items[13] * m_items[3] * m_items[10];

		inv[5] = m_items[0] * m_items[10] * m_items[15] -
			m_items[0] * m_items[11] * m_items[14] -
			m_items[8] * m_items[2] * m_items[15] +
			m_items[8] * m_items[3] * m_items[14] +
			m_items[12] * m_items[2] * m_items[11] -
			m_items[12] * m_items[3] * m_items[10];

		inv[9] = -m_items[0] * m_items[9] * m_items[15] +
			m_items[0] * m_items[11] * m_items[13] +
			m_items[8] * m_items[1] * m_items[15] -
			m_items[8] * m_items[3] * m_items[13] -
			m_items[12] * m_items[1] * m_items[11] +
			m_items[12] * m_items[3] * m_items[9];

		inv[13] = m_items[0] * m_items[9] * m_items[14] -
			m_items[0] * m_items[10] * m_items[13] -
			m_items[8] * m_items[1] * m_items[14] +
			m_items[8] * m_items[2] * m_items[13] +
			m_items[12] * m_items[1] * m_items[10] -
			m_items[12] * m_items[2] * m_items[9];

		inv[2] = m_items[1] * m_items[6] * m_items[15] -
			m_items[1] * m_items[7] * m_items[14] -
			m_items[5] * m_items[2] * m_items[15] +
			m_items[5] * m_items[3] * m_items[14] +
			m_items[13] * m_items[2] * m_items[7] -
			m_items[13] * m_items[3] * m_items[6];

		inv[6] = -m_items[0] * m_items[6] * m_items[15] +
			m_items[0] * m_items[7] * m_items[14] +
			m_items[4] * m_items[2] * m_items[15] -
			m_items[4] * m_items[3] * m_items[14] -
			m_items[12] * m_items[2] * m_items[7] +
			m_items[12] * m_items[3] * m_items[6];

		inv[10] = m_items[0] * m_items[5] * m_items[15] -
			m_items[0] * m_items[7] * m_items[13] -
			m_items[4] * m_items[1] * m_items[15] +
			m_items[4] * m_items[3] * m_items[13] +
			m_items[12] * m_items[1] * m_items[7] -
			m_items[12] * m_items[3] * m_items[5];

		inv[14] = -m_items[0] * m_items[5] * m_items[14] +
			m_items[0] * m_items[6] * m_items[13] +
			m_items[4] * m_items[1] * m_items[14] -
			m_items[4] * m_items[2] * m_items[13] -
			m_items[12] * m_items[1] * m_items[6] +
			m_items[12] * m_items[2] * m_items[5];

		inv[3] = -m_items[1] * m_items[6] * m_items[11] +
			m_items[1] * m_items[7] * m_items[10] +
			m_items[5] * m_items[2] * m_items[11] -
			m_items[5] * m_items[3] * m_items[10] -
			m_items[9] * m_items[2] * m_items[7] +
			m_items[9] * m_items[3] * m_items[6];

		inv[7] = m_items[0] * m_items[6] * m_items[11] -
			m_items[0] * m_items[7] * m_items[10] -
			m_items[4] * m_items[2] * m_items[11] +
			m_items[4] * m_items[3] * m_items[10] +
			m_items[8] * m_items[2] * m_items[7] -
			m_items[8] * m_items[3] * m_items[6];

		inv[11] = -m_items[0] * m_items[5] * m_items[11] +
			m_items[0] * m_items[7] * m_items[9] +
			m_items[4] * m_items[1] * m_items[11] -
			m_items[4] * m_items[3] * m_items[9] -
			m_items[8] * m_items[1] * m_items[7] +
			m_items[8] * m_items[3] * m_items[5];

		inv[15] = m_items[0] * m_items[5] * m_items[10] -
			m_items[0] * m_items[6] * m_items[9] -
			m_items[4] * m_items[1] * m_items[10] +
			m_items[4] * m_items[2] * m_items[9] +
			m_items[8] * m_items[1] * m_items[6] -
			m_items[8] * m_items[2] * m_items[5];

		det = m_items[0] * inv[0] + m_items[1] * inv[4] + m_items[2] * inv[8] + m_items[3] * inv[12];

		if (det == 0)
			return Matrix4T();

		det = 1.0f / det;

		Matrix4T result;
		for (i = 0; i < 16; i++)
			result.m_items[i] = inv[i] * det;

		return result;
	}

	Matrix4T& Translate(T deltaX, T deltaY, T deltaZ)
	{
		m_items[0] += m_items[3] * deltaX;   m_items[4] += m_items[7] * deltaX;   m_items[8] += m_items[11] * deltaX;   m_items[12] += m_items[15] * deltaX;
		m_items[1] += m_items[3] * deltaY;   m_items[5] += m_items[7] * deltaY;   m_items[9] += m_items[11] * deltaY;   m_items[13] += m_items[15] * deltaY;
		m_items[2] += m_items[3] * deltaZ;   m_items[6] += m_items[7] * deltaZ;   m_items[10] += m_items[11] * deltaZ;   m_items[14] += m_items[15] * deltaZ;

		return *this;
	}

	Matrix4T& Scale(T scale)
	{
		return Scale(scale, scale, scale);
	}

	Matrix4T& Scale(T scaleX, T scaleY, T scaleZ)
	{
		m_items[0] *= scaleX;   m_items[4] *= scaleX;   m_items[8] *= scaleX;   m_items[12] *= scaleX;
		m_items[1] *= scaleY;   m_items[5] *= scaleY;   m_items[9] *= scaleY;   m_items[13] *= scaleY;
		m_items[2] *= scaleZ;   m_items[6] *= scaleZ;   m_items[10] *= scaleZ;   m_items[14] *= scaleZ;

		return *this;
	}
private:
	T m_items[16];
};

typedef Matrix4T<float> Matrix4F;