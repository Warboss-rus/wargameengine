#pragma once

namespace MathLibLite
{

class Triangle;
class Plane;
class Line;
class LineSegment;
class AABB;
class OBB;
class Polygon;
class Polyhedron;
class float3x4;
class float2;

template<int Cols>
class MatrixProxy
{
private:
	float v[Cols];

public:
	const float operator[](int col) const
	{
		return v[col];
	}
	float &operator[](int col)
	{
		return v[col];
	}
};

class float3
{
public:
	enum{Size = 3};
	float x;
	float y;
	float z;
	float3() {}
	float3(float x, float y, float z);
	explicit float3(const float *data);
	float *ptr();
	const float *ptr() const;
	float &operator [](int index) { return At(index); }
	const float operator [](int index) const { return At(index); }
	float &At(int index);
	const float At(int index) const;
	float3 operator +(const float3 &v) const;
	float3 operator -() const;
	float3 operator -(const float3 &v) const;
	float3 operator *(float scalar) const;
	float3 operator +() const { return *this; }
	float3 &operator +=(const float3 &v);
	float3 &operator *=(float scalar);
	void Set(float x, float y, float z);
	float Length() const;
	float LengthSq() const;
	float Normalize();
	float3 Normalized() const;
	bool IsNormalized(float epsilonSq = 1e-6f) const;
	bool IsFinite() const;
	bool IsPerpendicular(const float3 &other, float epsilon = 1e-3f) const;
	static bool AreCollinear(const float3 &p1, const float3 &p2, const float3 &p3, float epsilon = 1e-4f);
	bool Equals(const float3 &other, float epsilon = 1e-3f) const;
	bool Equals(float x, float y, float z, float epsilon = 1e-3f) const;
	float3 Abs() const;
	float DistanceSq(const float3 &point) const;
	float Dot(const float3 &v) const;
	float3 Cross(const float3 &v) const;
	float3 Perpendicular(const float3 &hint = float3(0, 1, 0), const float3 &hint2 = float3(0, 0, 1)) const;
	float3 ProjectToNorm(const float3 &direction) const;
	static bool AreOrthonormal(const float3 &a, const float3 &b, const float3 &c, float epsilon = 1e-3f);
	static const float3 unitX;
	static const float3 unitY;
	static const float3 unitZ;
	static const float3 nan;
};

class float3x3
{
public:
	enum { Rows = 3 };
	enum { Cols = 3 };
	float v[Rows][Cols];
	float3x3() {}
	float3x3(float _00, float _01, float _02,
		float _10, float _11, float _12,
		float _20, float _21, float _22);
	const float3 Col(int col) const;
	float *ptr();
	const float *ptr() const;
	void Set(float _00, float _01, float _02,
		float _10, float _11, float _12,
		float _20, float _21, float _22);
	float3 operator *(const float3 &rhs) const;
	bool IsColOrthogonal(float epsilon = 1e-3f) const;

	float3 Mul(const float3 &rhs) const;
	float3 MulPos(const float3 &rhs) const { return Mul(rhs); }
	float3 MulDir(const float3 &rhs) const { return Mul(rhs); }

	MatrixProxy<Cols> &operator[](int row);
};

class AABB
{
public:
	float3 minPoint;
	float3 maxPoint;
	AABB(const float3 &minPoint, const float3 &maxPoint);
	float3 CenterPoint() const;
	float3 Size() const;
	float3 HalfSize() const;
	bool Intersects(const Line &line) const;
	bool IntersectLineAABB_CPP(const float3 &linePos, const float3 &lineDir, float &tNear, float &tFar) const;
};

class OBB
{
public:
	float3 pos;
	float3 r;
	float3 axis[3];
	OBB(const AABB &aabb);
	void SetFrom(const AABB &aabb);
	Polyhedron ToPolyhedron() const;
	float3 Size() const;
	float3 CenterPoint() const;
	float3 CornerPoint(int cornerIndex) const;
	void Translate(const float3 &offset);
	void Scale(const float3 &centerPoint, float scaleFactor);
	void Scale(const float3 &centerPoint, const float3 &scaleFactor);
	void Transform(const float3x3 &transform);
	void Transform(const float3x4 &transform);
	bool Intersects(const OBB &obb, float epsilon = 1e-3f) const;
	bool Intersects(const AABB &aabb) const;
	bool Intersects(const Line &line) const;
	float3x4 WorldToLocal() const;
	float3x4 LocalToWorld() const;
};

class Line
{
public:
	float3 pos;
	float3 dir;
	Line(const float3 &pos, const float3 &dir);
	static float3 ClosestPointLineLine(float3 start0, float3 end0, float3 start1, float3 end1, float *d, float *d2);
};

class LineSegment
{
public:
	float3 a;
	float3 b;
	LineSegment(const float3 &a, const float3 &b);
	float3 GetPoint(float d) const;
	float3 Dir() const;
	float LengthSq() const;
	float3 ClosestPoint(const float3 &point, float *d = 0) const;
	float3 ClosestPoint(const LineSegment &other, float *d = 0, float *d2 = 0) const;
};

class Polyhedron
{
public:
	struct Face
	{
		std::vector<int> v;
	};
	std::vector<float3> v;
	std::vector<Face> f;
	int NumFaces() const { return (int)f.size(); }
	float3 Vertex(int vertexIndex) const;
	Polygon FacePolygon(int faceIndex) const;
	Plane FacePlane(int faceIndex) const;
	bool Contains(const float3 &point) const;
	bool FaceContains(int faceIndex, const float3 &worldSpacePoint, float polygonThickness = 1e-3f) const;
	float3 ClosestPoint(const LineSegment &lineSegment, float3 *lineSegmentPt) const;
	float3 ClosestPoint(const LineSegment &lineSegment) const;
	std::vector<Triangle> Triangulate() const;
};

class ScaleOp
{
public:
	float x;
	float y;
	float z;
	ScaleOp(const float3 &scale);
	float3x4 ToFloat3x4() const;
};

class TranslateOp
{
public:
	float x;
	float y;
	float z;
	TranslateOp(const float3 &trans);
	float3 Offset() const;
};

class float3x4
{
public:
	enum { Rows = 3 };
	enum { Cols = 4 };
	float v[Rows][Cols];
	static float3x4 Scale(const float3 &scale, const float3 &scaleCenter);
	static ScaleOp Scale(const float3 &scale);
	static TranslateOp Translate(const float3 &offset);
	const float3 Col(int col) const;
	bool IsColOrthogonal(float epsilon = 1e-3f) const;
	bool IsOrthonormal(float epsilon = 1e-3f) const;
	void InverseOrthonormal();
	void SetTranslatePart(const float3 &offset);
	void SetCol(int column, float m_0c, float m_1c, float m_2c);
	void SetCol(int column, const float3 &columnVector);
	MatrixProxy<float3x4::Cols> &operator[](int row);
	const MatrixProxy<float3x4::Cols> &operator[](int row) const;
	bool Equals(const float3x4 &other, float epsilon) const;
	float3 TransformDir(float x, float y, float z) const;
	float3 TransformDir(const float3 &directionVector) const;
	void SetRow(int row, float m_r0, float m_r1, float m_r2, float m_r3);
	float3 MulPos(const float3 &pointVector) const;
	float3 TransformPos(float x, float y, float z) const;
	float3 TransformPos(const float3 &pointVector) const;
	float3 MulDir(const float3 &directionVector) const;
	const float3 & Row3(int row) const;
	float3 & Row3(int row);
};

class Polygon
{
public:
	Polygon() {}
	std::vector<float3> p;
	float3 ClosestPoint(const LineSegment &lineSegment, float3 *lineSegmentPt) const;
	std::vector<Triangle> Triangulate() const;
	int NumVertices() const{return (int)p.size();}
	float3 Vertex(int vertexIndex) const;
	float2 MapTo2D(int i) const;
	float2 MapTo2D(const float3 &point) const;
	float3 BasisU() const;
	float3 BasisV() const;
	Plane PlaneCCW() const;
};

class float2
{
public:
	enum { Size = 2 };
	float x;
	float y;
	float2() {}
	float2(float x, float y);
	static bool OrientedCCW(const float2 &a, const float2 &b, const float2 &c);
	float2 operator -(const float2 &rhs) const;
};

class Triangle
{
public:
	float3 a;
	float3 b;
	float3 c;
	Triangle(const float3 &a, const float3 &b, const float3 &c);
	float3 ClosestPoint(const LineSegment &lineSegment, float3 *otherPt = 0) const;
	bool Intersects(const LineSegment &l, float *d, float3 *intersectionPoint) const;
	float3 ClosestPointToTriangleEdge(const LineSegment &lineSegment, float *outU, float *outV, float *outD) const;
	float3 ClosestPoint(const float3 &p) const;
	float IntersectLineTri(const float3 &linePos, const float3 &lineDir,
		const float3 &v0, const float3 &v1, const float3 &v2,
		float &u, float &v) const;
	LineSegment Edge(int i) const;

	float3 BarycentricUVW(const float3 &point) const;
	float2 BarycentricUV(const float3 &point) const;
};

class Plane
{
public:
	float3 normal;
	float d;
	Plane() {}
	Plane(const float3 &v1, const float3 &v2, const float3 &v3);
	Plane(const Line &line, const float3 &normal);
	Plane(const float3 &point, const float3 &normal);

	void Set(const float3 &v1, const float3 &v2, const float3 &v3);
	void Set(const float3 &point, const float3 &normal_);
	float Distance(const float3 &point) const;
	float SignedDistance(const float3 &point) const;
};

Line operator *(const float3x4 &transform, const Line &line);
inline float Dot(const float3 &a, const float3 &b) { return a.Dot(b); }
inline float3 Abs(const float3 &a) { return a.Abs(); }
inline float3 Cross(const float3 &a, const float3 &b) { return a.Cross(b); }
}