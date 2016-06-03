#include <vector>
#include <sal.h>
#include <limits>
#include "MathLibLite.h"

namespace MathLibLite
{

const float3 float3::nan = float3(NAN, NAN, NAN);
const float3 float3::unitX = float3(1, 0, 0);
const float3 float3::unitY = float3(0, 1, 0);
const float3 float3::unitZ = float3(0, 0, 1);

bool EqualAbs(float a, float b, float epsilon = FLT_EPSILON)
{
	return abs(a - b) < epsilon;
}

template<typename T>
inline T Clamp(const T &val, const T &floor, const T &ceil)
{
	return val <= ceil ? (val >= floor ? val : floor) : ceil;
}

template<typename T>
inline T Clamp01(const T &val) { return Clamp(val, T(0), T(1)); }

float3::float3(float x_, float y_, float z_)
	:x(x_), y(y_), z(z_)
{
}

float3::float3(const float *data)
{
	if (!data)
		return;
	x = data[0];
	y = data[1];
	z = data[2];
}

float3x3::float3x3(float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22)
{
	Set(_00, _01, _02,
		_10, _11, _12,
		_20, _21, _22);
}

void float3x3::Set(float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22)
{
	v[0][0] = _00; v[0][1] = _01; v[0][2] = _02;
	v[1][0] = _10; v[1][1] = _11; v[1][2] = _12;
	v[2][0] = _20; v[2][1] = _21; v[2][2] = _22;
}

AABB::AABB(const float3 &minPoint_, const float3 &maxPoint_)
	:minPoint(minPoint_), maxPoint(maxPoint_)
{
}

OBB::OBB(const AABB &aabb)
{
	SetFrom(aabb);
}

void OBB::SetFrom(const AABB &aabb)
{
	pos = aabb.CenterPoint();
	r = aabb.HalfSize();
	axis[0] = float3(1, 0, 0);
	axis[1] = float3(0, 1, 0);
	axis[2] = float3(0, 0, 1);
}

Polyhedron OBB::ToPolyhedron() const
{
	// Note to maintainer: This function is an exact copy of AABB:ToPolyhedron() and Frustum::ToPolyhedron().

	Polyhedron p;
	// Populate the corners of this OBB.
	// The will be in the order 0: ---, 1: --+, 2: -+-, 3: -++, 4: +--, 5: +-+, 6: ++-, 7: +++.
	for (int i = 0; i < 8; ++i)
		p.v.push_back(CornerPoint(i));

	// Generate the 6 faces of this OBB.
	const int faces[6][4] =
	{
		{ 0, 1, 3, 2 }, // X-
		{ 4, 6, 7, 5 }, // X+
		{ 0, 4, 5, 1 }, // Y-
		{ 7, 6, 2, 3 }, // Y+
		{ 0, 2, 6, 4 }, // Z-
		{ 1, 5, 7, 3 }, // Z+
	};

	for (int f = 0; f < 6; ++f)
	{
		Polyhedron::Face face;
		for (int v = 0; v < 4; ++v)
			face.v.push_back(faces[f][v]);
		p.f.push_back(face);
	}

	return p;
}

float3 OBB::CenterPoint() const
{
	return pos;
}

void OBB::Translate(const float3 &offset)
{
	pos += offset;
}

void OBB::Scale(const float3 &centerPoint, float scaleFactor)
{
	return Scale(centerPoint, float3(scaleFactor, scaleFactor, scaleFactor));
}

float3x4 operator *(const TranslateOp &lhs, const ScaleOp &rhs);
float3x4 operator *(const float3x4 &lhs, const TranslateOp &rhs);

void OBB::Scale(const float3 &centerPoint, const float3 &scaleFactor)
{
	///@bug This scales in global axes, not local axes.
	float3x4 transform = float3x4::Scale(scaleFactor, centerPoint);
	Transform(transform);
}

template<typename Matrix>
void OBBTransform(OBB &o, const Matrix &transform)
{
	o.pos = transform.MulPos(o.pos);
	o.axis[0] = transform.MulDir(o.r.x * o.axis[0]);
	o.axis[1] = transform.MulDir(o.r.y * o.axis[1]);
	o.axis[2] = transform.MulDir(o.r.z * o.axis[2]);
	o.r.x = o.axis[0].Normalize();
	o.r.y = o.axis[1].Normalize();
	o.r.z = o.axis[2].Normalize();
}

void OBB::Transform(const float3x3 &transform)
{
	assume(transform.IsColOrthogonal());
	OBBTransform(*this, transform);
}

void OBB::Transform(const float3x4 &transform)
{
	assume(transform.IsColOrthogonal());
	OBBTransform(*this, transform);
}

bool OBB::Intersects(const AABB &aabb) const
{
	return Intersects(OBB(aabb));
}

bool OBB::Intersects(const OBB &b, float epsilon) const
{
	assume(pos.IsFinite());
	assume(b.pos.IsFinite());
	assume(float3::AreOrthonormal(axis[0], axis[1], axis[2]));
	assume(float3::AreOrthonormal(b.axis[0], b.axis[1], b.axis[2]));

	// Generate a rotation matrix that transforms from world space to this OBB's coordinate space.
	float3x3 R;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			R[i][j] = Dot(axis[i], b.axis[j]);

	float3 t = b.pos - pos;
	// Express the translation vector in a's coordinate frame.
	t = float3(Dot(t, axis[0]), Dot(t, axis[1]), Dot(t, axis[2]));

	float3x3 absR;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			absR[i][j] = abs(R[i][j]) + epsilon;

	// Test the three major axes of this OBB.
	for (int i = 0; i < 3; ++i)
	{
		float ra = r[i];
		float rb = DOT3(b.r, absR[i]);
		if (abs(t[i]) > ra + rb)
			return false;
	}

	// Test the three major axes of the OBB b.
	for (int i = 0; i < 3; ++i)
	{
		float ra = r[0] * absR[0][i] + r[1] * absR[1][i] + r[2] * absR[2][i];
		float rb = b.r[i];
		if (abs(t.x * R[0][i] + t.y * R[1][i] + t.z * R[2][i]) > ra + rb)
			return false;
	}

	// Test the 9 different cross-axes.

	// A.x <cross> B.x
	float ra = r.y * absR[2][0] + r.z * absR[1][0];
	float rb = b.r.y * absR[0][2] + b.r.z * absR[0][1];
	if (abs(t.z * R[1][0] - t.y * R[2][0]) > ra + rb)
		return false;

	// A.x < cross> B.y
	ra = r.y * absR[2][1] + r.z * absR[1][1];
	rb = b.r.x * absR[0][2] + b.r.z * absR[0][0];
	if (abs(t.z * R[1][1] - t.y * R[2][1]) > ra + rb)
		return false;

	// A.x <cross> B.z
	ra = r.y * absR[2][2] + r.z * absR[1][2];
	rb = b.r.x * absR[0][1] + b.r.y * absR[0][0];
	if (abs(t.z * R[1][2] - t.y * R[2][2]) > ra + rb)
		return false;

	// A.y <cross> B.x
	ra = r.x * absR[2][0] + r.z * absR[0][0];
	rb = b.r.y * absR[1][2] + b.r.z * absR[1][1];
	if (abs(t.x * R[2][0] - t.z * R[0][0]) > ra + rb)
		return false;

	// A.y <cross> B.y
	ra = r.x * absR[2][1] + r.z * absR[0][1];
	rb = b.r.x * absR[1][2] + b.r.z * absR[1][0];
	if (abs(t.x * R[2][1] - t.z * R[0][1]) > ra + rb)
		return false;

	// A.y <cross> B.z
	ra = r.x * absR[2][2] + r.z * absR[0][2];
	rb = b.r.x * absR[1][1] + b.r.y * absR[1][0];
	if (abs(t.x * R[2][2] - t.z * R[0][2]) > ra + rb)
		return false;

	// A.z <cross> B.x
	ra = r.x * absR[1][0] + r.y * absR[0][0];
	rb = b.r.y * absR[2][2] + b.r.z * absR[2][1];
	if (abs(t.y * R[0][0] - t.x * R[1][0]) > ra + rb)
		return false;

	// A.z <cross> B.y
	ra = r.x * absR[1][1] + r.y * absR[0][1];
	rb = b.r.x * absR[2][2] + b.r.z * absR[2][0];
	if (abs(t.y * R[0][1] - t.x * R[1][1]) > ra + rb)
		return false;

	// A.z <cross> B.z
	ra = r.x * absR[1][2] + r.y * absR[0][2];
	rb = b.r.x * absR[2][1] + b.r.y * absR[2][0];
	if (abs(t.y * R[0][2] - t.x * R[1][2]) > ra + rb)
		return false;

	// No separating axis exists, so the two OBB don't intersect.
	return true;
}

bool OBB::Intersects(const Line &line) const
{
	AABB aabb(float3(0, 0, 0), float3(Size()));
	Line l = WorldToLocal() * line;
	return aabb.Intersects(l);
}

float3x4 OBB::WorldToLocal() const
{
	float3x4 m = LocalToWorld();
	m.InverseOrthonormal();
	return m;
}

float3x4 OBB::LocalToWorld() const
{
	// To produce a normalized local->world matrix, do the following.
	/*
	float3x4 m;
	float3 x = axis[0] * r.x;
	float3 y = axis[1] * r.y;
	float3 z = axis[2] * r.z;
	m.SetCol(0, 2.f * x);
	m.SetCol(1, 2.f * y);
	m.SetCol(2, 2.f * z);
	m.SetCol(3, pos - x - y - z);
	return m;
	*/

	assume(axis[0].IsNormalized());
	assume(axis[1].IsNormalized());
	assume(axis[2].IsNormalized());
	float3x4 m;
	m.SetCol(0, axis[0]);
	m.SetCol(1, axis[1]);
	m.SetCol(2, axis[2]);
	m.SetCol(3, pos - axis[0] * r.x - axis[1] * r.y - axis[2] * r.z);
	assume(m.IsOrthonormal());
	return m;
}

Line::Line(const float3 &pos_, const float3 &dir_)
	:pos(pos_), dir(dir_)
{
}

LineSegment::LineSegment(const float3 &a_, const float3 &b_)
	: a(a_), b(b_)
{
}

const float float3::At(int index) const
{
	return ptr()[index];
}

float& float3::At(int index)
{
	return ptr()[index];
}

float *float3::ptr()
{
	return &x;
}

const float *float3::ptr() const
{
	return &x;
}

float3 float3::operator -(const float3 &rhs) const
{
	return float3(x - rhs.x, y - rhs.y, z - rhs.z);
}

float3 float3::operator -() const
{
	return float3(-x, -y, -z);
}

float3 float3::operator *(float scalar) const
{
	return float3(x * scalar, y * scalar, z * scalar);
}

float3 &float3::operator +=(const float3 &rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

float float3::Normalize()
{
	assume(IsFinite());
	float length = Length();
	if (length > 1e-6f)
	{
		*this *= 1.f / length;
		return length;
	}
	else
	{
		Set(1.f, 0.f, 0.f); // We will always produce a normalized vector.
		return 0; // But signal failure, so user knows we have generated an arbitrary normalization.
	}
}

bool float3::IsNormalized(float epsilonSq) const
{
	return fabs(LengthSq() - 1.f) <= epsilonSq;
}

bool float3::IsFinite() const
{
	return true;
}

void float3::Set(float x_, float y_, float z_)
{
	x = x_;
	y = y_;
	z = z_;
}

float float3::LengthSq() const
{
	return x*x + y*y + z*z;
}

float float3::Length() const
{
	return sqrt(LengthSq());
}

bool float3::IsPerpendicular(const float3 &other, float epsilon) const
{
	return fabs(Dot(other)) <= epsilon * Length() * other.Length();
}

float float3::Dot(const float3 &rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

float3 &float3::operator *=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

float3 float3x3::operator *(const float3 &rhs) const
{
	return float3(DOT3(v[0], rhs),
		DOT3(v[1], rhs),
		DOT3(v[2], rhs));
}

bool MUST_USE_RESULT float3::AreOrthonormal(const float3 &a, const float3 &b, const float3 &c, float epsilon)
{
	return a.IsPerpendicular(b, epsilon) &&
		a.IsPerpendicular(c, epsilon) &&
		b.IsPerpendicular(c, epsilon) &&
		a.IsNormalized(epsilon*epsilon) &&
		b.IsNormalized(epsilon*epsilon) &&
		c.IsNormalized(epsilon*epsilon);
}

bool float3x3::IsColOrthogonal(float epsilon) const
{
	return Col(0).IsPerpendicular(Col(1), epsilon)
		&& Col(0).IsPerpendicular(Col(2), epsilon)
		&& Col(1).IsPerpendicular(Col(2), epsilon);
}

const float3 float3x3::Col(int col) const
{
	return float3(v[0][col], v[1][col], v[2][col]);
}

float *float3x3::ptr()
{
	return &v[0][0];
}

const float *float3x3::ptr() const
{
	return &v[0][0];
}

MatrixProxy<float3x3::Cols> &float3x3::operator[](int row)
{
	return *(reinterpret_cast<MatrixProxy<Cols>*>(v[row]));
}

float3 float3x3::Mul(const float3 &rhs) const { return *this * rhs; }

float3 Polyhedron::ClosestPoint(const LineSegment &lineSegment) const
{
	return ClosestPoint(lineSegment, 0);
}

float3 operator *(float scalar, const float3 &rhs)
{
	return float3(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z);
}

Line operator *(const float3x4 &transform, const Line &l)
{
	return Line(transform.MulPos(l.pos), transform.MulDir(l.dir));
}

float3x4 operator *(const TranslateOp &lhs, const ScaleOp &rhs)
{
	float3x4 ret;
	ret[0][0] = rhs.x; ret[0][1] = 0; ret[0][2] = 0; ret[0][3] = lhs.x;
	ret[1][0] = 0; ret[1][1] = rhs.y; ret[1][2] = 0; ret[1][3] = lhs.y;
	ret[2][0] = 0; ret[2][1] = 0; ret[2][2] = rhs.z; ret[2][3] = lhs.z;

	return ret;
}

void float3x4::SetRow(int row, float m_r0, float m_r1, float m_r2, float m_r3)
{
	v[row][0] = m_r0;
	v[row][1] = m_r1;
	v[row][2] = m_r2;
	v[row][3] = m_r3;
}

void float3x4::InverseOrthonormal()
{
	assume(IsOrthonormal());
	/* In this function, we seek to optimize the matrix inverse in the case this
	matrix is orthonormal, i.e. it can be written in the following form:

	[ R | T ]
	M = [---+---]
	[ 0 | 1 ]

	where R is a 3x3 orthonormal (orthogonal vectors, normalized columns) rotation
	matrix, and T is a 3x1 vector representing the translation performed by
	this matrix.

	In this form, the inverse of this matrix is simple to compute and will not
	require the calculation of determinants or expensive Gaussian elimination. The
	inverse is of form

	[ R^t | R^t(-T) ]
	M^-1 = [-----+---------]
	[  0  |    1    ]

	which can be seen by multiplying out M * M^(-1) in block form. Especially the top-
	right cell turns out to (remember that R^(-1) == R^t since R is orthonormal)

	R * R^t(-T) + T * 1 == (R * R^t)(-T) + T == -T + T == 0, as expected.

	Therefore the inversion requires only two steps: */

	// a) Transpose the top-left 3x3 part in-place to produce R^t.
	std::swap(v[0][1], v[1][0]);
	std::swap(v[0][2], v[2][0]);
	std::swap(v[1][2], v[2][1]);

	// b) Replace the top-right 3x1 part by computing R^t(-T).
	SetTranslatePart(TransformDir(-v[0][3], -v[1][3], -v[2][3]));
}

void float3x4::SetTranslatePart(const float3 &offset)
{
	SetCol(3, offset);
}

void float3x4::SetCol(int column, const float3 &columnVector)
{
	SetCol(column, columnVector.x, columnVector.y, columnVector.z);
}

void float3x4::SetCol(int column, float m_0c, float m_1c, float m_2c)
{
	v[0][column] = m_0c;
	v[1][column] = m_1c;
	v[2][column] = m_2c;
}

const MatrixProxy<float3x4::Cols> & float3x4::operator[](int row) const
{
	return *(reinterpret_cast<const MatrixProxy<Cols>*>(v[row]));
}

MatrixProxy<float3x4::Cols> & float3x4::operator[](int row)
{
	return *(reinterpret_cast<MatrixProxy<Cols>*>(v[row]));
}

bool float3x4::Equals(const float3x4 &other, float epsilon) const
{
	for (int y = 0; y < Rows; ++y)
		for (int x = 0; x < Cols; ++x)
			if (!EqualAbs(v[y][x], other[y][x], epsilon))
				return false;
	return true;
}

float3 float3x4::MulPos(const float3 &pointVector) const
{
	return TransformPos(pointVector);
}

ScaleOp float3x4::Scale(const float3 &scale)
{
	return ScaleOp(scale);
}

float3x4 float3x4::Scale(const float3 &scale, const float3 &scaleCenter)
{
	return float3x4::Translate(scaleCenter) * float3x4::Scale(scale) * float3x4::Translate(-scaleCenter);
}

TranslateOp float3x4::Translate(const float3 &offset)
{
	return TranslateOp(offset);
}

const float3 float3x4::Col(int col) const
{
	return float3(v[0][col], v[1][col], v[2][col]);
}

bool float3x4::IsColOrthogonal(float epsilon /*= 1e-3f*/) const
{
	return Col(0).IsPerpendicular(Col(1), epsilon)
		&& Col(0).IsPerpendicular(Col(2), epsilon)
		&& Col(1).IsPerpendicular(Col(2), epsilon);
}

bool float3x4::IsOrthonormal(float epsilon /*= 1e-3f*/) const
{
	///@todo Epsilon magnitudes don't match.
	return IsColOrthogonal(epsilon) && Row3(0).IsNormalized(epsilon) && Row3(1).IsNormalized(epsilon) && Row3(2).IsNormalized(epsilon);
}

const float3 & float3x4::Row3(int row) const
{
	return reinterpret_cast<const float3 &>(v[row]);
}

float3 & float3x4::Row3(int row)
{
	return reinterpret_cast<float3 &>(v[row]);
}
#define DOT3_xyz(v1, x, y, z) ((v1)[0] * (x) + (v1)[1] * (y) + (v1)[2] * (z))
float3 float3x4::TransformDir(float x, float y, float z) const
{
	return float3(DOT3_xyz(v[0], x, y, z),
		DOT3_xyz(v[1], x, y, z),
		DOT3_xyz(v[2], x, y, z));
}

float3 float3x4::TransformDir(const float3 &directionVector) const
{
	return TransformDir(directionVector.x, directionVector.y, directionVector.z);
}

float3 float3x4::TransformPos(float x, float y, float z) const
{
	return float3(DOT3_xyz(v[0], x, y, z) + v[0][3],
		DOT3_xyz(v[1], x, y, z) + v[1][3],
		DOT3_xyz(v[2], x, y, z) + v[2][3]);
}

float3 float3x4::TransformPos(const float3 &pointVector) const
{
	return TransformPos(pointVector.x, pointVector.y, pointVector.z);
}

float3 float3x4::MulDir(const float3 &directionVector) const
{
	return TransformDir(directionVector);
}

float3x4 ScaleOp::ToFloat3x4() const
{
	float3x4 m;
	m.SetRow(0, x, 0, 0, 0);
	m.SetRow(1, 0, y, 0, 0);
	m.SetRow(2, 0, 0, z, 0);
	return m;
}

ScaleOp::ScaleOp(const float3 &scale) 
	:x(scale.x), y(scale.y), z(scale.z)
{
}

TranslateOp::TranslateOp(const float3 &trans) 
	: x(trans.x), y(trans.y), z(trans.z)
{
}

float3 TranslateOp::Offset() const
{
	return float3(x, y, z);
}

float3x4 operator *(const float3x4 &lhs, const TranslateOp &rhs)
{
	float3x4 r = lhs;
	r.SetTranslatePart(lhs.TransformPos(rhs.Offset()));
	return r;
}

float3 AABB::CenterPoint() const
{
	return (minPoint + maxPoint) * 0.5f;
}

float3 AABB::Size() const
{
	return maxPoint - minPoint;
}

float3 AABB::HalfSize() const
{
	return Size() * 0.5f;
}

float3 float3::operator +(const float3 &rhs) const
{
	return float3(x + rhs.x, y + rhs.y, z + rhs.z);
}

bool AABB::Intersects(const Line &line) const
{
	float tNear = -INFINITY;
	float tFar = INFINITY;

	return IntersectLineAABB_CPP(line.pos, line.dir, tNear, tFar);
}

inline float RecipFast(float x)
{
	return 1.f / x;
}

template<typename T>
T Max(const T &a, const T &b)
{
	return a >= b ? a : b;
}

template<typename T>
T Min(const T &a, const T &b)
{
	return a < b ? a : b;
}

bool AABB::IntersectLineAABB_CPP(const float3 &linePos, const float3 &lineDir, float &tNear, float &tFar) const
{
	// Test each cardinal plane (X, Y and Z) in turn.
	if (!EqualAbs(lineDir.x, 0.f))
	{
		float recipDir = RecipFast(lineDir.x);
		float t1 = (minPoint.x - linePos.x) * recipDir;
		float t2 = (maxPoint.x - linePos.x) * recipDir;

		// tNear tracks distance to intersect (enter) the AABB.
		// tFar tracks the distance to exit the AABB.
		if (t1 < t2)
			tNear = Max(t1, tNear), tFar = Min(t2, tFar);
		else // Swap t1 and t2.
			tNear = Max(t2, tNear), tFar = Min(t1, tFar);

		if (tNear > tFar)
			return false; // Box is missed since we "exit" before entering it.
	}
	else if (linePos.x < minPoint.x || linePos.x > maxPoint.x)
		return false; // The ray can't possibly enter the box, abort.

	if (!EqualAbs(lineDir.y, 0.f))
	{
		float recipDir = RecipFast(lineDir.y);
		float t1 = (minPoint.y - linePos.y) * recipDir;
		float t2 = (maxPoint.y - linePos.y) * recipDir;

		if (t1 < t2)
			tNear = Max(t1, tNear), tFar = Min(t2, tFar);
		else // Swap t1 and t2.
			tNear = Max(t2, tNear), tFar = Min(t1, tFar);

		if (tNear > tFar)
			return false; // Box is missed since we "exit" before entering it.
	}
	else if (linePos.y < minPoint.y || linePos.y > maxPoint.y)
		return false; // The ray can't possibly enter the box, abort.

	if (!EqualAbs(lineDir.z, 0.f)) // ray is parallel to plane in question
	{
		float recipDir = RecipFast(lineDir.z);
		float t1 = (minPoint.z - linePos.z) * recipDir;
		float t2 = (maxPoint.z - linePos.z) * recipDir;

		if (t1 < t2)
			tNear = Max(t1, tNear), tFar = Min(t2, tFar);
		else // Swap t1 and t2.
			tNear = Max(t2, tNear), tFar = Min(t1, tFar);
	}
	else if (linePos.z < minPoint.z || linePos.z > maxPoint.z)
		return false; // The ray can't possibly enter the box, abort.

	return tNear <= tFar;
}

float3 OBB::Size() const
{
	return r * 2.f;
}

float3 OBB::CornerPoint(int cornerIndex) const
{
	switch (cornerIndex)
	{
	default: // For release builds where assume() is disabled, return always the first option if out-of-bounds.
	case 0: return pos - r.x * axis[0] - r.y * axis[1] - r.z * axis[2];
	case 1: return pos - r.x * axis[0] - r.y * axis[1] + r.z * axis[2];
	case 2: return pos - r.x * axis[0] + r.y * axis[1] - r.z * axis[2];
	case 3: return pos - r.x * axis[0] + r.y * axis[1] + r.z * axis[2];
	case 4: return pos + r.x * axis[0] - r.y * axis[1] - r.z * axis[2];
	case 5: return pos + r.x * axis[0] - r.y * axis[1] + r.z * axis[2];
	case 6: return pos + r.x * axis[0] + r.y * axis[1] - r.z * axis[2];
	case 7: return pos + r.x * axis[0] + r.y * axis[1] + r.z * axis[2];
	}
}

float3 Polyhedron::ClosestPoint(const LineSegment &lineSegment, float3 *lineSegmentPt) const
{
	if (Contains(lineSegment.a))
	{
		if (lineSegmentPt)
			*lineSegmentPt = lineSegment.a;
		return lineSegment.a;
	}
	if (Contains(lineSegment.b))
	{
		if (lineSegmentPt)
			*lineSegmentPt = lineSegment.b;
		return lineSegment.b;
	}
	float3 closestPt = float3::nan;
	float closestDistance = FLT_MAX;
	float3 closestLineSegmentPt = float3::nan;
	for (int i = 0; i < NumFaces(); ++i)
	{
		float3 lineSegPt;
		float3 pt = FacePolygon(i).ClosestPoint(lineSegment, &lineSegPt);
		float d = pt.DistanceSq(lineSegPt);
		if (d < closestDistance)
		{
			closestDistance = d;
			closestPt = pt;
			closestLineSegmentPt = lineSegPt;
		}
	}
	if (lineSegmentPt)
		*lineSegmentPt = closestLineSegmentPt;
	return closestPt;
}

float3 Polygon::ClosestPoint(const LineSegment &lineSegment, float3 *lineSegmentPt) const
{
	std::vector<Triangle> tris = Triangulate();
	float3 closestPt = float3::nan;
	float3 closestLineSegmentPt = float3::nan;
	float closestDist = FLT_MAX;
	for (size_t i = 0; i < tris.size(); ++i)
	{
		float3 lineSegPt;
		float3 pt = tris[i].ClosestPoint(lineSegment, &lineSegPt);
		float d = pt.DistanceSq(lineSegPt);
		if (d < closestDist)
		{
			closestPt = pt;
			closestLineSegmentPt = lineSegPt;
			closestDist = d;
		}
	}
	if (lineSegmentPt)
		*lineSegmentPt = closestLineSegmentPt;
	return closestPt;
}

std::vector<Triangle> Polyhedron::Triangulate() const
{
	std::vector<Triangle> outTriangleList;
	for (int i = 0; i < NumFaces(); ++i)
	{
		Polygon p = FacePolygon(i);
		std::vector<Triangle> tris = p.Triangulate();
		outTriangleList.insert(outTriangleList.end(), tris.begin(), tris.end());
	}
	return outTriangleList;
}

Triangle::Triangle(const float3 &a_, const float3 &b_, const float3 &c_)
	:a(a_), b(b_), c(c_)
{
}

MathLibLite::float3 Triangle::ClosestPoint(const LineSegment &lineSegment, float3 *otherPt /*= 0*/) const
{
	///\todo Optimize.
	float3 intersectionPoint;
	if (Intersects(lineSegment, 0, &intersectionPoint))
	{
		if (otherPt)
			*otherPt = intersectionPoint;
		return intersectionPoint;
	}

	float u1, v1, d1;
	float3 pt1 = ClosestPointToTriangleEdge(lineSegment, &u1, &v1, &d1);

	float3 pt2 = ClosestPoint(lineSegment.a);
	float3 pt3 = ClosestPoint(lineSegment.b);

	float D1 = pt1.DistanceSq(lineSegment.GetPoint(d1));
	float D2 = pt2.DistanceSq(lineSegment.a);
	float D3 = pt3.DistanceSq(lineSegment.b);

	if (D1 <= D2 && D1 <= D3)
	{
		if (otherPt)
			*otherPt = lineSegment.GetPoint(d1);
		return pt1;
	}
	else if (D2 <= D3)
	{
		if (otherPt)
			*otherPt = lineSegment.a;
		return pt2;
	}
	else
	{
		if (otherPt)
			*otherPt = lineSegment.b;
		return pt3;
	}
}

bool Triangle::Intersects(const LineSegment &l, float *d, float3 *intersectionPoint) const
{
	/** The Triangle-Line/LineSegment/Ray intersection tests are based on M&ouml;ller-Trumbore method:
	"T. M&ouml;ller, B. Trumbore. Fast, Minimum Storage Ray/Triangle Intersection. 2005."
	http://jgt.akpeters.com/papers/MollerTrumbore97/. */
	float u, v;
	float t = IntersectLineTri(l.a, l.Dir(), a, b, c, u, v);
	bool success = (t >= 0 && t != INFINITY);
	if (!success)
		return false;
	float length = l.LengthSq();
	if (t < 0.f || t*t >= length)
		return false;
	length = sqrt(length);
	if (d)
	{
		float len = t / length;
		*d = len;
		if (intersectionPoint)
			*intersectionPoint = l.GetPoint(len);
	}
	else if (intersectionPoint)
		*intersectionPoint = l.GetPoint(t / length);
	return true;
}

float3 Triangle::ClosestPointToTriangleEdge(const LineSegment &lineSegment, float *outU, float *outV, float *outD) const
{
	///@todo Optimize!
	// The line is parallel to the triangle.
	float d1, d2, d3;
	float3 pt1 = Edge(0).ClosestPoint(lineSegment, 0, &d1);
	float3 pt2 = Edge(1).ClosestPoint(lineSegment, 0, &d2);
	float3 pt3 = Edge(2).ClosestPoint(lineSegment, 0, &d3);
	float dist1 = pt1.DistanceSq(lineSegment.GetPoint(d1));
	float dist2 = pt2.DistanceSq(lineSegment.GetPoint(d2));
	float dist3 = pt3.DistanceSq(lineSegment.GetPoint(d3));
	if (dist1 <= dist2 && dist1 <= dist3)
	{
		if (outU) *outU = BarycentricUV(pt1).x;
		if (outV) *outV = BarycentricUV(pt1).y;
		if (outD) *outD = d1;
		return pt1;
	}
	else if (dist2 <= dist3)
	{
		if (outU) *outU = BarycentricUV(pt2).x;
		if (outV) *outV = BarycentricUV(pt2).y;
		if (outD) *outD = d2;
		return pt2;
	}
	else
	{
		if (outU) *outU = BarycentricUV(pt3).x;
		if (outV) *outV = BarycentricUV(pt3).y;
		if (outD) *outD = d3;
		return pt3;
	}
}

float3 Triangle::ClosestPoint(const float3 &p) const
{
	/** The code for Triangle-float3 test is from Christer Ericson's Real-Time Collision Detection, pp. 141-142. */

	// Check if P is in vertex region outside A.
	float3 ab = b - a;
	float3 ac = c - a;
	float3 ap = p - a;
	float d1 = Dot(ab, ap);
	float d2 = Dot(ac, ap);
	if (d1 <= 0.f && d2 <= 0.f)
		return a; // Barycentric coordinates are (1,0,0).

				  // Check if P is in vertex region outside B.
	float3 bp = p - b;
	float d3 = Dot(ab, bp);
	float d4 = Dot(ac, bp);
	if (d3 >= 0.f && d4 <= d3)
		return b; // Barycentric coordinates are (0,1,0).

				  // Check if P is in edge region of AB, and if so, return the projection of P onto AB.
	float vc = d1*d4 - d3*d2;
	if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
	{
		float v = d1 / (d1 - d3);
		return a + v * ab; // The barycentric coordinates are (1-v, v, 0).
	}

	// Check if P is in vertex region outside C.
	float3 cp = p - c;
	float d5 = Dot(ab, cp);
	float d6 = Dot(ac, cp);
	if (d6 >= 0.f && d5 <= d6)
		return c; // The barycentric coordinates are (0,0,1).

				  // Check if P is in edge region of AC, and if so, return the projection of P onto AC.
	float vb = d5*d2 - d1*d6;
	if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
	{
		float w = d2 / (d2 - d6);
		return a + w * ac; // The barycentric coordinates are (1-w, 0, w).
	}

	// Check if P is in edge region of BC, and if so, return the projection of P onto BC.
	float va = d3*d6 - d5*d4;
	if (va <= 0.f && d4 - d3 >= 0.f && d5 - d6 >= 0.f)
	{
		float w = (d4 - d3) / (d4 - d3 + d5 - d6);
		return b + w * (c - b); // The barycentric coordinates are (0, 1-w, w).
	}

	// P must be inside the face region. Compute the closest point through its barycentric coordinates (u,v,w).
	float denom = 1.f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w;
}

float Triangle::IntersectLineTri(const float3 &linePos, const float3 &lineDir,
	const float3 &v0, const float3 &v1, const float3 &v2,
	float &u, float &v) const
{
	float3 vE1, vE2;
	float3 vT, vP, vQ;

	const float epsilon = 1e-4f;

	// Edge vectors
	vE1 = v1 - v0;
	vE2 = v2 - v0;

	// begin calculating determinant - also used to calculate U parameter
	vP = lineDir.Cross(vE2);

	// If det < 0, intersecting backfacing tri, > 0, intersecting frontfacing tri, 0, parallel to plane.
	const float det = vE1.Dot(vP);

	// If determinant is near zero, ray lies in plane of triangle.
	if (fabs(det) <= epsilon)
		return INFINITY;
	const float recipDet = 1.f / det;

	// Calculate distance from v0 to ray origin
	vT = linePos - v0;

	// Output barycentric u
	u = vT.Dot(vP) * recipDet;
	if (u < -epsilon || u > 1.f + epsilon)
		return INFINITY; // Barycentric U is outside the triangle - early out.

						  // Prepare to test V parameter
	vQ = vT.Cross(vE1);

	// Output barycentric v
	v = lineDir.Dot(vQ) * recipDet;
	if (v < -epsilon || u + v > 1.f + epsilon) // Barycentric V or the combination of U and V are outside the triangle - no intersection.
		return INFINITY;

	// Barycentric u and v are in limits, the ray intersects the triangle.

	// Output signed distance from ray to triangle.
	return vE2.Dot(vQ) * recipDet;
	//	return (det < 0.f) ? IntersectBackface : IntersectFrontface;
}

LineSegment Triangle::Edge(int i) const
{
	if (i == 0)
		return LineSegment(a, b);
	else if (i == 1)
		return LineSegment(b, c);
	else if (i == 2)
		return LineSegment(c, a);
	else
		return LineSegment(float3::nan, float3::nan);
}

float2 Triangle::BarycentricUV(const float3 &point) const
{
	float3 uvw = BarycentricUVW(point);
	return float2(uvw.y, uvw.z);
}

float2::float2(float x_, float y_)
	:x(x_), y(y_)
{
}

inline float TriArea2D(float x1, float y1, float x2, float y2, float x3, float y3)
{
	return (x1 - x2)*(y2 - y3) - (x2 - x3)*(y1 - y2);
}

float3 Triangle::BarycentricUVW(const float3 &point) const
{
	// Implementation from Christer Ericson's Real-Time Collision Detection, pp. 51-52.

	// Unnormalized triangle normal.
	float3 m = Cross(b - a, c - a);

	// Nominators and one-over-denominator for u and v ratios.
	float nu, nv, ood;

	// Absolute components for determining projection plane.
	float x = abs(m.x);
	float y = abs(m.y);
	float z = abs(m.z);

	if (x >= y && x >= z)
	{
		// Project to the yz plane.
		nu = TriArea2D(point.y, point.z, b.y, b.z, c.y, c.z); // Area of PBC in yz-plane.
		nv = TriArea2D(point.y, point.z, c.y, c.z, a.y, a.z); // Area OF PCA in yz-plane.
		ood = 1.f / m.x; // 1 / (2*area of ABC in yz plane)
	}
	else if (y >= z) // Note: The book has a redundant 'if (y >= x)' comparison
	{
		// y is largest, project to the xz-plane.
		nu = TriArea2D(point.x, point.z, b.x, b.z, c.x, c.z);
		nv = TriArea2D(point.x, point.z, c.x, c.z, a.x, a.z);
		ood = 1.f / -m.y;
	}
	else // z is largest, project to the xy-plane.
	{
		nu = TriArea2D(point.x, point.y, b.x, b.y, c.x, c.y);
		nv = TriArea2D(point.x, point.y, c.x, c.y, a.x, a.y);
		ood = 1.f / m.z;
	}
	float u = nu * ood;
	float v = nv * ood;
	float w = 1.f - u - v;
	return float3(u, v, w);
}

float float3::DistanceSq(const float3 &rhs) const
{
	float dx = x - rhs.x;
	float dy = y - rhs.y;
	float dz = z - rhs.z;
	return dx*dx + dy*dy + dz*dz;
}

float3 float3::Cross(const float3 &rhs) const
{
	return float3(y * rhs.z - z * rhs.y,
		z * rhs.x - x * rhs.z,
		x * rhs.y - y * rhs.x);
}

float3 LineSegment::Dir() const
{
	return (b - a).Normalized();
}

float3 LineSegment::GetPoint(float d) const
{
	return (1.f - d) * a + d * b;
}

float LineSegment::LengthSq() const
{
	return a.DistanceSq(b);
}

float3 float3::Normalized() const
{
	float3 copy = *this;
	copy.Normalize();
	return copy;
}

float3 LineSegment::ClosestPoint(const LineSegment &other, float *d, float *d2) const
{
	float u, u2;
	float3 closestPoint = Line::ClosestPointLineLine(a, b, other.a, other.b, &u, &u2);
	if (u >= 0.f && u <= 1.f && u2 >= 0.f && u2 <= 1.f)
	{
		if (d)
			*d = u;
		if (d2)
			*d2 = u2;
		return closestPoint;
	}
	else if (u >= 0.f && u <= 1.f) // Only u2 is out of bounds.
	{
		float3 p;
		if (u2 < 0.f)
		{
			p = other.a;
			if (d2)
				*d2 = 0.f;
		}
		else
		{
			p = other.b;
			if (d2)
				*d2 = 1.f;
		}

		return ClosestPoint(p, d);
	}
	else if (u2 >= 0.f && u2 <= 1.f) // Only u is out of bounds.
	{
		float3 p;
		if (u < 0.f)
		{
			p = a;
			if (d)
				*d = 0.f;
		}
		else
		{
			p = b;
			if (d)
				*d = 1.f;
		}

		if (d2)
			other.ClosestPoint(p, d2);
		return p;
	}
	else // Both u and u2 are out of bounds.
	{
		float3 p;
		float t;
		if (u < 0.f)
		{
			p = a;
			t = 0.f;
		}
		else
		{
			p = b;
			t = 1.f;
		}

		float3 p2;
		float t2;
		if (u2 < 0.f)
		{
			p2 = other.a;
			t2 = 0.f;
		}
		else
		{
			p2 = other.b;
			t2 = 1.f;
		}

		float T, T2;
		closestPoint = ClosestPoint(p2, &T);
		float3 closestPoint2 = other.ClosestPoint(p, &T2);

		if (closestPoint.DistanceSq(p2) <= closestPoint2.DistanceSq(p))
		{
			if (d)
				*d = T;
			if (d2)
				*d2 = t2;
			return closestPoint;
		}
		else
		{
			if (d)
				*d = t;
			if (d2)
				*d2 = T2;
			return p;
		}
	}
}

float Dmnop(const float3 *v, int m, int n, int o, int p)
{
	return (v[m].x - v[n].x) * (v[o].x - v[p].x) + (v[m].y - v[n].y) * (v[o].y - v[p].y) + (v[m].z - v[n].z) * (v[o].z - v[p].z);
}

float3 Line::ClosestPointLineLine(float3 start0, float3 end0, float3 start1, float3 end1, float *d, float *d2)
{
	const float3 v[4] = { start0, end0, start1, end1 };

	float d0232 = Dmnop(v, 0, 2, 3, 2);
	float d3210 = Dmnop(v, 3, 2, 1, 0);
	float d3232 = Dmnop(v, 3, 2, 3, 2);
	float mu = (d0232 * d3210 - Dmnop(v, 0, 2, 1, 0)*d3232) / (Dmnop(v, 1, 0, 1, 0)*Dmnop(v, 3, 2, 3, 2) - Dmnop(v, 3, 2, 1, 0)*Dmnop(v, 3, 2, 1, 0));
	if (d)
		*d = mu;

	if (d2)
		*d2 = (d0232 + mu * d3210) / d3232;

	return start0 + mu * (end0 - start0);
}

float3 LineSegment::ClosestPoint(const float3 &point, float *d) const
{
	float3 dir = b - a;
	float u = Clamp01(Dot(point - a, dir) / dir.LengthSq());
	if (d)
		*d = u;
	return a + u * dir;
}

bool IntersectLineLine2D(const float2 &a1, const float2 &a2, const float2 &b1, const float2 &b2, float2 &out)
{
	float u = (b2.x - b1.x)*(a1.y - b1.y) - (b2.y - b1.y)*(a1.x - b1.x);
	float v = (a2.x - a1.x)*(a1.y - b1.y) - (a2.y - a1.y)*(a1.x - b1.x);

	float det = (b2.y - b1.y)*(a2.x - a1.x) - (b2.x - b1.x)*(a2.y - a1.y);
	if (abs(det) < 1e-4f)
		return false;
	det = 1.f / det;
	out.x = u * det;
	out.y = v * det;

	return true;
}

bool IntersectLineSegmentLineSegment2D(const float2 &a1, const float2 &a2, const float2 &b1, const float2 &b2, float2 &out)
{
	bool ret = IntersectLineLine2D(a1, a2, b1, b2, out);
	return ret && out.x >= 0.f && out.x <= 1.f && out.y >= 0.f && out.y <= 1.f;
}

bool IsAnEar(const std::vector<float2> &poly, int i, int j)
{
	float2 dummy;
	int x = (int)poly.size() - 1;
	for (int y = 0; y < i; ++y)
	{
		if (IntersectLineSegmentLineSegment2D(poly[i], poly[j], poly[x], poly[y], dummy))
			return false;
		x = y;
	}
	x = j + 1;
	for (int y = x + 1; y < (int)poly.size(); ++y)
	{
		if (IntersectLineSegmentLineSegment2D(poly[i], poly[j], poly[x], poly[y], dummy))
			return false;
		x = y;
	}
	return true;
}

std::vector<Triangle> Polygon::Triangulate() const
{
	std::vector<Triangle> t;
	// Handle degenerate cases.
	if (NumVertices() < 3)
		return t;
	if (NumVertices() == 3)
	{
		t.push_back(Triangle(Vertex(0), Vertex(1), Vertex(2)));
		return t;
	}
	std::vector<float2> p2d;
	std::vector<int> polyIndices;
	for (int v = 0; v < NumVertices(); ++v)
	{
		p2d.push_back(MapTo2D(v));
		polyIndices.push_back(v);
	}

	// Clip ears of the polygon until it has been reduced to a triangle.
	int i = 0;
	int j = 1;
	int k = 2;
	size_t numTries = 0; // Avoid creating an infinite loop.
	while (p2d.size() > 3 && numTries < p2d.size())
	{
		if (float2::OrientedCCW(p2d[i], p2d[j], p2d[k]) && IsAnEar(p2d, i, k))
		{
			// The vertex j is an ear. Clip it off.
			t.push_back(Triangle(p[polyIndices[i]], p[polyIndices[j]], p[polyIndices[k]]));
			p2d.erase(p2d.begin() + j);
			polyIndices.erase(polyIndices.begin() + j);

			// The previous index might now have become an ear. Move back one index to see if so.
			if (i > 0)
			{
				i = (i + (int)p2d.size() - 1) % p2d.size();
				j = (j + (int)p2d.size() - 1) % p2d.size();
				k = (k + (int)p2d.size() - 1) % p2d.size();
			}
			numTries = 0;
		}
		else
		{
			// The vertex at j is not an ear. Move to test next vertex.
			i = j;
			j = k;
			k = (k + 1) % p2d.size();
			++numTries;
		}
	}

	if (p2d.size() > 3) // If this occurs, then the polygon is NOT counter-clockwise oriented.
		return t;
	/*
	{
	// For conveniency, create a copy that has the winding order fixed, and triangulate that instead.
	// (Causes a large performance hit!)
	Polygon p2 = *this;
	for(size_t i = 0; i < p2.p.size()/2; ++i)
	std::swap(p2.p[i], p2.p[p2.p.size()-1-i]);
	return p2.Triangulate();
	}
	*/
	// Add the last poly.
	t.push_back(Triangle(p[polyIndices[0]], p[polyIndices[1]], p[polyIndices[2]]));

	return t;
}

float3 Polygon::Vertex(int vertexIndex) const
{
	return p[vertexIndex];
}

float2 Polygon::MapTo2D(int i) const
{
	return MapTo2D(p[i]);
}

float2 Polygon::MapTo2D(const float3 &point) const
{
	assume(!p.empty());
	float3 basisU = BasisU();
	float3 basisV = BasisV();
	float3 pt = point - p[0];
	return float2(Dot(pt, basisU), Dot(pt, basisV));
}

float3 Polygon::BasisU() const
{
	if (p.size() < 2)
		return float3::unitX;
	float3 u = p[1] - p[0];
	u.Normalize(); // Always succeeds, even if u was zero (generates (1,0,0)).
	return u;
}

float3 Polygon::BasisV() const
{
	if (p.size() < 2)
		return float3::unitY;
	return Cross(PlaneCCW().normal, BasisU()).Normalized();
}

bool float2::OrientedCCW(const float2 &a, const float2 &b, const float2 &c)
{
	return (a.x - c.x)*(b.y - c.y) - (a.y - c.y)*(b.x - c.x) >= 0.f;
}

Plane Polygon::PlaneCCW() const
{
	if (p.size() > 3)
	{
		for (size_t i = 0; i < p.size(); ++i)
			for (size_t j = i + 1; j < p.size(); ++j)
				for (size_t k = j + 1; k < p.size(); ++k)
					if (!float3::AreCollinear(p[i], p[j], p[k]))
						return Plane(p[i], p[j], p[k]);

		// Polygon contains multiple points, but they are all collinear.
		// Pick an arbitrary plane along the line as the polygon plane (as if the polygon had only two points)
		return Plane(Line(p[0], p[1]), (p[0] - p[1]).Perpendicular());
	}
	if (p.size() == 3)
		return Plane(p[0], p[1], p[2]);
	if (p.size() == 2)
		return Plane(Line(p[0], p[1]), (p[0] - p[1]).Perpendicular());
	if (p.size() == 1)
		return Plane(p[0], float3(0, 1, 0));
	return Plane();
}

Plane::Plane(const float3 &v1, const float3 &v2, const float3 &v3)
{
	Set(v1, v2, v3);
}

Plane::Plane(const Line &line, const float3 &normal)
{
	float3 perpNormal = normal - normal.ProjectToNorm(line.dir);
	Set(line.pos, perpNormal.Normalized());
}

Plane::Plane(const float3 &point, const float3 &normal)
{
	Set(point, normal);
}

void Plane::Set(const float3 &v1, const float3 &v2, const float3 &v3)
{
	normal = ((v2 - v1).Cross(v3 - v1)).Normalized();
	d = Dot(v1, normal);
}

void Plane::Set(const float3 &point, const float3 &normal_)
{
	normal = normal_;
	d = Dot(point, normal);
}

bool MUST_USE_RESULT float3::AreCollinear(const float3 &p1, const float3 &p2, const float3 &p3, float epsilon)
{
	return (p2 - p1).Cross(p3 - p1).LengthSq() <= epsilon;
}

float3 float3::Perpendicular(const float3 &hint, const float3 &hint2) const
{
	float3 v = this->Cross(hint);
	float len = v.Normalize();
	if (len == 0)
		return hint2;
	else
		return v;
}

float3 float3::ProjectToNorm(const float3 &direction) const
{
	return direction * this->Dot(direction);
}

Polygon Polyhedron::FacePolygon(int faceIndex) const
{
	Polygon p;
	p.p.reserve(f[faceIndex].v.size());
	for (size_t v1 = 0; v1 < f[faceIndex].v.size(); ++v1)
		p.p.push_back(Vertex(f[faceIndex].v[v1]));
	return p;
}

float3 Polyhedron::Vertex(int vertexIndex) const
{
	return v[vertexIndex];
}

bool Polyhedron::Contains(const float3 &point) const
{
	int numIntersections = 0;
	for (int i = 0; i < (int)f.size(); ++i)
	{
		Plane p(v[f[i].v[0]] - point, v[f[i].v[1]] - point, v[f[i].v[2]] - point);

		// Find the intersection of the plane and the ray (0,0,0) -> (t,0,0), t >= 0.
		// <normal, point_on_ray> == d
		// n.x * t == d
		//       t == d / n.x
		if (abs(p.normal.x) > 1e-5f)
		{
			float t = p.d / p.normal.x;
			// If t >= 0, the plane and the ray intersect, and the ray potentially also intersects the polygon.
			// Finish the test by checking whether the point of intersection is contained in the polygon, in
			// which case the ray-polygon intersection occurs.
			if (t >= 0.f && FaceContains(i, point + float3(t, 0, 0)))
				++numIntersections;
		}
	}

	return numIntersections % 2 == 1;
}

bool Polyhedron::FaceContains(int faceIndex, const float3 &worldSpacePoint, float polygonThickness) const
{
	// N.B. This implementation is a duplicate of Polygon::Contains, but adapted to avoid dynamic memory allocation
	// related to converting the face of a Polyhedron to a Polygon object.

	// Implementation based on the description from http://erich.realtimerendering.com/ptinpoly/

	const Face &face = f[faceIndex];
	const std::vector<int> &vertices = face.v;

	if (vertices.size() < 3)
		return false;

	Plane p = FacePlane(faceIndex);
	if (FacePlane(faceIndex).Distance(worldSpacePoint) > polygonThickness)
		return false;

	int numIntersections = 0;

	float3 basisU = v[vertices[1]] - v[vertices[0]];
	basisU.Normalize();
	float3 basisV = Cross(p.normal, basisU).Normalized();

	float2 localSpacePoint = float2(Dot(worldSpacePoint, basisU), Dot(worldSpacePoint, basisV));

	const float epsilon = 1e-4f;

	float2 p0 = float2(Dot(v[vertices.back()], basisU), Dot(v[vertices.back()], basisV)) - localSpacePoint;
	if (abs(p0.y) < epsilon)
		p0.y = -epsilon; // Robustness check - if the ray (0,0) -> (+inf, 0) would pass through a vertex, move the vertex slightly.
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		float2 p1 = float2(Dot(v[vertices[i]], basisU), Dot(v[vertices[i]], basisV)) - localSpacePoint;
		if (abs(p1.y) < epsilon)
			p1.y = -epsilon; // Robustness check - if the ray (0,0) -> (+inf, 0) would pass through a vertex, move the vertex slightly.

		if (p0.y * p1.y < 0.f)
		{
			if (p0.x > 1e-3f && p1.x > 1e-3f)
				++numIntersections;
			else
			{
				// P = p0 + t*(p1-p0) == (x,0)
				//     p0.x + t*(p1.x-p0.x) == x
				//     p0.y + t*(p1.y-p0.y) == 0
				//                 t == -p0.y / (p1.y - p0.y)

				// Test whether the lines (0,0) -> (+inf,0) and p0 -> p1 intersect at a positive X-coordinate.
				float2 d = p1 - p0;
				if (abs(d.y) > 1e-5f)
				{
					float t = -p0.y / d.y;
					float x = p0.x + t * d.x;
					if (t >= 0.f && t <= 1.f && x > 1e-6f)
						++numIntersections;
				}
			}
		}
		p0 = p1;
	}

	return numIntersections % 2 == 1;
}

float Plane::Distance(const float3 &point) const
{
	return abs(SignedDistance(point));
}

float Plane::SignedDistance(const float3 &point) const
{
	return normal.Dot(point) - d;
}

float2 float2::operator -(const float2 &rhs) const
{
	return float2(x - rhs.x, y - rhs.y);
}

Plane Polyhedron::FacePlane(int faceIndex) const
{
	const Face &face = f[faceIndex];
	if (face.v.size() >= 3)
		return Plane(v[face.v[0]], v[face.v[1]], v[face.v[2]]);
	else if (face.v.size() == 2)
		return Plane(Line(v[face.v[0]], v[face.v[1]]), (v[face.v[0]] - v[face.v[1]]).Perpendicular());
	else if (face.v.size() == 1)
		return Plane(v[face.v[0]], float3(0, 1, 0));
	else
		return Plane();
}

}