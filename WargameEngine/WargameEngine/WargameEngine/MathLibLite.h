#pragma once

namespace MathLibLite
{

#define ALIGN16 __declspec(align(16))
#define ALIGN32 __declspec(align(32))
#define ALIGN64 __declspec(align(64))
#define MUST_USE_RESULT _Check_return_

class Ray;
class LCG;
class Plane;
class Triangle;
class Sphere;
class Capsule;
class Quat;
class SqaleOp;
class Line;
class LineSegment;
class AABB;
class OBB;
class Polygon;
class Frustum;
class Circle;
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
	enum
	{
		/// Specifies the number of elements in this vector.
		Size = 3
	};
	/// The x component.
	/** A float3 is 12 bytes in size. This element lies in the memory offsets 0-3 of this class. */
	float x;
	/// The y component. [similarOverload: x]
	/** This element is packed to the memory offsets 4-7 of this class. */
	float y;
	/// The z component. [similarOverload: x]
	/** This element is packed to the memory offsets 8-11 of this class. */
	float z;

	/// The default constructor does not initialize any members of this class.
	/** This means that the values of the members x, y and z are all undefined after creating a new float3 using
	this default constructor. Remember to assign to them before use.
	@see x, y, z. */
	float3() {}

	/// Constructs a new float3 with the value (x, y, z).
	/** @see x, y, z. */
	float3(float x, float y, float z);

	/// Constructs a new float3 with the value (scalar, scalar, scalar).
	/** @see x, y, z. */
	explicit float3(float scalar);

	/// Constructs a new float3 with the value (xy.x, xy.y, z).
	float3(const float2 &xy, float z);

	/// Constructs this float3 from a C array, to the value (data[0], data[1], data[2]).
	/** @param data An array containing three elements for x, y and z. This pointer may not be null. */
	explicit float3(const float *data);

	/// Casts this float3 to a C array.
	/** This function does not allocate new memory or make a copy of this float3. This function simply
	returns a C pointer view to this data structure. Use ptr()[0] to access the x component of this float3,
	ptr()[1] to access y, and ptr()[2] to access the z component of this float3.
	@note Since the returned pointer points to this class, do not dereference the pointer after this
	float3 has been deleted. You should never store a copy of the returned pointer.
	@note This function is provided for compatibility with other APIs which require raw C pointer access
	to vectors. Avoid using this function in general, and instead always use the operator [] or
	the At() function to access the elements of this vector by index.
	@return A pointer to the first float element of this class. The data is contiguous in memory.
	@see operator [](), At(). */
	float *ptr();
	const float *ptr() const;

	/// Accesses an element of this vector using array notation.
	/** @param index The element to get. Pass in 0 for x, 1 for y and 2 for z.
	@note If you have a non-const instance of this class, you can use this notation to set the elements of
	this vector as well, e.g. vec[1] = 10.f; would set the y-component of this vector.
	@see ptr(), At(). */
	float &operator [](int index) { return At(index); }
	const float operator [](int index) const { return At(index); }

	/// Accesses an element of this vector.
	/** @param index The element to get. Pass in 0 for x, 1 for y, and 2 for z.
	@note If you have a non-const instance of this class, you can use this notation to set the elements of
	this vector as well, e.g. vec.At(1) = 10.f; would set the y-component of this vector.
	@see ptr(), operator [](). */
	float &At(int index);
	const float At(int index) const;

	/// Adds two vectors. [indexTitle: operators +,-,*,/]
	/** This function is identical to the member function Add().
	@return float3(x + v.x, y + v.y, z + v.z); */
	float3 operator +(const float3 &v) const;
	/// Performs an unary negation of this vector. [similarOverload: operator+] [hideIndex]
	/** This function is identical to the member function Neg().
	@return float3(-x, -y, -z). */
	float3 operator -() const;
	/// Subtracts the given vector from this vector. [similarOverload: operator+] [hideIndex]
	/** This function is identical to the member function Sub().
	@return float3(x - v.x, y - v.y, z - v.z); */
	float3 operator -(const float3 &v) const;
	/// Multiplies this vector by a scalar. [similarOverload: operator+] [hideIndex]
	/** This function is identical to the member function Mul().
	@return float3(x * scalar, y * scalar, z * scalar); */
	float3 operator *(float scalar) const;
	/// Divides this vector by a scalar. [similarOverload: operator+] [hideIndex]
	/** This function is identical to the member function Div().
	@return float3(x / scalar, y / scalar, z / scalar); */
	float3 operator /(float scalar) const;
	/// Unary operator + allows this structure to be used in an expression '+x'.
	float3 operator +() const { return *this; }

	/// Adds a vector to this vector, in-place. [indexTitle: operators +=,-=,*=,/=]
	/** @return A reference to this. */
	float3 &operator +=(const float3 &v);
	/// Subtracts a vector from this vector, in-place. [similarOverload: operator+=] [hideIndex]
	/** @return A reference to this. */
	float3 &operator -=(const float3 &v);
	/// Multiplies this vector by a scalar, in-place. [similarOverload: operator+=] [hideIndex]
	/** @return A reference to this. */
	float3 &operator *=(float scalar);
	/// Divides this vector by a scalar, in-place. [similarOverload: operator+=] [hideIndex]
	/** @return A reference to this. */
	float3 &operator /=(float scalar);

	/// Adds a vector to this vector. [IndexTitle: Add/Sub/Mul/Div]
	/// @return (x+v.x, y+v.y, z+v.z).
	float3 Add(const float3 &v) const { return *this + v; }

	/// Adds the vector (s,s,s) to this vector.
	/// @note Mathematically, the addition of a vector and scalar is not defined in linear space structures,
	///	 but this function is provided here for syntactical convenience.
	/// @return (x+s, y+s, z+s).
	float3 Add(float s) const;

	/// Subtracts a vector from this vector. [similarOverload: Add] [hideIndex]
	/// @return (x-v.x, y-v.y, z-v.z).
	float3 Sub(const float3 &v) const { return *this - v; }

	/// Subtracts the vector (s,s,s) from this vector. [similarOverload: Add] [hideIndex]
	/// @note Mathematically, the subtraction of a vector by a scalar is not defined in linear space structures,
	///	 but this function is provided here for syntactical convenience.
	/// @return (x-s, y-s, z-s).
	float3 Sub(float s) const;

	/// Subtracts this vector from the vector (s,s,s). [similarOverload: Add] [hideIndex]
	/// @note Mathematically, the subtraction of a scalar by a vector is not defined in linear space structures,
	///	 but this function is provided here for syntactical convenience.
	/// @return (s-x, s-y, s-z).
	float3 SubLeft(float s) const;

	/// Multiplies this vector by a vector, element-wise. [similarOverload: Add] [hideIndex]
	/// @note Mathematically, the multiplication of two vectors is not defined in linear space structures,
	///	 but this function is provided here for syntactical convenience.
	/// @return (x*v.x, y*v.y, z*v.z).
	float3 Mul(const float3 &v) const;

	/// Multiplies this vector by a scalar. [similarOverload: Add] [hideIndex]
	/// @return (x*s, y*s, z*s).
	float3 Mul(float s) const { return *this * s; }

	/// Divides this vector by a vector, element-wise. [similarOverload: Add] [hideIndex]
	/// @note Mathematically, the division of two vectors is not defined in linear space structures,
	///	 but this function is provided here for syntactical convenience.
	/// @return (x/v.x, y/v.y, z/v.z).
	float3 Div(const float3 &v) const;

	/// Divides this vector by a scalar. [similarOverload: Add] [hideIndex]
	/// @return (x/s, y/s, z/s).
	float3 Div(float s) const { return *this / s; }

	/// Divides the vector (s,s,s) by this vector, element-wise. [similarOverload: Add] [hideIndex]
	/// @note Mathematically, the division of a scalar by a vector is not defined in linear space structures,
	///	 but this function is provided here for syntactical convenience.
	/// @return (s/x, s/y, s/z).
	float3 DivLeft(float s) const;

	/// Performs a 2D swizzled access to this vector. [indexTitle: xx/xy/xz/..]
	/** The xy(), yz(), etc.. functions return a float2 which is a permuted selection of two elements of this float3.
	The xx(), yy(), zz() functions return a float2 which have the same scalar for both elements. */
	float2 xx() const;
	float2 xy() const; ///< [similarOverload: xx] [hideIndex]
	float2 xz() const; ///< [similarOverload: xx] [hideIndex]
	float2 yx() const; ///< [similarOverload: xx] [hideIndex]
	float2 yy() const; ///< [similarOverload: xx] [hideIndex]
	float2 yz() const; ///< [similarOverload: xx] [hideIndex]
	float2 zx() const; ///< [similarOverload: xx] [hideIndex]
	float2 zy() const; ///< [similarOverload: xx] [hideIndex]
	float2 zz() const; ///< [similarOverload: xx] [hideIndex]

						/// Performs a 3D swizzled access to this vector. [indexTitle: xyz/xzy/yzx/..]
						/** Use these functions to permute or replace elements of this vector with others.
						@return The function abc() returns float3(a,b,c). */
	float3 xxx() const { return float3(x, x, x); }
	float3 xxy() const { return float3(x, x, y); } ///< [similarOverload: xxx] [hideIndex]
	float3 xxz() const { return float3(x, x, z); } ///< [similarOverload: xxx] [hideIndex]
	float3 xyx() const { return float3(x, y, x); } ///< [similarOverload: xxx] [hideIndex]
	float3 xyy() const { return float3(x, y, y); } ///< [similarOverload: xxx] [hideIndex]
	float3 xyz() const { return float3(x, y, z); } ///< [similarOverload: xxx] [hideIndex]
	float3 xzx() const { return float3(x, z, x); } ///< [similarOverload: xxx] [hideIndex]
	float3 xzy() const { return float3(x, z, y); } ///< [similarOverload: xxx] [hideIndex]
	float3 xzz() const { return float3(x, z, z); } ///< [similarOverload: xxx] [hideIndex]

	float3 yxx() const { return float3(y, x, x); } ///< [similarOverload: xxx] [hideIndex]
	float3 yxy() const { return float3(y, x, y); } ///< [similarOverload: xxx] [hideIndex]
	float3 yxz() const { return float3(y, x, z); } ///< [similarOverload: xxx] [hideIndex]
	float3 yyx() const { return float3(y, y, x); } ///< [similarOverload: xxx] [hideIndex]
	float3 yyy() const { return float3(y, y, y); } ///< [similarOverload: xxx] [hideIndex]
	float3 yyz() const { return float3(y, y, z); } ///< [similarOverload: xxx] [hideIndex]
	float3 yzx() const { return float3(y, z, x); } ///< [similarOverload: xxx] [hideIndex]
	float3 yzy() const { return float3(y, z, y); } ///< [similarOverload: xxx] [hideIndex]
	float3 yzz() const { return float3(y, z, z); } ///< [similarOverload: xxx] [hideIndex]

	float3 zxx() const { return float3(z, x, x); } ///< [similarOverload: xxx] [hideIndex]
	float3 zxy() const { return float3(z, x, y); } ///< [similarOverload: xxx] [hideIndex]
	float3 zxz() const { return float3(z, x, z); } ///< [similarOverload: xxx] [hideIndex]
	float3 zyx() const { return float3(z, y, x); } ///< [similarOverload: xxx] [hideIndex]
	float3 zyy() const { return float3(z, y, y); } ///< [similarOverload: xxx] [hideIndex]
	float3 zyz() const { return float3(z, y, z); } ///< [similarOverload: xxx] [hideIndex]
	float3 zzx() const { return float3(z, z, x); } ///< [similarOverload: xxx] [hideIndex]
	float3 zzy() const { return float3(z, z, y); } ///< [similarOverload: xxx] [hideIndex]
	float3 zzz() const { return float3(z, z, z); } ///< [similarOverload: xxx] [hideIndex]

													/// Performs a swizzled access to this vector.
													/** For example, Swizzled(2,1,0) return float3(z,y,x). Swizzled(2,2,2,2) returns float4(z,z,z,z).
													@param i Chooses the element of this vector to pick for the x value of the returned vector, in the range [0, 2].
													@param j Chooses the element of this vector to pick for the y value of the returned vector, in the range [0, 2].
													@param k Chooses the element of this vector to pick for the z value of the returned vector, in the range [0, 2].
													@param l Chooses the element of this vector to pick for the w value of the returned vector, in the range [0, 2]. */
	float3 Swizzled(int i, int j, int k) const;
	float2 Swizzled(int i, int j) const;

	/// Generates a new float3 by filling its entries by the given scalar.
	/** @see float3::float3(float scalar), SetFromScalar(). */
	static MUST_USE_RESULT float3 FromScalar(float scalar);

	/// Fills each entry of this float3 by the given scalar.
	/** @see float3::float3(float scalar), FromScalar(). */
	void SetFromScalar(float scalar);

	/// Sets all elements of this vector.
	/** @see x, y, z, At(). */
	void Set(float x, float y, float z);

	/// Converts the given vector represented in spherical coordinates to an euclidean float3 (x,y,z) triplet.
	/** @param azimuth The direction, or yaw, of the vector. This function uses the convention that the X-Z plane is
	the 2D horizontal "map" plane, with the vector (0,0,radius) corresponding to the vector in the direction azimuth=0 and inclination=0.
	This value is typically in the range [-pi, pi] (, or [0, 2pi]).
	@param inclination The elevation, or pitch, of the vector. This function uses the convention that the +Y axis
	points towards up, i.e. +Y is the "Zenith direction". This value is typically in the range [-pi/2, pi/2].
	@param radius The magnitude of the vector. This is usually >= 0, although passing in the zero vector as radius returns (0,0,0), and passing
	in a negative radius mirrors the coordinate along the origin.
	@see FromSphericalCoordinates, ToSphericalCoordinates, ToSphericalCoordinatesNormalized. */
	void SetFromSphericalCoordinates(float azimuth, float inclination, float radius);
	void SetFromSphericalCoordinates(const float3 &spherical) { SetFromSphericalCoordinates(spherical.x, spherical.y, spherical.z); }
	static MUST_USE_RESULT float3 FromSphericalCoordinates(float azimuth, float inclination, float radius);
	static MUST_USE_RESULT float3 FromSphericalCoordinates(const float3 &spherical) { return FromSphericalCoordinates(spherical.x, spherical.y, spherical.z); }

	/// Identical to SetFromSphericalCoordinates(azimuth, inclination, radius), except this function sets radius == 1 to generate a normalized
	/// vector on the unit sphere.
	/** @see FromSphericalCoordinates, ToSphericalCoordinates, ToSphericalCoordinatesNormalized. */
	void SetFromSphericalCoordinates(float azimuth, float inclination);
	static MUST_USE_RESULT float3 FromSphericalCoordinates(float azimuth, float inclination);

	/// Converts this euclidean (x,y,z) float3 to spherical coordinates representation in the form (azimuth, inclination, radius).
	/** @note This corresponsds to the matrix operation R_y * R_x * (0,0,radius), where R_y is a rotation about the y-axis by azimuth,
	and R_x is a rotation about the x-axis by inclination.
	@note It is valid for the magnitude of this vector to be (very close to) zero, in which case the return value is the zero vector.
	@see FromSphericalCoordinates, SetFromSphericalCoordinates, ToSphericalCoordinatesNormalized. */
	float3 ToSphericalCoordinates() const;

	/// Converts this normalized euclidean (x,y,z) float3 to spherical coordinates representation in the form (azimuth, inclination)
	/** @note This function requires that this float3 is normalized. This function is identical to ToSphericalCoordinates, but is slightly
	faster in the case this vector is known to be normalized in advance.
	@note This corresponsds to the matrix operation R_y * R_x * (0,0,radius), where R_y is a rotation about the y-axis by azimuth,
	and R_x is a rotation about the x-axis by inclination.
	@see ToSphericalCoordinates, FromSphericalCoordinates, SetFromSphericalCoordinates. */
	float2 ToSphericalCoordinatesNormalized() const;

	/// Computes the length of this vector.
	/** @return Sqrt(x*x + y*y + z*z).
	@see LengthSq(), Distance(), DistanceSq(). */
	float Length() const;

	/// Computes the squared length of this vector.
	/** Calling this function is faster than calling Length(), since this function avoids computing a square root.
	If you only need to compare lengths to each other, but are not interested in the actual length values,
	you can compare by using LengthSq(), instead of Length(), since Sqrt() is an order-preserving
	(monotonous and non-decreasing) function.
	@return x*x + y*y + z*z.
	@see LengthSq(), Distance(), DistanceSq(). */
	float LengthSq() const;

	/// Normalizes this float3.
	/** In the case of failure, this vector is set to (1, 0, 0), so calling this function will never result in an
	unnormalized vector.
	@note If this function fails to normalize the vector, no error message is printed, the vector is set to (1,0,0) and
	an error code 0 is returned. This is different than the behavior of the Normalized() function, which prints an
	error if normalization fails.
	@note This function operates in-place.
	@return The old length of this vector, or 0 if normalization failed.
	@see Normalized(). */
	float Normalize();

	/// Returns a normalized copy of this vector.
	/** @note If the vector is zero and cannot be normalized, the vector (1, 0, 0) is returned, and an error message is printed.
	If you do not want to generate an error message on failure, but want to handle the failure yourself, use the
	Normalize() function instead.
	@see Normalize(). */
	float3 Normalized() const;

	/// Scales this vector so that its new length is as given.
	/** Calling this function is effectively the same as normalizing the vector first and then multiplying by newLength.
	In the case of failure, this vector is set to (newLength, 0, 0), so calling this function will never result in an
	unnormalized vector.
	@note This function operates in-place.
	@return The old length of this vector. If this function returns 0, the scaling failed, and this vector is arbitrarily
	reset to (newLength, 0, 0). In case of failure, no error message is generated. You are expected to handle the failure
	yourself.
	@see ScaledToLength(). */
	float ScaleToLength(float newLength);

	/// Returns a scaled copy of this vector which has its new length as given.
	/** This function assumes the length of this vector is not zero. In the case of failure, an error message is printed,
	and the vector (newLength, 0, 0) is returned.
	@see ScaleToLength(). */
	float3 ScaledToLength(float newLength) const;

	/// Tests if the length of this vector is one, up to the given epsilon.
	/** @see IsZero(), IsFinite(), IsPerpendicular(). */
	bool IsNormalized(float epsilonSq = 1e-6f) const;

	/// Tests if this is the null vector, up to the given epsilon.
	/** @see IsNormalized(), IsFinite(), IsPerpendicular(). */
	bool IsZero(float epsilonSq = 1e-6f) const;

	/// Tests if this vector contains valid finite elements.
	/** @see IsNormalized(), IsZero(), IsPerpendicular(). */
	bool IsFinite() const;

	/// Tests if two vectors are perpendicular to each other.
	/** @see IsNormalized(), IsZero(), IsPerpendicular(), Equals(). */
	bool IsPerpendicular(const float3 &other, float epsilon = 1e-3f) const;

	/// Tests if the points p1, p2 and p3 lie on a straight line, up to the given epsilon.
	/** @see AreOrthogonal(), AreOrthonormal(), Line::AreCollinear(). */
	static MUST_USE_RESULT bool AreCollinear(const float3 &p1, const float3 &p2, const float3 &p3, float epsilon = 1e-4f);

	/// Tests if two vectors are equal, up to the given epsilon.
	/** @see IsPerpendicular(). */
	bool Equals(const float3 &other, float epsilon = 1e-3f) const;
	bool Equals(float x, float y, float z, float epsilon = 1e-3f) const;

	/// Parses a string that is of form "x,y,z" or "(x,y,z)" or "(x;y;z)" or "x y z" to a new float3.
	static MUST_USE_RESULT float3 FromString(const char *str);

	/// @return x + y + z.
	float SumOfElements() const;
	/// @return x * y * z.
	float ProductOfElements() const;
	/// @return (x+y+z)/3.
	float AverageOfElements() const;
	/// @return Min(x, y, z).
	/** @see MinElementIndex(). */
	float MinElement() const;
	/// Returns the index that has the smallest value in this vector.
	/** @see MinElement(). */
	int MinElementIndex() const;
	/// @return Max(x, y, z).
	/** @see MaxElementIndex(). */
	float MaxElement() const;
	/// Returns the index that has the smallest value in this vector.
	/** @see MaxElement(). */
	int MaxElementIndex() const;
	/// Takes the element-wise absolute value of this vector.
	/** @return float3(|x|, |y|, |z|).
	@see Neg(). */
	float3 Abs() const;
	/// Returns a copy of this vector with each element negated.
	/** This function returns a new vector where each element x of the original vector is replaced by the value -x.
	@return float3(-x, -y, -z).
	@see Abs(). */
	float3 Neg() const;
	/// Computes the element-wise reciprocal of this vector.
	/** This function returns a new vector where each element x of the original vector is replaced by the value 1/x.
	@return float3(1/x, 1/y, 1/z). */
	float3 Recip() const;
	/// Returns an element-wise minimum of this and the vector (ceil, ceil, ceil).
	/** Each element that is larger than ceil is replaced by ceil. */
	float3 Min(float ceil) const;
	/// Returns an element-wise minimum of this and the given vector.
	/** Each element that is larger than ceil is replaced by ceil.
	@see Max(), Clamp(). */
	float3 Min(const float3 &ceil) const;
	/// Returns an element-wise maximum of this and the vector (floor, floor, floor).
	/** Each element that is smaller than floor is replaced by floor. */
	float3 Max(float floor) const;
	/// Returns an element-wise maximum of this and the given vector.
	/** Each element that is smaller than floor is replaced by floor.
	@see Min(), Clamp(). */
	float3 Max(const float3 &floor) const;
	/// Returns a vector that has floor <= this[i] <= ceil for each element.
	float3 Clamp(float floor, float ceil) const;
	/// Limits each element of this vector between the corresponding elements in floor and ceil.
	/** @see Min(), Max(), Clamp01(), ClampLength(). */
	float3 Clamp(const float3 &floor, const float3 &ceil) const;
	/// Limits each element of this vector in the range [0, 1].
	/** @see Min(), Max(), Clamp(), ClampLength(). */
	float3 Clamp01() const;

	/// Returns a copy of this vector, with its length scaled down to maxLength.
	/** @see Clamp(). */
	float3 ClampLength(float maxLength) const;
	/// Returns a copy of this vector, with its length scaled between minLength and maxLength.
	/** @see Clamp(). */
	float3 ClampLength(float minLength, float maxLength) const;

	/// Computes the distance between this point and the given object.
	/** This function finds the nearest point to this point on the given object, and computes its distance
	to this point.
	If this point lies inside the given object, a distance of 0 is returned.
	@todo Add float3::Distance(Polygon/Circle/Disc/Frustum/Polyhedron).
	@see DistanceSq(), Length(), LengthSq(). */
	float Distance(const float3 &point) const;
	float Distance(const Line &line) const;
	float Distance(const Ray &ray) const;
	float Distance(const LineSegment &lineSegment) const;
	float Distance(const Plane &plane) const;
	float Distance(const Triangle &triangle) const;
	float Distance(const AABB &aabb) const;
	float Distance(const OBB &obb) const;
	float Distance(const Sphere &sphere) const;
	float Distance(const Capsule &capsule) const;

	/// Computes the squared distance between this and the given point.
	/** Calling this function is faster than calling Distance(), since this function avoids computing a square root.
	If you only need to compare distances to each other, but are not interested in the actual distance values,
	you can compare by using DistanceSq(), instead of Distance(), since Sqrt() is an order-preserving
	(monotonous and non-decreasing) function.
	@see Distance(), Length(), LengthSq(). */
	float DistanceSq(const float3 &point) const;

	/// Computes the dot product of this and the given vector.
	/** The dot product has a geometric interpretation of measuring how close two direction vectors are to pointing
	in the same direction, computing angles between vectors, or the length of a projection of one vector to another.
	@return x*v.x + y*v.y + z*v.z.
	@see AngleBetween(), ProjectTo(), ProjectToNorm(), Cross(), OuterProduct(), ScalarTripleProduct(). */
	float Dot(const float3 &v) const;

	/// Computes the cross product of this and the given vector.
	/** Unless this vector and the given vector are linearly dependent, the cross product returns a vector that is perpendicular
	to both vectors.
	@return float3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x).
	@see Dot(), OuterProduct(), Perpendicular(), AnotherPerpendicular(), ScalarTripleProduct(). */
	float3 Cross(const float3 &v) const;

	/// Computes a new normalized direction vector that is perpendicular to this vector and the specified hint vector.
	/** If this vector points toward the hint vector, the vector hint2 is returned instead.
	@see AnotherPerpendicular(), Cross(). */
	float3 Perpendicular(const float3 &hint = float3(0, 1, 0), const float3 &hint2 = float3(0, 0, 1)) const;

	/// Returns another vector that is perpendicular to this vector and the vector returned by Perpendicular().
	/** The set (this, Perpendicular(), AnotherPerpendicular()) forms a right-handed normalized 3D basis.
	@see Perpendicular(), Cross(). */
	float3 AnotherPerpendicular(const float3 &hint = float3(0, 1, 0), const float3 &hint2 = float3(0, 0, 1)) const;

	/// Generates a random vector that is perpendicular to this vector.
	/** The distribution is uniformly random. */
	float3 RandomPerpendicular(LCG &rng) const;

	/// Computes the scalar triple product of the given three vectors.
	/** @return [u v w] = (u x v) . w = u . (v x w)
	@see Dot(), Cross(). */
	static MUST_USE_RESULT float ScalarTripleProduct(const float3 &u, const float3 &v, const float3 &w);

	/// Returns this vector reflected about a plane with the given normal.
	/** By convention, both this and the reflected vector point away from the plane with the given normal
	@see Refract(). */
	float3 Reflect(const float3 &normal) const;

	/// Refracts this vector about a plane with the given normal.
	/** By convention, the this vector points towards the plane, and the returned vector points away from the plane.
	When the ray is going from a denser material to a lighter one, total internal reflection can occur.
	In this case, this function will just return a reflected vector from a call to Reflect().
	@param normal Specifies the plane normal direction
	@param negativeSideRefractionIndex The refraction index of the material we are exiting.
	@param positiveSideRefractionIndex The refraction index of the material we are entering.
	@see Reflect(). */
	float3 Refract(const float3 &normal, float negativeSideRefractionIndex, float positiveSideRefractionIndex) const;

	/// Projects this vector onto the given unnormalized direction vector.
	/** @param direction The direction vector to project this vector onto. This function will normalize this
	vector, so you can pass in an unnormalized vector.
	@see ProjectToNorm(). */
	float3 ProjectTo(const float3 &direction) const;

	/// Projects this vector onto the given normalized direction vector.
	/** @param direction The vector to project onto. This vector must be normalized.
	@see ProjectTo(). */
	float3 ProjectToNorm(const float3 &direction) const;

	/// Returns the angle between this vector and the specified vector, in radians.
	/** @note This function takes into account that this vector or the other vector can be unnormalized, and normalizes the computations.
	If you are computing the angle between two normalized vectors, it is better to use AngleBetweenNorm().
	@see AngleBetweenNorm(). */
	float AngleBetween(const float3 &other) const;

	/// Returns the angle between this vector and the specified normalized vector, in radians.
	/** @param normalizedVector The direction vector to compute the angle against. This vector must be normalized.
	@note This vector must be normalized to call this function.
	@see AngleBetween(). */
	float AngleBetweenNorm(const float3 &normalizedVector) const;

	/// Breaks this vector down into parallel and perpendicular components with respect to the given direction.
	/** @param direction The direction the decomposition is to be computed. This vector must be normalized.
	@param outParallel [out] Receives the part of this vector that is parallel to the given direction vector.
	@param outPerpendicular [out] Receives the part of this vector that is perpendicular to the given direction vector. */
	void Decompose(const float3 &direction, float3 &outParallel, float3 &outPerpendicular) const;

	/// Linearly interpolates between this and the vector b.
	/** @param b The target endpoint to lerp towards to.
	@param t The interpolation weight, in the range [0, 1].
	@return Lerp(b, 0) returns this vector, Lerp(b, 1) returns the vector b.
	Lerp(b, 0.5) returns the vector half-way in between the two vectors, and so on.
	Lerp(b, t) returns (1-t)*this + t*b. */
	float3 Lerp(const float3 &b, float t) const;
	/// This function is the same as calling a.Lerp(b, t).
	static MUST_USE_RESULT float3 Lerp(const float3 &a, const float3 &b, float t);

	/// Makes the given vectors linearly independent.
	/** This function directly follows the Gram-Schmidt procedure on the input vectors.
	The vector a is kept unmodified, and vector b is modified to be perpendicular to a.
	Finally, if specified, the vector c is adjusted to be perpendicular to a and b.
	@note If any of the input vectors is zero, then the resulting set of vectors cannot be made orthogonal.
	@see AreOrthogonal(), Orthonormalize(), AreOrthonormal(). */
	static void Orthogonalize(const float3 &a, float3 &b);
	static void Orthogonalize(const float3 &a, float3 &b, float3 &c);

	/// Returns true if the given vectors are orthogonal to each other.
	/** @see Orthogonalize(), Orthonormalize(), AreOrthonormal(), AreCollinear(). */
	static MUST_USE_RESULT bool AreOrthogonal(const float3 &a, const float3 &b, float epsilon = 1e-3f);
	static MUST_USE_RESULT bool AreOrthogonal(const float3 &a, const float3 &b, const float3 &c, float epsilon = 1e-3f);

	/// Makes the given vectors linearly independent and normalized in length.
	/** This function directly follows the Gram-Schmidt procedure on the input vectors.
	The vector a is first normalized, and vector b is modified to be perpendicular to a, and also normalized.
	Finally, if specified, the vector c is adjusted to be perpendicular to a and b, and normalized.
	@note If any of the input vectors is zero, then the resulting set of vectors cannot be made orthonormal.
	@see Orthogonalize(), AreOrthogonal(), AreOrthonormal(). */
	static void Orthonormalize(float3 &a, float3 &b);
	static void Orthonormalize(float3 &a, float3 &b, float3 &c);

	/// Returns true if the given vectors are orthogonal to each other and all of length 1.
	/** @see Orthogonalize(), AreOrthogonal(), Orthonormalize(), AreCollinear(). */
	static MUST_USE_RESULT bool AreOrthonormal(const float3 &a, const float3 &b, float epsilon = 1e-3f);
	static MUST_USE_RESULT bool AreOrthonormal(const float3 &a, const float3 &b, const float3 &c, float epsilon = 1e-3f);

	/// Generates a direction vector of the given length.
	/** The returned vector points at a uniformly random direction.
	@see RandomSphere(), RandomBox(). */
	static MUST_USE_RESULT float3 RandomDir(LCG &lcg, float length = 1.f);
	/// Generates a random point inside a sphere.
	/** The returned point is generated uniformly inside the sphere.
	@see RandomDir(), RandomBox(). */
	static MUST_USE_RESULT float3 RandomSphere(LCG &lcg, const float3 &center, float radius);
	/// Generates a random point inside an axis-aligned box.
	/** The returned point is generated uniformly inside the box.
	@see RandomDir(), RandomSphere(). */
	static MUST_USE_RESULT float3 RandomBox(LCG &lcg, float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
	static MUST_USE_RESULT float3 RandomBox(LCG &lcg, const float3 &minValues, const float3 &maxValues);

	/// Returns a random float3 with each entry randomized between the range [minElem, maxElem].
	static MUST_USE_RESULT float3 RandomBox(LCG &lcg, float minElem, float maxElem);

	/// Specifies a compile-time constant float3 with value (0, 0, 0).
	/** @note Due to static data initialization order being undefined in C++, do NOT use this
	member to initialize other static data in other compilation units! */
	static const float3 zero;
	/// Specifies a compile-time constant float3 with value (1, 1, 1). [similarOverload: zero]
	/** @note Due to static data initialization order being undefined in C++, do NOT use this
	member to initialize other static data in other compilation units! */
	static const float3 one;
	/// Specifies a compile-time constant float3 with value (1, 0, 0).
	/** @note Due to static data initialization order being undefined in C++, do NOT use this
	member to initialize other static data in other compilation units! */
	static const float3 unitX;
	/// Specifies a compile-time constant float3 with value (0, 1, 0). [similarOverload: unitX]
	/** @note Due to static data initialization order being undefined in C++, do NOT use this
	member to initialize other static data in other compilation units! */
	static const float3 unitY;
	/// Specifies a compile-time constant float3 with value (0, 0, 1). [similarOverload: unitX]
	/** @note Due to static data initialization order being undefined in C++, do NOT use this
	member to initialize other static data in other compilation units! */
	static const float3 unitZ;
	/// A compile-time constant float3 with value (NaN, NaN, NaN).
	/** For this constant, each element has the value of quiet NaN, or Not-A-Number.
	@note Never compare a float3 to this value! Due to how IEEE floats work, for each float x, both expressions "x == nan" and "x != nan" return false!
	That is, nothing is equal to NaN, not even NaN itself!
	@note Due to static data initialization order being undefined in C++, do NOT use this
	member to initialize other static data in other compilation units! */
	static const float3 nan;
	/// A compile-time constant float3 with value (+infinity, +infinity, +infinity). [similarOverload: nan]
	/** @note Due to static data initialization order being undefined in C++, do NOT use this
	member to initialize other static data in other compilation units! */
	static const float3 inf;

	/*
	#ifdef MATH_IRRLICHT_INTEROP
	float3(const Vector3df &other) { x = other.x; y = other.y; z = other.z; }
	operator Vector3df() const { return Vector3df(x, y, z); }
	#endif
	*/
};

class float3x3
{
public:
	/// Specifies the height of this matrix.
	enum { Rows = 3 };

	/// Specifies the width of this matrix.
	enum { Cols = 3 };

	/// Stores the data in this matrix in row-major format. [noscript]
	float v[Rows][Cols];

	/// A constant matrix that has zeroes in all its entries.
	static const float3x3 zero;

	/// A constant matrix that is the identity.
	/** The identity matrix looks like the following:
	1 0 0
	0 1 0
	0 0 1
	Transforming a vector by the identity matrix is like multiplying a number by one, i.e. the vector is not changed. */
	static const float3x3 identity;

	/// A compile-time constant float3x3 which has NaN in each element.
	/// For this constant, each element has the value of quiet NaN, or Not-A-Number.
	/// @note Never compare a float3x3 to this value! Due to how IEEE floats work, for each float x, both the expression "x == nan" and "x != nan" returns false!
	///	   That is, nothing is equal to NaN, not even NaN itself!
	static const float3x3 nan;

	/// Creates a new float3x3 with uninitialized member values.
	/** [opaque-qtscript] */
	float3x3() {}

	/// Constructs a new float3x3 by explicitly specifying all the matrix elements.
	/// The elements are specified in row-major format, i.e. the first row first followed by the second and third row.
	/// E.g. The element _10 denotes the scalar at second (index 1) row, first (index 0) column.
	float3x3(float _00, float _01, float _02,
		float _10, float _11, float _12,
		float _20, float _21, float _22);

	/// Constructs the matrix by explicitly specifying the four column vectors.
	/** @param col0 The first column. If this matrix represents a change-of-basis transformation, this parameter is the world-space
	direction of the local X axis.
	@param col1 The second column. If this matrix represents a change-of-basis transformation, this parameter is the world-space
	direction of the local Y axis.
	@param col2 The third column. If this matrix represents a change-of-basis transformation, this parameter is the world-space
	direction of the local Z axis. */
	float3x3(const float3 &col0, const float3 &col1, const float3 &col2);

	/// Constructs this float3x3 from the given quaternion.
	explicit float3x3(const Quat &orientation);

	/// Creates a new float3x3 that rotates about one of the principal axes by the given angle. [indexTitle: RotateX/Y/Z]
	/** Calling RotateX, RotateY or RotateZ is slightly faster than calling the more generic RotateAxisAngle function. */
	static float3x3 RotateX(float angleRadians);
	/** [similarOverload: RotateX] [hideIndex] */
	static float3x3 RotateY(float angleRadians);
	/** [similarOverload: RotateX] [hideIndex] */
	static float3x3 RotateZ(float angleRadians);

	/// Creates a new float3x3 that rotates about the given axis by the given angle.
	static float3x3 RotateAxisAngle(const float3 &axisDirection, float angleRadians);

	/// Creates a matrix that rotates the sourceDirection vector to coincide with the targetDirection vector.
	/** Both input direction vectors must be normalized.
	@note There are infinite such rotations - this function returns the rotation that has the shortest angle
	(when decomposed to axis-angle notation).
	@return An orthonormal matrix M with a determinant of +1. For the matrix M it holds that
	M * sourceDirection = targetDirection. */
	static float3x3 RotateFromTo(const float3 &sourceDirection, const float3 &targetDirection);

	/// Creates a LookAt matrix.
	/** A LookAt matrix is a rotation matrix that orients an object to face towards a specified target direction.
	@param localForward Specifies the forward direction in the local space of the object. This is the direction
	the model is facing at in its own local/object space, often +X (1,0,0), +Y (0,1,0) or +Z (0,0,1). The
	vector to pass in here depends on the conventions you or your modeling software is using, and it is best
	pick one convention for all your objects, and be consistent.
	This input parameter must be a normalized vector.
	@param targetDirection Specifies the desired world space direction the object should look at. This function
	will compute a rotation matrix which will rotate the localForward vector to orient towards this targetDirection
	vector. This input parameter must be a normalized vector.
	@param localUp Specifies the up direction in the local space of the object. This is the up direction the model
	was authored in, often +Y (0,1,0) or +Z (0,0,1). The vector to pass in here depends on the conventions you
	or your modeling software is using, and it is best to pick one convention for all your objects, and be
	consistent. This input parameter must be a normalized vector. This vector must be perpendicular to the
	vector localForward, i.e. localForward.Dot(localUp) == 0.
	@param worldUp Specifies the global up direction of the scene in world space. Simply rotating one vector to
	coincide with another (localForward->targetDirection) would cause the up direction of the resulting
	orientation to drift (e.g. the model could be looking at its target its head slanted sideways). To keep
	the up direction straight, this function orients the localUp direction of the model to point towards the
	specified worldUp direction (as closely as possible). The worldUp and targetDirection vectors cannot be
	collinear, but they do not need to be perpendicular either.
	@return A matrix that maps the given local space forward direction vector to point towards the given target
	direction, and the given local up direction towards the given target world up direction. This matrix can be
	used as the 'world transform' of an object. The returned matrix M is orthonormal with a determinant of +1.
	For the matrix M it holds that M * localForward = targetDirection, and M * localUp lies in the plane spanned by
	the vectors targetDirection and worldUp.
	@see RotateFromTo().
	@note Be aware that the convention of a 'LookAt' matrix in MathGeoLib differs from e.g. GLM. In MathGeoLib, the returned
	matrix is a mapping from local space to world space, meaning that the returned matrix can be used as the 'world transform'
	for any 3D object (camera or not). The view space is the local space of the camera, so this function returns the mapping
	view->world. In GLM, the LookAt function is tied to cameras only, and it returns the inverse mapping world->view. */
	static float3x3 LookAt(const float3 &localForward, const float3 &targetDirection, const float3 &localUp, const float3 &worldUp);

	/// Returns a uniformly random 3x3 matrix that performs only rotation.
	/** This matrix produces a random orthonormal basis for an orientation of an object. There is no mirroring
	or scaling present in the generated matrix. Also, naturally since float3x3 cannot represent translation or projection,
	those properties are not present either. */
	static float3x3 RandomRotation(LCG &lcg);

	/// Returns a random 3x3 matrix with each entry randomized between the range[minElem, maxElem].
	/** Warning: The matrices returned by this function do not represent well-formed 3D transformations.
	This function is mostly used for testing and debugging purposes only. */
	static float3x3 RandomGeneral(LCG &lcg, float minElem, float maxElem);

	/// Creates a new float3x3 that performs the rotation expressed by the given quaternion.
	static float3x3 FromQuat(const Quat &orientation);

	/// Converts this rotation matrix to a quaternion.
	/// This function assumes that the matrix is orthonormal (no shear or scaling) and does not perform any mirroring (determinant > 0).
	Quat ToQuat() const;

	/// Creates a new float3x3 as a combination of rotation and scale.
	/** This function creates a new float3x3 M of the form M = R * S, where R is a
	rotation matrix and S a scale matrix. Transforming a vector v using this matrix computes the vector
	v' == M * v == R*S*v == (R * (S * v)), which means that the scale operation is applied to the
	vector first, followed by rotation and finally translation. */
	static float3x3 FromRS(const Quat &rotate, const float3 &scale);
	static float3x3 FromRS(const float3x3 &rotate, const float3 &scale);

	/// Creates a new float3x3 from the given sequence of Euler rotation angles (in radians).
	/** The FromEulerABC function returns a matrix M = A(ea) * B(eb) * C(ec). Rotation
	C is applied first, followed by B and then A. [indexTitle: FromEuler***] */
	static float3x3 FromEulerXYX(float ex, float ey, float ex2);
	static float3x3 FromEulerXZX(float ex, float ez, float ex2); ///< [similarOverload: FromEulerXYX] [hideIndex]
	static float3x3 FromEulerYXY(float ey, float ex, float ey2); ///< [similarOverload: FromEulerXYX] [hideIndex]
	static float3x3 FromEulerYZY(float ey, float ez, float ey2); ///< [similarOverload: FromEulerXYX] [hideIndex]
	static float3x3 FromEulerZXZ(float ez, float ex, float ez2); ///< [similarOverload: FromEulerXYX] [hideIndex]
	static float3x3 FromEulerZYZ(float ez, float ey, float ez2); ///< [similarOverload: FromEulerXYX] [hideIndex]
	static float3x3 FromEulerXYZ(float ex, float ey, float ez); ///< [similarOverload: FromEulerXYX] [hideIndex]
	static float3x3 FromEulerXZY(float ex, float ez, float ey); ///< [similarOverload: FromEulerXYX] [hideIndex]
	static float3x3 FromEulerYXZ(float ey, float ex, float ez); ///< [similarOverload: FromEulerXYX] [hideIndex]
	static float3x3 FromEulerYZX(float ey, float ez, float ex); ///< [similarOverload: FromEulerXYX] [hideIndex]
	static float3x3 FromEulerZXY(float ez, float ex, float ey); ///< [similarOverload: FromEulerXYX] [hideIndex]
	static float3x3 FromEulerZYX(float ez, float ey, float ex); ///< [similarOverload: FromEulerXYX] [hideIndex]

																/// Creates a new transformation matrix that scales by the given factors.
																/// This matrix scales with respect to origin.
	/// Creates a new float3x3 that scales points along the given axis.
	/** @param axis A normalized direction vector that specifies the direction of scaling.
	@param scalingFactor The amount of scaling to apply along the specified axis. */
	static float3x3 ScaleAlongAxis(const float3 &axis, float scalingFactor);

	/// Returns the scaling performed by this matrix.
	/// GetScale().x specifies the amount of scaling applied to the local x direction vector when it is transformed by this matrix.
	/// i.e. GetScale()[i] equals Col(i).Length();
	float3 GetScale() const;

	/// Produces a matrix that shears along a principal axis.
	/** The shear matrix offsets the two other axes according to the
	position of the point along the shear axis. [indexTitle: ShearX/Y/Z] */
	static float3x3 ShearX(float yFactor, float zFactor);
	static float3x3 ShearY(float xFactor, float zFactor); ///< [similarOverload: ShearX] [hideIndex]
	static float3x3 ShearZ(float xFactor, float yFactor); ///< [similarOverload: ShearX] [hideIndex]

														  /// Creates a new matrix that mirrors with respect to the given plane.
														  /** Points lying on one side of the plane will end up at the opposite side of the plane, at the same distance of the plane
														  they were. */
	static float3x3 Mirror(const Plane &p);

	/// Creates a new float3x3 that performs orthographic projection. [indexTitle: OrthographicProjection/YZ/XZ/XY]
	static float3x3 OrthographicProjection(const Plane &target);
	static float3x3 OrthographicProjectionYZ(); ///< [similarOverload: OrthographicProjection] [hideIndex]
	static float3x3 OrthographicProjectionXZ(); ///< [similarOverload: OrthographicProjection] [hideIndex]
	static float3x3 OrthographicProjectionXY(); ///< [similarOverload: OrthographicProjection] [hideIndex]

												/// Computes the covariance matrix of the given set of data points.
												//	static float3x3 CovarianceMatrix(const float3 *pointArray, int numPoints);

												/// Returns the given element. [noscript]
												/** Returns a reference to the element at m[row][col] (or "m[y][x]").
												Remember that zero-based indexing is used, so m[0][0] is the upper-left element of this matrix.
												@note You can use the index notation to set elements of the matrix, e.g. m[0][1] = 5.f;
												@note MatrixProxy is a temporary helper class. Do not store references to it, but always
												directly dereference it with the [] operator.
												For example, m[0][2] Returns the last element on the first row. */

	/// Returns the given element. [noscript]
	/** This function returns the element of this matrix at (row, col)==(i, j)==(y, x).
	If you have a non-const object, you can set values of this matrix through this
	reference, using the notation m.At(row, col) = someValue; */
	float &At(int row, int col);
	const float At(int row, int col) const;

	/// Returns the given row. [noscript]
	/** @param row The zero-based index [0, 2] of the row to get. */
	float3 &Row(int row);
	const float3 &Row(int row) const;

	float3 &Row3(int row) { return Row(row); }
	const float3 &Row3(int row) const { return Row(row); }

	/// Returns the given column.
	/** @param col The zero-based index [0, 2] of the column to get. */
	const float3 Col(int col) const;
	const float3 Col3(int col) const { return Col(col); }

	/// Returns the main diagonal.
	/** The main diagonal consists of the elements at m[0][0], m[1][1], m[2][2]. */
	const float3 Diagonal() const;

	/// Scales the given row by a scalar.
	void ScaleRow(int row, float scalar);

	/// Scales the given column by a scalar.
	void ScaleCol(int col, float scalar);

	// Returns the local right axis in the post-transformed coordinate space, according to the given convention.
	// @note The returned vector might not be normalized if this matrix contains scaling.
	// @note The basis returned by (Right, Up, Forward) might not be of the same handedness as the
	//	   pre-transformed coordinate system, if the matrix contained reflection.
	//	template<typename Convention = XposRight_YposUp_ZposForward> float3 Right() const;

	// Returns the local up axis in the post-transformed coordinate space, according to the given convention.
	// @note The returned vector might not be normalized if this matrix contains scaling.
	// @note The basis returned by (Right, Up, Forward) might not be of the same handedness as the
	//	   pre-transformed coordinate system, if the matrix contained reflection.
	//	template<typename Convention = XposRight_YposUp_ZposForward> float3 Up() const;

	// Returns the local forward axis in the post-transformed coordinate space, according to the given convention.
	// @note The returned vector might not be normalized if this matrix contains scaling.
	// @note The basis returned by (Right, Up, Forward) might not be of the same handedness as the
	//	   pre-transformed coordinate system, if the matrix contained reflection.
	//	template<typename Convention = XposRight_YposUp_ZposForward> float3 Forward() const;

	/// Returns the local +X/+Y/+Z axis in world space.
	/** This is the same as transforming the vector (1,0,0) by this matrix. [indexTitle: PositiveX/Y/Z] */
	float3 WorldX() const;
	/// Returns the local +Y axis in world space.
	/** This is the same as transforming the vector (0,1,0) by this matrix. [similarOverload: PositiveX] [hideIndex] */
	float3 WorldY() const;
	/// Returns the local +Z axis in world space.
	/** This is the same as transforming the vector (0,0,1) by this matrix. [similarOverload: PositiveX] [hideIndex] */
	float3 WorldZ() const;

	/// Accesses this structure as a float array.
	/// @return A pointer to the upper-left element. The data is contiguous in memory.
	/// ptr[0] gives the element [0][0], ptr[1] is [0][1], ptr[2] is [0][2].
	/// ptr[4] == [1][0], ptr[5] == [1][1], ..., and finally, ptr[15] == [3][3].
	float *ptr();
	/// @return A pointer to the upper-left element . The data is contiguous in memory.
	const float *ptr() const;

	/// Sets the values of the given row.
	/** @param row The index of the row to set, in the range [0-2].
	@param data A pointer to an array of 3 floats that contain the new x, y and z values for the row. */
	void SetRow(int row, const float *data);
	void SetRow(int row, float x, float y, float z);
	void SetRow(int row, const float3 &rowVector);

	/// Sets the values of the given column.
	/** @param column The index of the column to set, in the range [0-2].
	@param data A pointer to an array of 3 floats that contain the new x, y and z values for the column. */
	void SetCol(int column, const float *data);
	void SetCol(int column, float x, float y, float z);
	void SetCol(int column, const float3 &columnVector);

	/// Sets all values of this matrix.
	void Set(float _00, float _01, float _02,
		float _10, float _11, float _12,
		float _20, float _21, float _22);

	/// Sets this to be a copy of the matrix rhs.
	void Set(const float3x3 &rhs);

	/// Sets all values of this matrix.
	/// @param values The values in this array will be copied over to this matrix. The source must contain 9 floats in row-major order (the same
	///		order as the Set() function above has its input parameters in).
	void Set(const float *values);

	/// Sets a single element of this matrix.
	/** @param row The row index (y-coordinate) of the element to set, in the range [0-2].
	@param col The col index (x-coordinate) of the element to set, in the range [0-2].
	@param value The new value to set to the cell [row][col]. */
	void Set(int row, int col, float value);

	/// Sets this matrix to equal the identity.
	void SetIdentity();

	/// Swaps two columns.
	void SwapColumns(int col1, int col2);

	/// Swaps two rows.
	void SwapRows(int row1, int row2);

	/// Sets this matrix to perform rotation about the positive X axis which passes through
	/// the origin. [similarOverload: SetRotatePart] [hideIndex]
	void SetRotatePartX(float angleRadians);
	/// Sets this matrix to perform rotation about the positive Y axis. [similarOverload: SetRotatePart] [hideIndex]
	void SetRotatePartY(float angleRadians);
	/// Sets this matrix to perform rotation about the positive Z axis. [similarOverload: SetRotatePart] [hideIndex]
	void SetRotatePartZ(float angleRadians);

	/// Sets this matrix to perform rotation about the given axis and angle. [indexTitle: SetRotatePart/X/Y/Z]
	void SetRotatePart(const float3 &axisDirection, float angleRadians);
	/// Sets this matrix to perform the rotation expressed by the given quaternion.
	void SetRotatePart(const Quat &orientation);

	float3x3 &operator =(const Quat &rhs);
	float3x3 &operator =(const float3x3 &rhs);

	/// Computes the determinant of this matrix.
	/** If the determinant is nonzero, this matrix is invertible.
	If the determinant is negative, this matrix performs reflection about some axis.
	From http://msdn.microsoft.com/en-us/library/bb204853(VS.85).aspx :
	"If the determinant is positive, the basis is said to be "positively" oriented (or right-handed).
	If the determinant is negative, the basis is said to be "negatively" oriented (or left-handed)."
	@note This function computes 9 LOADs, 9 MULs and 5 ADDs. */
	float Determinant() const;

	/// Computes the determinant of a symmetric matrix.
	/** This function can be used to compute the determinant of a matrix in the case the matrix is known beforehand
	to be symmetric. This function is slightly faster than Determinant().
	@note This function computes 6 LOADs, 9 MULs and 4 ADDs. */
	float DeterminantSymmetric() const;

	//	float2x2 SubMatrix(int i, int j) const;

	/// Returns the adjugate of this matrix.
	//	float3x3 Adjugate() const;

	/// Inverts this matrix using Cramer's rule.
	/// @return Returns true on success, false otherwise.
	bool Inverse(float epsilon = 1e-3f);

	bool InverseFast(float epsilon = 1e-3f);

	/// Solves the linear equation Ax=b.
	/** The matrix A in the equations is this matrix. */
	bool SolveAxb(float3 b, float3 &x) const;

	/// Returns an inverted copy of this matrix. This function uses the Cramer's rule.
	/// If this matrix does not have an inverse, returns the matrix that was the result of running
	/// Gauss's method on the matrix.
	float3x3 Inverted() const;

	/// Inverts a column-orthogonal matrix.
	/// If a matrix is of form M=R*S, where
	/// R is a rotation matrix and S is a diagonal matrix with non-zero but potentially non-uniform scaling
	/// factors (possibly mirroring), then the matrix M is column-orthogonal and this function can be used to compute the inverse.
	/// Calling this function is faster than the calling the generic matrix Inverse() function.
	/// Returns true on success. On failure, the matrix is not modified. This function fails if any of the
	/// elements of this vector are not finite, or if the matrix contains a zero scaling factor on X, Y or Z.
	/// @note The returned matrix will be row-orthogonal, but not column-orthogonal in general.
	/// The returned matrix will be column-orthogonal iff the original matrix M was row-orthogonal as well.
	/// (in which case S had uniform scale, InverseOrthogonalUniformScale() could have been used instead)
	bool InverseColOrthogonal();

	/// Inverts a matrix that is a concatenation of only rotate and uniform scale operations.
	/// If a matrix is of form M=R*S, where
	/// R is a rotation matrix and S is a diagonal matrix with non-zero and uniform scaling factors (possibly mirroring),
	/// then the matrix M is both column- and row-orthogonal and this function can be used to compute the inverse.
	/// This function is faster than calling InverseColOrthogonal() or the generic Inverse().
	/// Returns true on success. On failure, the matrix is not modified. This function fails if any of the
	/// elements of this vector are not finite, or if the matrix contains a zero scaling factor on X, Y or Z.
	/// This function may not be called if this matrix contains any shearing or nonuniform scaling.
	bool InverseOrthogonalUniformScale();

	/// Inverts a rotation matrix.
	/// If a matrix is of form M=R*S, where R is a rotation matrix and S is either identity or a mirroring matrix, then
	/// the matrix M is orthonormal and this function can be used to compute the inverse.
	/// This function is faster than calling InverseOrthogonalUniformScale(), InverseColOrthogonal() or the
	/// generic Inverse().
	/// This function may not be called if this matrix contains any scaling or shearing, but it may contain mirroring.
	void InverseOrthonormal();

	/// Inverts a symmetric matrix.
	/** This function is faster than directly calling Inverse().
	This function computes 6 LOADs, 9 STOREs, 21 MULs, 1 DIV, 1 CMP and 8 ADDs.
	@return True if computing the inverse succeeded, false otherwise (determinant was zero). If this function fails,
	the original matrix is not modified.
	@note This function operates in-place. */
	bool InverseSymmetric();

	/// Transposes this matrix.
	/// This operation swaps all elements with respect to the diagonal.
	void Transpose();

	/// Returns a transposed copy of this matrix.
	float3x3 Transposed() const;

	/// Computes the inverse transpose of this matrix in-place.
	/** Use the inverse transpose to transform covariant vectors (normal vectors). */
	bool InverseTranspose();

	/// Returns the inverse transpose of this matrix.
	/** Use that matrix to transform covariant vectors (normal vectors). */
	float3x3 InverseTransposed() const;

	/// Returns the sum of the diagonal elements of this matrix.
	float Trace() const;

	/// Orthonormalizes the basis formed by the column vectors of this matrix.
	void Orthonormalize(int firstColumn, int secondColumn, int thirdColumn);

	/// Removes the scaling performed by this matrix. That is, decomposes this matrix M into a form M = M' * S, where
	/// M' has unitary column vectors and S is a diagonal matrix. Then replaces this matrix with M'.
	/// @note This function assumes that this matrix does not contain projection (the fourth row of this matrix is [0 0 0 1]).
	/// @note This function does not remove reflection (-1 scale along some axis).
	void RemoveScale();

	/// Transforms the given 3-vector by this matrix M, i.e. returns M * (x, y, z).
	float3 Transform(const float3 &vector) const;
	float3 Transform(float x, float y, float z) const;

	/// Transforms the given 3-vector by this matrix M so that the vector occurs on the left-hand side, i.e.
	/// computes the product lhs * M. This is against the common convention used by this class when transforming
	/// geometrical objects, but this operation is still occasionally useful for other purposes.
	/// (Remember that M * v != v * M in general).
	float3 TransformLeft(const float3 &lhs) const;

	/// Performs a batch transform of the given array.
	void BatchTransform(float3 *pointArray, int numPoints) const;

	/// Performs a batch transform of the given array.
	void BatchTransform(float3 *pointArray, int numPoints, int stride) const;

	/// Multiplies the two matrices.
	float3x3 operator *(const float3x3 &rhs) const;

	/// Converts the quaternion to a float3x3 and multiplies the two matrices together.
	float3x3 operator *(const Quat &rhs) const;

	/// Transforms the given vector by this matrix (in the order M * v).
	float3 operator *(const float3 &rhs) const;

	float3x3 operator *(float scalar) const;
	float3x3 operator /(float scalar) const;
	float3x3 operator +(const float3x3 &rhs) const;
	float3x3 operator -(const float3x3 &rhs) const;
	float3x3 operator -() const;

	/// Unary operator + allows this structure to be used in an expression '+x'.
	float3x3 operator +() const { return *this; }

	float3x3 &operator *=(float scalar);
	float3x3 &operator /=(float scalar);
	float3x3 &operator +=(const float3x3 &rhs);
	float3x3 &operator -=(const float3x3 &rhs);

	/// Tests if this matrix does not contain any NaNs or infs.
	/** @return Returns true if the entries of this float3x3 are all finite, and do not contain NaN or infs. */
	bool IsFinite() const;

	/// Tests if this is the identity matrix.
	/** @return Returns true if this matrix is the identity matrix, up to the given epsilon. */
	bool IsIdentity(float epsilon = 1e-3f) const;

	/// Tests if this matrix is in lower triangular form.
	/** @return Returns true if this matrix is in lower triangular form, up to the given epsilon. */
	bool IsLowerTriangular(float epsilon = 1e-3f) const;

	/// Tests if this matrix is in upper triangular form.
	/** @return Returns true if this matrix is in upper triangular form, up to the given epsilon. */
	bool IsUpperTriangular(float epsilon = 1e-3f) const;

	/// Tests if this matrix has an inverse.
	/** @return Returns true if this matrix can be inverted, up to the given epsilon. */
	bool IsInvertible(float epsilon = 1e-3f) const;

	/// Tests if this matrix is symmetric (M == M^T).
	/** The test compares the elements for equality, up to the given epsilon. A matrix is symmetric if it is its own transpose. */
	bool IsSymmetric(float epsilon = 1e-3f) const;

	/// Tests if this matrix is skew-symmetric (M == -M^T).
	/** The test compares the floating point elements of this matrix up to the given epsilon. A matrix M is skew-symmetric
	the identity M=-M^T holds. */
	bool IsSkewSymmetric(float epsilon = 1e-3f) const;

	/// Returns true if this matrix does not perform any scaling.
	/** A matrix does not do any scaling if the column vectors of this
	matrix are normalized in length, compared to the given epsilon. Note that this matrix may still perform
	reflection, i.e. it has a -1 scale along some axis.
	@note This function only examines the upper 3-by-3 part of this matrix.
	@note This function assumes that this matrix does not contain projection (the fourth row of this matrix is [0 0 0 1]). */
	bool HasUnitaryScale(float epsilonSq = 1e-6f) const;

	/// Returns true if this matrix performs a reflection along some plane.
	/** In 3D space, an even number of reflections corresponds to a rotation about some axis, so a matrix consisting of
	an odd number of consecutive mirror operations can only reflect about one axis. A matrix that contains reflection reverses
	the handedness of the coordinate system. This function tests if this matrix
	does perform mirroring. This occurs iff this matrix has a negative determinant. */
	bool HasNegativeScale() const;

	/// Returns true if this matrix contains only uniform scaling, compared to the given epsilon.
	/// @note If the matrix does not really do any scaling, this function returns true (scaling uniformly by a factor of 1).
	bool HasUniformScale(float epsilon = 1e-3f) const;

	/// Returns true if the row vectors of this matrix are all perpendicular to each other.
	bool IsRowOrthogonal(float epsilon = 1e-3f) const;

	/// Returns true if the column vectors of this matrix are all perpendicular to each other.
	bool IsColOrthogonal(float epsilon = 1e-3f) const;
	bool IsColOrthogonal3(float epsilon = 1e-3f) const { return IsColOrthogonal(epsilon); }

	/// Returns true if the column and row vectors of this matrix form an orthonormal set.
	/// @note In math terms, there does not exist such a thing as 'orthonormal matrix'. In math terms, a matrix
	/// is orthogonal iff its column and row vectors are orthogonal *unit* vectors.
	/// In the terms of this library however, a matrix is orthogonal iff its column and row vectors are orthogonal (no need to be unitary),
	/// and a matrix is orthonormal if the column and row vectors are orthonormal.
	bool IsOrthonormal(float epsilon = 1e-3f) const;

	/// Returns true if this float3x3 is equal to the given float3x3, up to given per-element epsilon.
	bool Equals(const float3x3 &other, float epsilon = 1e-3f) const;

	/// Extracts the rotation part of this matrix into Euler rotation angles (in radians). [indexTitle: ToEuler***]
	/// @note It is better to think about the returned float3 as an array of three floats, and
	/// not as a triple of xyz, because e.g. the .y component returned by ToEulerYXZ() does
	/// not return the amount of rotation about the y axis, but contains the amount of rotation
	/// in the second axis, in this case the x axis.
	float3 ToEulerXYX() const;
	float3 ToEulerXZX() const; ///< [similarOverload: ToEulerXYX] [hideIndex]
	float3 ToEulerYXY() const; ///< [similarOverload: ToEulerXYX] [hideIndex]
	float3 ToEulerYZY() const; ///< [similarOverload: ToEulerXYX] [hideIndex]
	float3 ToEulerZXZ() const; ///< [similarOverload: ToEulerXYX] [hideIndex]
	float3 ToEulerZYZ() const; ///< [similarOverload: ToEulerXYX] [hideIndex]
	float3 ToEulerXYZ() const; ///< [similarOverload: ToEulerXYX] [hideIndex]
	float3 ToEulerXZY() const; ///< [similarOverload: ToEulerXYX] [hideIndex]
	float3 ToEulerYXZ() const; ///< [similarOverload: ToEulerXYX] [hideIndex]
	float3 ToEulerYZX() const; ///< [similarOverload: ToEulerXYX] [hideIndex]
	float3 ToEulerZXY() const; ///< [similarOverload: ToEulerXYX] [hideIndex]
	float3 ToEulerZYX() const; ///< [similarOverload: ToEulerXYX] [hideIndex]

							   /// Returns the scale components of this matrix.
							   /** This function decomposes this matrix M into a form M = M' * S, where M' has unitary column vectors and S is a diagonal matrix.
							   @return ExtractScale returns the diagonal entries of S, i.e. the scale of the columns of this matrix . If this matrix
							   represents a local->world space transformation for an object, then this scale represents a 'local scale', i.e.
							   scaling that is performed before translating and rotating the object from its local coordinate system to its world
							   position.
							   @note This function assumes that this matrix does not contain projection (the fourth row of this matrix is [0 0 0 1]).
							   @note This function does not detect and return reflection (-1 scale along some axis). */
	float3 ExtractScale() const;

	/// Decomposes this matrix to rotate and scale parts.
	/** This function decomposes this matrix M to a form M = R * S, where R a rotation matrix and S a
	scale matrix.
	@note Remember that in the convention of this class, transforms are applied in the order M * v, so scale is
	applied first, then rotation, and finally the translation last.
	@note This function assumes that this matrix does not contain projection (the fourth row of this matrix is [0 0 0 1]).
	@param rotate [out] This object receives the rotation part of this transform.
	@param scale [out] This vector receives the scaling along the local (before transformation by R) X, Y and Z axes
	performed by this matrix. */
	void Decompose(Quat &rotate, float3 &scale) const;
	void Decompose(float3x3 &rotate, float3 &scale) const;

	float3x3 Mul(const float3x3 &rhs) const;
	float3x3 Mul(const Quat &rhs) const;
	float3 Mul(const float3 &rhs) const;
	float3 MulPos(const float3 &rhs) const { return Mul(rhs); }
	float3 MulDir(const float3 &rhs) const { return Mul(rhs); }

	MatrixProxy<Cols> &operator[](int row);
	const MatrixProxy<Cols> &operator[](int row) const;
};

class ALIGN16 AABB
{
public:

	/// Specifies the minimum extent of this AABB in the world space x, y and z axes.
	ALIGN16 float3 minPoint;
	float padding;

	/// Specifies the maximum extent of this AABB in the world space x, y and z axes. [similarOverload: minPoint]
	ALIGN16 float3 maxPoint;
	float padding2;

	/// The default constructor does not initialize any members of this class.
	/** This means that the values of the members minPoint and maxPoint are undefined after creating a new AABB using this
	default constructor. Remember to assign to them before use.
	@see minPoint, maxPoint. */
	AABB() {}

	/// Constructs this AABB by specifying the minimum and maximum extending corners of the box.
	/** @see minPoint, maxPoint. */
	AABB(const float3 &minPoint, const float3 &maxPoint);

	/// Constructs this AABB to enclose the given OBB.
	/** This constructor computes the optimal minimum volume AABB that encloses the given OBB.
	@note Since an AABB cannot generally represent an OBB, this conversion is not exact, but the returned AABB
	specifies a larger volume.
	@see class OBB. */
	explicit AABB(const OBB &obb);

	/// Constructs this AABB to enclose the given Sphere.
	/** @see class Sphere. */
	explicit AABB(const Sphere &s);

	inline static int NumFaces() { return 6; }
	inline static int NumEdges() { return 12; }
	inline static int NumVertices() { return 8; }

	/// Returns the minimum world-space coordinate along the given axis.
	float MinX() const { return minPoint.x; }
	float MinY() const { return minPoint.y; } ///< [similarOverload: MinX]
	float MinZ() const { return minPoint.z; } ///< [similarOverload: MinX]
											  /// Returns the maximum world-space coordinate along the given axis.
	float MaxX() const { return maxPoint.x; }
	float MaxY() const { return maxPoint.y; } ///< [similarOverload: MaxX]
	float MaxZ() const { return maxPoint.z; } ///< [similarOverload: MaxX]

											  /// Sets this structure to a degenerate AABB that does not have any volume.
											  /** This function is useful for initializing the AABB to "null" before a loop of calls to Enclose(),
											  which incrementally expands the bounds of this AABB to enclose the given objects.
											  @see Enclose(). */
	void SetNegativeInfinity();

	/// Sets this AABB by specifying its center and size.
	/** @param center The center point of this AABB.
	@param size A vector that specifies the size of this AABB in x, y and z directions.
	@see SetFrom(), FromCenterAndSize(). */
	void SetFromCenterAndSize(const float3 &center, const float3 &size);

	/// Sets this AABB to enclose the given OBB.
	/** This function computes the minimal axis-aligned bounding box for the given oriented bounding box. If the orientation
	of the OBB is not aligned with the world axes, this conversion is not exact and loosens the volume of the bounding box.
	@param obb The oriented bounding box to convert into this AABB.
	@todo Implement SetFrom(Polyhedron).
	@see SetCenter(), class OBB. */
	void SetFrom(const OBB &obb);

	// Computes the minimal enclosing AABB of the given polyhedron.		
	/* This function computes the smallest AABB (in terms of volume) that contains the given polyhedron, and stores
	the result in this structure.
	@note An AABB cannot generally exactly represent a polyhedron. Converting a polyhedron to an AABB loses some
	features of the polyhedron.
	@return If the given polyhedron is closed, this function succeeds and returns true. If the polyhedron is uncapped
	(has infinite volume), this function does not modify this data structure, but returns false. */
	//	bool SetFrom(const Polyhedron &polyhedron);

	/// Sets this AABB to enclose the given sphere.
	/** This function computes the smallest possible AABB (in terms of volume) that contains the given sphere, and stores the result in this structure. */
	void SetFrom(const Sphere &s);

	/// Sets this AABB to enclose the given set of points.
	/** @param pointArray A pointer to an array of points to enclose inside an AABB.
	@param numPoints The number of elements in the pointArray list.
	@see MinimalEnclosingAABB(). */
	void SetFrom(const float3 *pointArray, int numPoints);

	/// Converts this AABB to a polyhedron.
	/** This function returns a polyhedron representation of this AABB. This conversion is exact, meaning that the returned
	polyhedron represents the same set of points than this AABB.
	@see class Polyhedron. */
	Polyhedron ToPolyhedron() const;

	/// Converts this AABB to an OBB.
	/** This function returns an OBB representation of this AABB. This conversion is exact, meaning that the returned
	OBB represents the same set of points than this AABB.
	@see class OBB. */
	OBB ToOBB() const;

	/// Returns the smallest sphere that contains this AABB.
	/** This function computes the minimal volume sphere that contains all the points inside this AABB.
	@see MaximalContainedSphere(). */
	Sphere MinimalEnclosingSphere() const;

	/// Returns the largest sphere that can fit inside this AABB.
	/** This function computes the largest sphere that can fit inside this AABB. This sphere is unique up to the center point
	of the sphere. The returned sphere will be positioned to the same center point as this AABB.
	@see MinimalEnclosingSphere(). */
	Sphere MaximalContainedSphere() const;

	/// Tests if this AABB is finite.
	/** @return True if the member variables of this AABB are valid floats and do not contain NaNs or infs, and false otherwise.
	@see IsDegenerate(), minPoint, maxPoint. */
	bool IsFinite() const;

	/// Tests if this AABB is degenerate.
	/** @return True if this AABB does not span a strictly positive volume.
	@see IsFinite(), Volume(), minPoint, maxPoint. */
	bool IsDegenerate() const;

	/// @return The center point of this AABB.
	float3 CenterPoint() const;
	/// [similarOverload: CenterPoint]
	float3 Centroid() const { return CenterPoint(); }

	/// Generates a point inside this AABB.
	/** @param x A normalized value between [0,1]. This specifies the point position along the world x axis.
	@param y A normalized value between [0,1]. This specifies the point position along the world y axis.
	@param z A normalized value between [0,1]. This specifies the point position along the world z axis.
	@return A point inside this AABB at point specified by given parameters.
	@see Edge(), CornerPoint(), PointOnEdge(), FaceCenterPoint(), FacePoint(). */
	float3 PointInside(float x, float y, float z) const;

	/// Returns an edge of this AABB.
	/** @param edgeIndex The index of the edge line segment to get, in the range [0, 11].
	@todo Specify which index generates which edge.
	@see PointInside(), CornerPoint(), PointOnEdge(), FaceCenterPoint(), FacePoint(). */
	LineSegment Edge(int edgeIndex) const;

	/// Returns a corner point of this AABB.
	/** This function generates one of the eight corner points of this AABB.
	@param cornerIndex The index of the corner point to generate, in the range [0, 7].
	The points are returned in the order 0: ---, 1: --+, 2: -+-, 3: -++, 4: +--, 5: +-+, 6: ++-, 7: +++. (corresponding the XYZ axis directions).
	@todo Draw which index generates which corner point.
	@see PointInside(), Edge(), PointOnEdge(), FaceCenterPoint(), FacePoint(), GetCornerPoints(). */
	float3 CornerPoint(int cornerIndex) const;

	/// Computes an extreme point of this AABB in the given direction.
	/** An extreme point is a farthest point of this AABB in the given direction. Given a direction,
	this point is not necessarily unique.
	@param direction The direction vector of the direction to find the extreme point. This vector may
	be unnormalized, but may not be null.
	@return An extreme point of this AABB in the given direction. The returned point is always a
	corner point of this AABB.
	@see CornerPoint(). */
	float3 ExtremePoint(const float3 &direction) const;

	/// Returns a point on an edge of this AABB.
	/** @param edgeIndex The index of the edge to generate a point to, in the range [0, 11]. @todo Document which index generates which one.
	@param u A normalized value between [0,1]. This specifies the relative distance of the point along the edge.
	@see PointInside(), CornerPoint(), CornerPoint(), FaceCenterPoint(), FacePoint(). */
	float3 PointOnEdge(int edgeIndex, float u) const;

	/// Returns the point at the center of the given face of this AABB.
	/** @param faceIndex The index of the AABB face to generate the point at. The valid range is [0, 5].
	This index corresponds to the planes in the order (-X, +X, -Y, +Y, -Z, +Z).
	@see PointInside(), CornerPoint(), PointOnEdge(), PointOnEdge(), FacePoint(). */
	float3 FaceCenterPoint(int faceIndex) const;

	/// Generates a point at the surface of the given face of this AABB.
	/** @param faceIndex The index of the AABB face to generate the point at. The valid range is [0, 5].
	This index corresponds to the planes in the order (-X, +X, -Y, +Y, -Z, +Z).
	@param u A normalized value between [0, 1].
	@param v A normalized value between [0, 1].
	@see PointInside(), CornerPoint(), PointOnEdge(), PointOnEdge(), FaceCenterPoint(). */
	float3 FacePoint(int faceIndex, float u, float v) const;

	/// Returns the surface normal direction vector the given face points towards.
	/** @param faceIndex The index of the AABB face to generate the point at. The valid range is [0, 5].
	This index corresponds to the planes in the order (-X, +X, -Y, +Y, -Z, +Z).
	@see FacePoint(), FacePlane(). */
	float3 FaceNormal(int faceIndex) const;

	/// Computes the plane equation of the given face of this AABB.
	/** @param faceIndex The index of the AABB face. The valid range is [0, 5].
	This index corresponds to the planes in the order (-X, +X, -Y, +Y, -Z, +Z).
	@return The plane equation the specified face lies on. The normal of this plane points outwards from this AABB.
	@see FacePoint(), FaceNormal(), GetFacePlanes(). */
	Plane FacePlane(int faceIndex) const;

	/// Fills an array with all the eight corner points of this AABB.
	/** @param outPointArray [out] The array to write the points to. Must have space for 8 elements.
	@see CornerPoint(). */
	void GetCornerPoints(float3 *outPointArray) const;

	/// Fills an array with all the six planes of this AABB.
	/** @param outPlaneArray [out] The array to write the planes to. Must have space for 6 elements.
	@see FacePlane(). */
	void GetFacePlanes(Plane *outPlaneArray) const;

	/// Generates an AABB that encloses the given point set.
	/** This function finds the smallest AABB that contains the given set of points.
	@param pointArray A pointer to an array of points to enclose inside an AABB.
	@param numPoints The number of elements in the pointArray list.
	@see SetFrom(). */
	static AABB MinimalEnclosingAABB(const float3 *pointArray, int numPoints);

	/// Finds the most extremal points along the three world axes simultaneously.
	/** @param pointArray A pointer to an array of points to process.
	@param numPoints The number of elements in the pointArray list.
	@param minx [out] Receives the point that has the smallest x coordinate.
	@param maxx [out] Receives the point that has the largest x coordinate.
	@param miny [out] Receives the point that has the smallest y coordinate.
	@param maxy [out] Receives the point that has the largest y coordinate.
	@param minz [out] Receives the point that has the smallest z coordinate.
	@param maxz [out] Receives the point that has the largest z coordinate. */
	static void ExtremePointsAlongAABB(const float3 *pointArray, int numPoints, int &minx, int &maxx, int &miny, int &maxy, int &minz, int &maxz);

	/// Creates a new AABB given is center position and size along the X, Y and Z axes.
	/** @see SetCenter(). */
	static AABB FromCenterAndSize(const float3 &aabbCenterPos, const float3 &aabbSize);

	/// Returns the side lengths of this AABB in x, y and z directions.
	/** The returned vector is equal to the diagonal vector of this AABB, i.e. it spans from the
	minimum corner of the AABB to the maximum corner of the AABB.
	@see HalfSize(), Diagonal(). */
	float3 Size() const;

	/// [similarOverload: Size]
	/** Returns Size()/2.
	@see Size(), HalfDiagonal(). */
	float3 HalfSize() const;

	/// Returns the diameter vector of this AABB.
	/** @note For AABB, Diagonal() and Size() are the same concept. These functions are provided for symmetry
	with the OBB class.
	@see Size(), HalfDiagonal(). */
	float3 Diagonal() const { return Size(); }

	/// [similarOverload: Diagonal]
	/** Returns Diagonal()/2.
	@see Diagonal(), HalfSize(). */
	float3 HalfDiagonal() const { return HalfSize(); }

	/// Computes the volume of this AABB.
	/** @see SurfaceArea(), IsDegenerate(). */
	float Volume() const;

	/// Computes the surface area of the faces of this AABB.
	/** @see Volume(). */
	float SurfaceArea() const;

	/// Generates a random point inside this AABB.
	/** The points are distributed uniformly.
	@see RandomPointOnSurface(), RandomPointOnEdge(), RandomCornerPoint(). */
	float3 RandomPointInside(LCG &rng) const;

	/// Generates a random point on a random face of this AABB.
	/** The points are distributed uniformly.
	@see RandomPointInside(), RandomPointOnEdge(), RandomCornerPoint(). */
	float3 RandomPointOnSurface(LCG &rng) const;

	/// Generates a random point on a random edge of this AABB.
	/** The points are distributed uniformly.
	@see RandomPointInside(), RandomPointOnSurface(), RandomCornerPoint(). */
	float3 RandomPointOnEdge(LCG &rng) const;

	/// Picks a random corner point of this AABB.
	/** The points are distributed uniformly.
	@see RandomPointInside(), RandomPointOnSurface(), RandomPointOnEdge(). */
	float3 RandomCornerPoint(LCG &rng) const;

	/// Translates this AABB in world space.
	/** @param offset The amount of displacement to apply to this AABB, in world space coordinates.
	@see Scale(), Transform(). */
	void Translate(const float3 &offset);

	/// Applies a uniform scale to this AABB.
	/** This function scales this AABB structure in-place, using the given center point as the origin
	for the scaling operation.
	@param centerPoint Specifies the center of the scaling operation, in world space.
	@param scaleFactor The uniform scale factor to apply to each world space axis.
	@see Translate(), Transform(). */
	void Scale(const float3 &centerPoint, float scaleFactor);

	/// Applies a non-uniform scale to this AABB.
	/** This function scales this AABB structure in-place, using the given center point as the origin
	for the scaling operation.
	@param centerPoint Specifies the center of the scaling operation, in world space.
	@param scaleFactor The non-uniform scale factors to apply to each world space axis.
	@see Translate(), Transform(). */
	void Scale(const float3 &centerPoint, const float3 &scaleFactor);

	/// Applies a transformation to this AABB.
	/** This function transforms this AABB with the given transformation, and then recomputes this AABB
	to enclose the resulting oriented bounding box. This transformation is not exact and in general, calling
	this function results in the loosening of the AABB bounds.
	@param transform The transformation to apply to this AABB. This function assumes that this
	transformation does not contain shear, nonuniform scaling or perspective properties, i.e. that the fourth
	row of the float4x4 is [0 0 0 1].
	@see Translate(), Scale(), Transform(), classes float3x3, float3x4, float4x4, Quat. */
	void TransformAsAABB(const float3x3 &transform);
	void TransformAsAABB(const Quat &transform);

	/// Applies a transformation to this AABB and returns the resulting OBB.
	/** Transforming an AABB produces an oriented bounding box. This set of functions does not apply the transformation
	to this object itself, but instead returns the OBB that results in the transformation.
	@param transform The transformation to apply to this AABB. This function assumes that this
	transformation does not contain shear, nonuniform scaling or perspective properties, i.e. that the fourth
	row of the float4x4 is [0 0 0 1].
	@see Translate(), Scale(), TransformAsAABB(), classes float3x3, float3x4, float4x4, Quat. */
	OBB Transform(const float3x3 &transform) const;
	OBB Transform(const Quat &transform) const;

	/// Computes the closest point inside this AABB to the given point.
	/** If the target point lies inside this AABB, then that point is returned.
	@see Distance(), Contains(), Intersects().
	@todo Add ClosestPoint(Line/Ray/LineSegment/Plane/Triangle/Polygon/Circle/Disc/AABB/OBB/Sphere/Capsule/Frustum/Polyhedron). */
	float3 ClosestPoint(const float3 &targetPoint) const;

	/// Computes the distance between this AABB and the given object.
	/** This function finds the nearest pair of points on this and the given object, and computes their distance.
	If the two objects intersect, or one object is contained inside the other, the returned distance is zero.
	@todo Add AABB::Distance(Line/Ray/LineSegment/Plane/Triangle/Polygon/Circle/Disc/AABB/OBB/Capsule/Frustum/Polyhedron).
	@see Contains(), Intersects(), ClosestPoint(). */
	float Distance(const float3 &point) const;
	float Distance(const Sphere &sphere) const;

	/// Tests if the given object is fully contained inside this AABB.
	/** This function returns true if the given object lies inside this AABB, and false otherwise.
	@note The comparison is performed using less-or-equal, so the faces of this AABB count as being inside, but
	due to float inaccuracies, this cannot generally be relied upon.
	@todo Add Contains(Circle/Disc/Sphere/Capsule).
	@see Distance(), Intersects(), ClosestPoint(). */
	bool Contains(const float3 &point) const;
	bool Contains(const LineSegment &lineSegment) const;
	bool Contains(const AABB &aabb) const;
	bool Contains(const OBB &obb) const;
	bool Contains(const Sphere &sphere) const;
	bool Contains(const Triangle &triangle) const;
	bool Contains(const Polyhedron &polyhedron) const;
	bool Contains(const Capsule &capsule) const;

	/// Tests whether this AABB and the given object intersect.
	/** Both objects are treated as "solid", meaning that if one of the objects is fully contained inside
	another, this function still returns true. (e.g. in case a line segment is contained inside this AABB,
	or this AABB is contained inside a Sphere, etc.)
	@param ray The first parameter of this function specifies the other object to test against.
	@param dNear [out] If specified, receives the parametric distance along the line denoting where the
	line entered this AABB.
	@param dFar [out] If specified, receives the parametric distance along the line denoting where the
	line exited this AABB.
	@see Contains(), Distance(), ClosestPoint().
	@note If you do not need the intersection intervals, you should call the functions without these
	parameters in the function signature for optimal performance.
	@todo Add Intersects(Circle/Disc). */
	bool Intersects(const Ray &ray, float &dNear, float &dFar) const;
	bool Intersects(const Ray &ray) const;
	bool Intersects(const Line &line, float &dNear, float &dFar) const;
	bool Intersects(const Line &line) const;
	bool Intersects(const LineSegment &lineSegment, float &dNear, float &dFar) const;
	bool Intersects(const LineSegment &lineSegment) const;
	bool Intersects(const Plane &plane) const;
	bool Intersects(const AABB &aabb) const;
	bool Intersects(const OBB &obb) const;
	/** For reference documentation on the Sphere-AABB intersection test, see Christer Ericson's Real-Time Collision Detection, p. 165. [groupSyntax]
	@param sphere The first parameter of this function specifies the other object to test against.
	@param closestPointOnAABB [out] Returns the closest point on this AABB to the given sphere. This pointer
	may be null. */
	bool Intersects(const Sphere &sphere, float3 *closestPointOnAABB = 0) const;
	bool Intersects(const Capsule &capsule) const;
	bool Intersects(const Triangle &triangle) const;
	bool Intersects(const Polygon &polygon) const;
	bool Intersects(const Frustum &frustum) const;
	bool Intersects(const Polyhedron &polyhedron) const;

	/// Projects this AABB onto the given axis.
	/** @param axis The axis to project onto. This vector can be unnormalized.
	@param dMin [out] Returns the minimum extent of this AABB on the given axis.
	@param dMax [out] Returns the maximum extent of this AABB on the given axis. */
	void ProjectToAxis(const float3 &axis, float &dMin, float &dMax) const;

	/// Expands this AABB to enclose the given object.
	/** This function computes an AABB that encloses both this AABB and the specified object, and stores the resulting
	AABB into this.
	@note The generated AABB is not necessarily the optimal enclosing AABB for this AABB and the given object. */
	void Enclose(const float3 &point);
	void Enclose(const LineSegment &lineSegment);
	void Enclose(const AABB &aabb);
	void Enclose(const OBB &obb);
	void Enclose(const Sphere &sphere);
	void Enclose(const Triangle &triangle);
	void Enclose(const Capsule &capsule);
	void Enclose(const Polyhedron &polyhedron);
	void Enclose(const float3 *pointArray, int numPoints);

	/// Generates an unindexed triangle mesh representation of this AABB.
	/** @param numFacesX The number of faces to generate along the X axis. This value must be >= 1.
	@param numFacesY The number of faces to generate along the Y axis. This value must be >= 1.
	@param numFacesZ The number of faces to generate along the Z axis. This value must be >= 1.
	@param outPos [out] An array of size numVertices which will receive a triangle list
	of vertex positions. Cannot be null.
	@param outNormal [out] An array of size numVertices which will receive vertex normals.
	If this parameter is null, vertex normals are not returned.
	@param outUV [out] An array of size numVertices which will receive vertex UV coordinates.
	If this parameter is null, a UV mapping is not generated.
	@param ccwIsFrontFacing If true, then the front-facing direction of the faces will be the sides
	with counterclockwise winding order. Otherwise, the faces are generated in clockwise winding order.
	The number of vertices that outPos, outNormal and outUV must be able to contain is
	(x*y + x*z + y*z)*2*6. If x==y==z==1, then a total of 36 vertices are required. Call
	NumVerticesInTriangulation to obtain this value.
	@see ToPolyhedron(), ToEdgeList(), NumVerticesInTriangulation(). */
	void Triangulate(int numFacesX, int numFacesY, int numFacesZ,
		float3 *outPos, float3 *outNormal, float2 *outUV,
		bool ccwIsFrontFacing) const;

	/// Returns the number of vertices that the Triangulate() function will output with the given subdivision parameters.
	/** @see Triangulate(). */
	static int NumVerticesInTriangulation(int numFacesX, int numFacesY, int numFacesZ)
	{
		return (numFacesX*numFacesY + numFacesX*numFacesZ + numFacesY*numFacesZ) * 2 * 6;
	}

	/// Generates an edge list representation of the edges of this AABB.
	/** @param outPos [out] An array that contains space for at least 24 vertices (NumVerticesInEdgeList()).
	@see Triangulate(), Edge(), NumVerticesInEdgeList(). */
	void ToEdgeList(float3 *outPos) const;

	/// Returns the number of vertices that the ToEdgeList() function will output.
	/** @see ToEdgeList(). */
	static int NumVerticesInEdgeList()
	{
		return 4 * 3 * 2;
	}

	/// Finds the set intersection of this and the given AABB.
	/** @return This function returns the AABB that is contained in both this and the given AABB.
	@todo Add Intersection(OBB/Polyhedron). */
	AABB Intersection(const AABB &aabb) const;

	// Finds the set intersection of this AABB and the given OBB.
	/* @return This function returns a Polyhedron that represents the set of points that are contained in this AABB
	and the given OBB. */
	//	Polyhedron Intersection(const OBB &obb) const;

	// Finds the set intersection of this AABB and the given Polyhedron.
	/* @return This function returns a Polyhedron that represents the set of points that are contained in this AABB
	and the given Polyhedron. */
	//	Polyhedron Intersection(const Polyhedron &polyhedron) const;

	/// Computes the intersection of a line, ray or line segment and an AABB.
	/** Based on "T. Kay, J. Kajiya. Ray Tracing Complex Scenes. SIGGRAPH 1986 vol 20, number 4. pp. 269-"
	http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
	@param linePos The starting position of the line.
	@param lineDir The direction of the line. This direction vector must be normalized!
	@param tNear [in, out] For the test, the input line is treated as a line segment. Pass in the signed distance
	from the line origin to the start of the line. For a Line-AABB test, -FLOAT_INF is typically passed here.
	For a Ray-AABB test, 0.0f should be inputted. If intersection occurs, the signed distance from line origin
	to the line entry point in the AABB is returned here.
	@param tFar [in, out] Pass in the signed distance from the line origin to the end of the line. For Line-AABB and
	Ray-AABB tests, pass in FLOAT_INF. For a LineSegment-AABB test, pass in the length of the line segment here.
	If intersection occurs, the signed distance from line origin to the line exit point in the AABB
	is returned here.
	@return True if an intersection occurs, false otherwise.
	@note This is a low level utility function. It may be more convenient to use one of the AABB::Intersects()
	functions instead.
	@see Intersects(). */
	bool IntersectLineAABB(const float3 &linePos, const float3 &lineDir, float &tNear, float &tFar) const;

	bool IntersectLineAABB_CPP(const float3 &linePos, const float3 &lineDir, float &tNear, float &tFar) const;

	bool Equals(const AABB &rhs, float epsilon = 1e-3f) const { return minPoint.Equals(rhs.minPoint, epsilon) && maxPoint.Equals(rhs.maxPoint, epsilon); }
};

class OBB
{
public:
	/// The center position of this OBB.
	/** In the local space of the OBB, the center of this OBB is at (r.x,r.y,r.z), and the OBB is an AABB with
	size 2*r. */
	float3 pos;

	/// Stores half-sizes to x, y and z directions in the local space of this OBB. [similarOverload: pos]
	/** These members should be positive to represent a non-degenerate OBB. */
	float3 r;

	/// Specifies normalized direction vectors for the local axes. [noscript] [similarOverload: pos]
	/** axis[0] specifies the +X direction in the local space of this OBB, axis[1] the +Y direction and axis[2]
	the +Z direction.
	The scale of these vectors is always normalized. The half-length of the OBB along its local axes is
	specified by the vector r.
	The axis vectors must always be orthonormal. Be sure to guarantee that condition holds if you
	directly set to this member variable. */
	float3 axis[3];

	/// The default constructor does not initialize any members of this class. [opaque-qtscript]
	/** This means that the values of the members pos, r and axis are undefined after creating a new OBB using this
	default constructor. Remember to assign to them before use.
	@see pos, r, axis. */
	OBB() {}

	/// Constructs an OBB from an AABB.
	/** Since the OBB is an AABB with arbirary rotations allowed, this conversion is exact, i.e. it does not loosen
	the set of points represented by the AABB. Therefore this constructor is implicit, meaning that you
	can directly assign an AABB to an OBB.
	@note Converting an OBB back to an AABB is not exact. See the MinimalEnclosingAABB() function for
	converting to the opposite direction.
	@see class AABB, SetFrom(), MinimalEnclosingAABB(). */
	OBB(const AABB &aabb);

	inline static int NumFaces() { return 6; }
	inline static int NumEdges() { return 12; }
	inline static int NumVertices() { return 8; }

	/// Sets this structure to a degenerate OBB that does not have any volume.
	/** This function sets pos=(0,0,0), r = (-inf,-inf,-inf) and axis=float3x3::identity for this OBB.
	@note This function operates in-place. After calling this function, this OBB is degenerate.
	@see pos, r, axis, IsDegenerate(). */
	void SetNegativeInfinity();

	/// Sets this OBB from an AABB.
	/** This conversion is exact, and does not loosen the volume.
	@note Converting an OBB back to an AABB is not exact. See the MinimalEnclosingAABB() function for
	converting to the opposite direction.
	@param aabb The axis-aligned bounding box to convert to an OBB.
	@see classes AABB, float3x3, float3x4, float4x4, Quat, MinimalEnclosingAABB(). */
	void SetFrom(const AABB &aabb);
	/** @param transform If a transformation matrix is supplied, this transformation is applied to the AABB before
	representing it as an oriented bounding box. The basis of this matrix is assumed to be orthogonal, which
	means no projection or shear is allowed. Additionally, the matrix must contain only uniform scaling. */
	void SetFrom(const AABB &aabb, const float3x3 &transform);
	void SetFrom(const AABB &aabb, const Quat &transform);

	/// Sets this OBB to enclose the given sphere.
	/** This function computes the tightest possible OBB (in terms of volume) that contains the given sphere, and stores the result in this structure.
	@note Since an OBB is a box, and Sphere is, well, a sphere, this conversion is not exact, but loosens the set of points in the representation.
	@see class Sphere, MinimalEnclosingSphere(). */
	void SetFrom(const Sphere &sphere);

	/// Converts this OBB to a polyhedron.
	/** This function returns a polyhedron representation of this OBB. This conversion is exact, meaning that the returned
	polyhedron represents the same set of points than this OBB. */
	Polyhedron ToPolyhedron() const;

	/// Returns the tightest AABB that contains this OBB.
	/** This function computes the optimal minimum volume AABB that encloses this OBB.
	@note Since an AABB cannot generally represent an OBB, this conversion is not exact, but the returned AABB
	specifies a larger volume.
	@see SetFrom(), MaximalContainedAABB(), MinimalEnclosingSphere(), MaximalContainedSphere(). */
	AABB MinimalEnclosingAABB() const;

	/// Returns the smallest sphere that contains this OBB.
	/** This function computes the optimal minimum volume sphere that encloses this OBB.
	@see MinimalEnclosingAABB(), MaximalContainedAABB(), MaximalContainedSphere(). */
	Sphere MinimalEnclosingSphere() const;

	/// Returns the largest sphere that can fit inside this OBB. [similarOverload: MinimalEnclosingSphere]
	/** This function computes the largest sphere that can fit inside this OBB. This sphere is unique up to the center point
	of the sphere. The returned sphere will be positioned to the same center point as this OBB.
	@see MinimalEnclosingSphere(), MaximalContainedAABB(), MaximalContainedSphere(). */
	Sphere MaximalContainedSphere() const;

	/// Returns the side lengths of this OBB in its local x, y and z directions.
	/** @return 2*r. */
	float3 Size() const;

	/// Returns the half-side lengths of this OBB in its local x, y and z directions. [similarOverload: Size]
	/** @return r.
	@see r, Size(), HalfSize(). */
	float3 HalfSize() const;

	/// Returns a diagonal vector of this OBB.
	/** This vector runs from one corner of the OBB from the opposite corner.
	@note A box has four diagonals. This function returns the direction vector from the -X-Y-Z corner to
	the +X+Y+Z corner of the OBB, in the global space of this OBB. */
	float3 Diagonal() const;
	/// Returns Diagonal()/2. [similarOverload: Diagonal].
	/** @return A direction vector from the center of this OBB to the +X+Y+Z corner of this OBB, in global space.
	@see Size(), HalfSize(). */
	float3 HalfDiagonal() const;

	/// Tests if this OBB is finite.
	/** @return True if the member variables of this OBB are valid floats and do not contain NaNs or infs, and false otherwise.
	@see IsDegenerate(). */
	bool IsFinite() const;

	/// Tests if this OBB is degenerate.
	/** @return True if this OBB does not span a strictly positive volume.
	@see r, Volume(). */
	bool IsDegenerate() const;

	/// Returns the center point of this OBB in global (world) space of this OBB.
	/** @note The center point of this OBB in local space is always (r.x, r.y, r.z).
	@see pos.  */
	float3 CenterPoint() const;

	/// Returns the center of mass of this OBB. [similarOverload: CenterPoint]
	/** @note This function is identical to CenterPoint(), and is provided to ease template function implementations.
	@see Volume(), SurfaceArea(). */
	float3 Centroid() const { return CenterPoint(); }

	/// Computes the volume of this OBB.
	/** @see CenterPoint(), SurfaceArea(). */
	float Volume() const;

	/// Computes the total surface area of the faces of this OBB.
	/** @see CenterPoint(), Volume(). */
	float SurfaceArea() const;

	/// Generates a point inside this OBB.
	/** @param x A normalized value between [0,1]. This specifies the point position along the local x axis of the OBB.
	@param y A normalized value between [0,1]. This specifies the point position along the local y axis of the OBB.
	@param z A normalized value between [0,1]. This specifies the point position along the local z axis of the OBB.
	@return A point in the global space of this OBB corresponding to the parametric coordinate (x,y,z).
	@see Edge(), CornerPoint(), PointOnEdge(), FaceCenterPoint(), FacePoint(). */
	float3 PointInside(float x, float y, float z) const;

	/// Returns an edge of this OBB.
	/** @param edgeIndex The index of the edge line segment to get, in the range [0, 11].
	@todo Draw a diagram that shows which index generates which edge.
	@see PointInside(), CornerPoint(), PointOnEdge(), FaceCenterPoint(), FacePoint(). */
	LineSegment Edge(int edgeIndex) const;

	/// Returns a corner point of this OBB.
	/** This function generates one of the eight corner points of this OBB.
	@param cornerIndex The index of the corner point to generate, in the range [0, 7].
	The points are returned in the order 0: ---, 1: --+, 2: -+-, 3: -++, 4: +--, 5: +-+, 6: ++-, 7: +++. (corresponding the XYZ axis directions).
	@todo Draw a diagram that shows which index generates which edge.
	@see PointInside(), Edge(), PointOnEdge(), FaceCenterPoint(), FacePoint(). */
	float3 CornerPoint(int cornerIndex) const;

	/// Computes an extreme point of this OBB in the given direction.
	/** An extreme point is a farthest point of this OBB in the given direction. Given a direction,
	this point is not necessarily unique.
	@param direction The direction vector of the direction to find the extreme point. This vector may
	be unnormalized, but may not be null.
	@return An extreme point of this OBB in the given direction. The returned point is always a
	corner point of this OBB.
	@see CornerPoint(). */
	float3 ExtremePoint(const float3 &direction) const;

	/// Projects this OBB onto the given 1D axis direction vector.
	/** This function collapses this OBB onto an 1D axis for the purposes of e.g. separate axis test computations.
	The function returns a 1D range [outMin, outMax] denoting the interval of the projection.
	@param direction The 1D axis to project to. This vector may be unnormalized, in which case the output
	of this function gets scaled by the length of this vector.
	@param outMin [out] Returns the minimum extent of this object along the projection axis.
	@param outMax [out] Returns the maximum extent of this object along the projection axis. */
	void ProjectToAxis(const float3 &direction, float &outMin, float &outMax) const;

	/// Returns a point on an edge of this OBB.
	/** @param edgeIndex The index of the edge to generate a point to, in the range [0, 11]. @todo Document which index generates which one.
	@param u A normalized value between [0,1]. This specifies the relative distance of the point along the edge.
	@see PointInside(), Edge(), CornerPoint(), FaceCenterPoint(), FacePoint(). */
	float3 PointOnEdge(int edgeIndex, float u) const;

	/// Returns the point at the center of the given face of this OBB.
	/** @param faceIndex The index of the OBB face to generate the point at. The valid range is [0, 5].
	@todo Document which index generates which face.
	@see PointInside(), Edge(), CornerPoint(), PointOnEdge(), FacePoint(). */
	float3 FaceCenterPoint(int faceIndex) const;

	/// Generates a point at the surface of the given face of this OBB.
	/** @param faceIndex The index of the OBB face to generate the point at. The valid range is [0, 5].
	@param u A normalized value between [0, 1].
	@param v A normalized value between [0, 1].
	@todo Document which index generates which face.
	@see PointInside(), Edge(), CornerPoint(), PointOnEdge(), FaceCenterPoint(), FacePlane(). */
	float3 FacePoint(int faceIndex, float u, float v) const;

	/// Returns the plane of the given face of this OBB.
	/** The normal of the plane points outwards from this OBB, i.e. towards the space that
	is not part of the OBB.
	@param faceIndex The index of the face to get, in the range [0, 5].
	@see PointInside(), Edge(), CornerPoint(), PointOnEdge(), FaceCenterPoint(), FacePoint(). */
	Plane FacePlane(int faceIndex) const;

	/// Fills an array with all the eight corner points of this OBB.
	/** @param outPointArray [out] The array to write the points to. Must have space for 8 elements.
	@see CornerPoint(), GetFacePlanes(). */
	void GetCornerPoints(float3 *outPointArray) const;

	/// Fills an array with all the six planes of this OBB.
	/** @param outPlaneArray [out] The array to write the planes to. Must have space for 6 elements.
	@see FacePlane(), GetCornerPoints(). */
	void GetFacePlanes(Plane *outPlaneArray) const;

	/// Finds the two extreme points along the given direction vector from the given point array.
	/** @param dir The direction vector to project the point array to. This vector does not need to be normalized.
	@param pointArray [in] The list of points to process.
	@param numPoints The number of elements in pointArray.
	@param idxSmallest [out] The index of the smallest point along the given direction will be received here.
	This pointer may be left null, if this information is of no interest.
	@param idxLargest [out] The index of the largest point along the given direction will be received here.
	This pointer may be left null, if this information is of no interest. */
	static void ExtremePointsAlongDirection(const float3 &dir, const float3 *pointArray, int numPoints, int &idxSmallest, int &idxLargest);

	/// Generates a random point inside this OBB.
	/** The points are distributed uniformly.
	@see class LCG, PointInside(), RandomPointOnSurface(), RandomPointOnEdge(), RandomCornerPoint(). */
	float3 RandomPointInside(LCG &rng) const;

	/// Generates a random point on a random face of this OBB.
	/** The points are distributed uniformly.
	@see class LCG, FacePoint(), RandomPointInside(), RandomPointOnEdge(), RandomCornerPoint(). */
	float3 RandomPointOnSurface(LCG &rng) const;

	/// Generates a random point on a random edge of this OBB.
	/** The points are distributed uniformly.
	@see class LCG, PointOnEdge(), RandomPointInside(), RandomPointOnSurface(), RandomCornerPoint(). */
	float3 RandomPointOnEdge(LCG &rng) const;

	/// Picks a random corner point of this OBB.
	/** The points are distributed uniformly.
	@see class LCG, CornerPoint(), RandomPointInside(), RandomPointOnSurface(), RandomPointOnEdge(). */
	float3 RandomCornerPoint(LCG &rng) const;

	/// Translates this OBB in world space.
	/** @param offset The amount of displacement to apply to this OBB, in world space coordinates.
	@see Scale(), Transform(). */
	void Translate(const float3 &offset);

	/// Applies a uniform scale to this OBB.
	/** This function scales this OBB structure in-place, using the given center point as the origin
	for the scaling operation.
	@param centerPoint Specifies the center of the scaling operation, in global (world) space.
	@param scaleFactor The uniform scale factor to apply to each global (world) space axis.
	@see Translate(), Transform(). */
	void Scale(const float3 &centerPoint, float scaleFactor);

	/// Applies a non-uniform scale to the local axes of this OBB.
	/** This function scales this OBB structure in-place, using the given global space center point as
	the origin for the scaling operation.
	@param centerPoint Specifies the center of the scaling operation, in global (world) space.
	@param scaleFactor The non-uniform scale factors to apply to each local axis of this OBB.
	@see Translate(), Transform(). */
	void Scale(const float3 &centerPoint, const float3 &scaleFactor);

	/// Applies a transformation to this OBB.
	/** @param transform The transformation to apply to this OBB. This transformation must be affine, and
	must contain an orthogonal set of column vectors (may not contain shear or projection).
	The transformation can only contain uniform scale, and may not contain mirroring.
	@see Translate(), Scale(), classes float3x3, float3x4, float4x4, Quat. */
	void Transform(const float3x3 &transform);
	void Transform(const float3x4 &transform);
	void Transform(const Quat &transform);

	/// Computes the closest point inside this OBB to the given point.
	/** If the target point lies inside this OBB, then that point is returned.
	@see Distance(), Contains(), Intersects().
	@todo Add ClosestPoint(Line/Ray/LineSegment/Plane/Triangle/Polygon/Circle/Disc/AABB/OBB/Sphere/Capsule/Frustum/Polyhedron). */
	float3 ClosestPoint(const float3 &point) const;

	/// Computes the distance between this OBB and the given object.
	/** This function finds the nearest pair of points on this and the given object, and computes their distance.
	If the two objects intersect, or one object is contained inside the other, the returned distance is zero.
	@todo Add OBB::Distance(Line/Ray/LineSegment/Plane/Triangle/Polygon/Circle/Disc/AABB/OBB/Capsule/Frustum/Polyhedron).
	@see Contains(), Intersects(), ClosestPoint(). */
	float Distance(const float3 &point) const;
	float Distance(const Sphere &sphere) const;

	/// Tests if the given object is fully contained inside this OBB.
	/** This function returns true if the given object lies inside this OBB, and false otherwise.
	@note The comparison is performed using less-or-equal, so the faces of this OBB count as being inside, but
	due to float inaccuracies, this cannot generally be relied upon.
	@todo Add Contains(Circle/Disc/Sphere/Capsule).
	@see Distance(), Intersects(), ClosestPoint(). */
	bool Contains(const float3 &point) const;
	bool Contains(const LineSegment &lineSegment) const;
	bool Contains(const AABB &aabb) const;
	bool Contains(const OBB &obb) const;
	bool Contains(const Triangle &triangle) const;
	bool Contains(const Polygon &polygon) const;
	bool Contains(const Frustum &frustum) const;
	bool Contains(const Polyhedron &polyhedron) const;

	/// Tests whether this OBB and the given object intersect.
	/** Both objects are treated as "solid", meaning that if one of the objects is fully contained inside
	another, this function still returns true. (e.g. in case a line segment is contained inside this OBB,
	or this OBB is contained inside a Sphere, etc.)
	The first parameter of this function specifies the other object to test against.
	The OBB-OBB intersection test is from Christer Ericson's book Real-Time Collision Detection, p. 101-106.
	See http://realtimecollisiondetection.net/ [groupSyntax]
	@param obb The other oriented bounding box to test intersection with.
	@param epsilon The OBB-OBB test utilizes a SAT test to detect the intersection. A robust implementation requires
	an epsilon threshold to test that the used axes are not degenerate.
	@see Contains(), Distance(), ClosestPoint().
	@todo Add Intersects(Circle/Disc). */
	bool Intersects(const OBB &obb, float epsilon = 1e-3f) const;
	bool Intersects(const AABB &aabb) const;
	bool Intersects(const Plane &plane) const;
	/** @param dNear [out] If specified, receives the parametric distance along the line denoting where the
	line entered this OBB.
	@param dFar [out] If specified, receives the parametric distance along the line denoting where the
	line exited this OBB. */
	bool Intersects(const Ray &ray, float &dNear, float &dFar) const;
	bool Intersects(const Ray &ray) const;
	bool Intersects(const Line &line, float &dNear, float &dFar) const;
	bool Intersects(const Line &line) const;
	bool Intersects(const LineSegment &lineSegment, float &dNear, float &dFar) const;
	bool Intersects(const LineSegment &lineSegment) const;
	/** @param closestPointOnOBB [out] If specified, receives the closest point on this OBB To the given sphere. This
	pointer may be null. */
	bool Intersects(const Sphere &sphere, float3 *closestPointOnOBB = 0) const;
	bool Intersects(const Capsule &capsule) const;
	bool Intersects(const Triangle &triangle) const;
	bool Intersects(const Polygon &polygon) const;
	bool Intersects(const Frustum &frustum) const;
	bool Intersects(const Polyhedron &polyhedron) const;

	/// Expands this OBB to enclose the given object. The axis directions of this OBB remain intact.
	/** This function operates in-place. This function does not necessarily result in an OBB that is an
	optimal fit for the previous OBB and the given point. */
	void Enclose(const float3 &point);

	/// Generates an unindexed triangle mesh representation of this OBB.
	/** @param numFacesX The number of faces to generate along the X axis. This value must be >= 1.
	@param numFacesY The number of faces to generate along the Y axis. This value must be >= 1.
	@param numFacesZ The number of faces to generate along the Z axis. This value must be >= 1.
	@param outPos [out] An array of size numVertices which will receive a triangle list
	of vertex positions. Cannot be null.
	@param outNormal [out] An array of size numVertices which will receive vertex normals.
	If this parameter is null, vertex normals are not returned.
	@param outUV [out] An array of size numVertices which will receive vertex UV coordinates.
	If this parameter is null, a UV mapping is not generated.
	The number of vertices that outPos, outNormal and outUV must be able to contain is
	(x*y + x*z + y*z)*2*6. If x==y==z==1, then a total of 36 vertices are required. Call
	NumVerticesInTriangulation to obtain this value.
	@see ToPolyhedron(), ToEdgeList(), NumVerticesInTriangulation(). */
	void Triangulate(int numFacesX, int numFacesY, int numFacesZ, float3 *outPos, float3 *outNormal, float2 *outUV, bool ccwIsFrontFacing) const;

	/// Returns the number of vertices that the Triangulate() function will output with the given subdivision parameters.
	/** @see Triangulate(). */
	static int NumVerticesInTriangulation(int numFacesX, int numFacesY, int numFacesZ)
	{
		return (numFacesX*numFacesY + numFacesX*numFacesZ + numFacesY*numFacesZ) * 2 * 6;
	}

	/// Generates an edge list representation of the edges of this OBB.
	/** @param outPos [out] An array that contains space for at least 24 vertices (NumVerticesInEdgeList()).
	@see Triangulate(), Edge(), NumVerticesInEdgeList(). */
	void ToEdgeList(float3 *outPos) const;

	/// Returns the number of vertices that the ToEdgeList() function will output.
	/** @see ToEdgeList(). */
	static int NumVerticesInEdgeList()
	{
		return 4 * 3 * 2;
	}

	/// Computes the transformation matrix that maps from the global (world) space of this OBB to the local space of this OBB.
	/** In local space, the center of the OBB lies at (r.x,r.y,r.z), and the OBB is aligned along the cardinal axes, i.e. is an AABB.
	The local +X vector runs in the direction specified by axis[0], the +Y direction is specified by axis[1], and +Z by axis[2].
	The size of the OBB is 2*r.
	In global (world) space, the center of the OBB lies at the point given by the pos member variable.
	@return This global (world) to local space transform can be represented using a float3x4 matrix. This function computes
	and returns the matrix that maps from the world space of this OBB to the local space of this OBB.
	@see pos, r, axis. */
	float3x4 WorldToLocal() const;

	/// Computes the transformation matrix that maps from the local space of this OBB to the global (world) space of this OBB. [similarOverload: WorldToLocal]
	/** This mapping is the inverse of the transform computed by WorldToLocal().
	@return A matrix that transforms from the local space of this OBB to the global (world) space of this OBB.
	@see pos, r, axis. */
	float3x4 LocalToWorld() const;

	// Finds the set intersection of this and the given OBB.
	/* @return This function returns the Polyhedron that is contained in both this and the given OBB. */
	//	Polyhedron Intersection(const AABB &aabb) const;

	// Finds the set intersection of this and the given OBB.
	/* @return This function returns the Polyhedron that is contained in both this and the given OBB. */
	//	Polyhedron Intersection(const OBB &obb) const;

	// Finds the set intersection of this OBB and the given Polyhedron.
	/* @return This function returns a Polyhedron that represents the set of points that are contained in this OBB
	and the given Polyhedron. */
	//	Polyhedron Intersection(const Polyhedron &polyhedron) const;
};

class Line
{
public:
	/// Specifies the origin of this line.
	float3 pos;

	/// The normalized direction vector of this ray. [similarOverload: pos]
	/** @note For proper functionality, this direction vector needs to always be normalized. If you set to this
	member manually, remember to make sure you only assign normalized direction vectors. */
	float3 dir;

	/// The default constructor does not initialize any members of this class.
	/** This means that the values of the members pos and dir are undefined after creating a new Line using this
	default constructor. Remember to assign to them before use.
	@see pos, dir. */
	Line() {}

	/// Constructs a new line by explicitly specifying the member variables.
	/** @param pos The origin position of the line.
	@param dir The direction of the line. This vector must be normalized, this function will not normalize
	the vector for you (for performance reasons).
	@see pos, dir. */
	Line(const float3 &pos, const float3 &dir);

	/// Converts a Ray to a Line.
	/** This conversion simply copies the members pos and dir over from the given Ray to this Line.
	This means that the new Line starts at the same position, but extends to two directions in space,
	instead of one.
	@see class Ray, ToRay(). */
	explicit Line(const Ray &ray);

	/// Converts a LineSegment to a Line.
	/** This constructor sets pos = lineSegment.a, and dir = (lineSegment.b - lineSegment.a).Normalized().
	@see class LineSegment, ToLineSegment(). */
	explicit Line(const LineSegment &lineSegment);

	bool IsFinite() const;

	/// Gets a point along the line at the given distance.
	/** Use this function to convert a 1D parametric point along the Line to a 3D point in the linear space.
	@param distance The point to compute. GetPoint(0) will return pos. GetPoint(t) will return a point
	at distance |t| from pos, towards the direction specified by dir. If a negative value is specified,
	a point towards the direction -dir is returned.
	@return pos + distance * dir.
	@see pos, dir. */
	float3 GetPoint(float distance) const;

	/// Translates this Line in world space.
	/** @param offset The amount of displacement to apply to this Line, in world space coordinates.
	@see Transform(). */
	void Translate(const float3 &offset);

	/// Applies a transformation to this line, in-place.
	/** @see Translate(), classes float3x3, float3x4, float4x4, Quat. */
	void Transform(const float3x3 &transform);
	void Transform(const Quat &transform);

	/// Tests if the given object is fully contained on this line.
	/** @param distanceThreshold The magnitude of the epsilon test threshold to use. Since a Line
	is a 1D object in a 3D space, an epsilon threshold is used to allow errors caused by floating-point
	inaccuracies.
	@return True if this line contains the given object, up to the given distance threshold.
	@see class LineSegment, class Ray, Distance(), ClosestPoint(), Intersects(). */
	bool Contains(const float3 &point, float distanceThreshold = 1e-3f) const;
	bool Contains(const Ray &ray, float distanceThreshold = 1e-3f) const;
	bool Contains(const LineSegment &lineSegment, float distanceThreshold = 1e-3f) const;

	/// Tests if two lines are equal.
	/** This function tests for set equality (not just member value equality). This means that the pos and dir parameters
	of either line can be completely different, as long as the set of points on the both lines are equal.
	@return True if this and the given Line represent the same set of points, up to the given epsilon. */
	bool Equals(const Line &line, float epsilon = 1e-3f) const;

	/// Computes the distance between this line and the given object.
	/** This function finds the nearest pair of points on this and the given object, and computes their distance.
	If the two objects intersect, or one object is contained inside the other, the returned distance is zero.
	@param d [out] If specified, receives the parametric distance along this line that
	specifies the closest point on this line to the given object. The value returned here can be negative.
	This pointer may be null.
	@see Contains(), Intersects(), ClosestPoint(), GetPoint(). */
	float Distance(const float3 &point, float *d = 0) const;
	/** @param d2 [out] If specified, receives the parametric distance along the other line that specifies the
	closest point on that line to this line. The value returned here can be negative. This pointer may
	be null. */
	float Distance(const Ray &other, float *d, float *d2 = 0) const;
	float Distance(const Ray &other) const;
	float Distance(const Line &other, float *d, float *d2 = 0) const;
	float Distance(const Line &other) const;
	float Distance(const LineSegment &other, float *d, float *d2 = 0) const;
	float Distance(const LineSegment &other) const;
	float Distance(const Sphere &other) const;
	float Distance(const Capsule &other) const;

	/// Computes the closest point on this line to the given object.
	/** If the other object intersects this line, this function will return an arbitrary point inside
	the region of intersection.
	@param d [out] If specified, receives the parametric distance along this line that
	specifies the closest point on this line to the given object. The value returned here can be negative.
	This pointer may be null.
	@see Contains(), Distance(), Intersects(), GetPoint(). */
	float3 ClosestPoint(const float3 &targetPoint, float *d = 0) const;
	/** @param d2 [out] If specified, receives the parametric distance along the other line that specifies the
	closest point on that line to this line. The value returned here can be negative. This pointer may
	be null. */
	float3 ClosestPoint(const Ray &other, float *d = 0, float *d2 = 0) const;
	float3 ClosestPoint(const Line &other, float *d = 0, float *d2 = 0) const;
	float3 ClosestPoint(const LineSegment &other, float *d = 0, float *d2 = 0) const;
	/** @param outU [out] If specified, receives the barycentric U-coordinate (in two-coordinate barycentric UV convention)
	representing the closest point on the triangle to this line. This pointer may be null.
	@param outV [out] If specified, receives the barycentric V-coordinate (in two-coordinate barycentric UV convention)
	representing the closest point on the triangle to this line. This pointer may be null.
	@see Contains(), Distance(), Intersects(), GetPoint(), Triangle::Point(float u, float v). */
	float3 ClosestPoint(const Triangle &triangle, float *outU = 0, float *outV = 0, float *d = 0) const;

	/// Tests whether this line and the given object intersect.	
	/** Both objects are treated as "solid", meaning that if one of the objects is fully contained inside
	another, this function still returns true.
	@param d [out] If specified, this parameter will receive the parametric distance of
	the intersection point along this object. Use the GetPoint(d) function
	to get the actual point of intersection. This pointer may be null.
	@param intersectionPoint [out] If specified, receives the actual point of intersection. This pointer
	may be null.
	@return True if an intersection occurs or one of the objects is contained inside the other, false otherwise.
	@see Contains(), Distance(), ClosestPoint(), GetPoint(). */
	bool Intersects(const Triangle &triangle, float *d, float3 *intersectionPoint) const;
	bool Intersects(const Plane &plane, float *d) const;
	/** @param intersectionNormal [out] If specified, receives the surface normal of the other object at
	the point of intersection. This pointer may be null. */
	bool Intersects(const Sphere &s, float3 *intersectionPoint = 0, float3 *intersectionNormal = 0, float *d = 0) const;
	/** @param dNear [out] If specified, receives the distance along this line to where the line enters
	the bounding box.
	@param dFar [out] If specified, receives the distance along this line to where the line exits
	the bounding box. */
	bool Intersects(const AABB &aabb, float &dNear, float &dFar) const;
	bool Intersects(const AABB &aabb) const;
	bool Intersects(const OBB &obb, float &dNear, float &dFar) const;
	bool Intersects(const OBB &obb) const;
	bool Intersects(const Capsule &capsule) const;
	bool Intersects(const Polygon &polygon) const;
	bool Intersects(const Frustum &frustum) const;
	bool Intersects(const Polyhedron &polyhedron) const;
	/// Tests if this ray intersects the given disc.
	/// @todo This signature will be moved to bool Intersects(const Disc &disc) const;
	bool IntersectsDisc(const Circle &disc) const;

	/// Converts this Line to a Ray.
	/** The pos and dir members of the returned Ray will be equal to this Line. The only difference is
	that a Line extends to infinity in two directions, whereas the returned Ray spans only in
	the positive direction.
	@see dir, Line::Line, class Ray, ToLineSegment(). */
	Ray ToRay() const;

	/// Converts this Line to a LineSegment.
	/** @param d Specifies the position of the other endpoint along this Line. This parameter may be negative.
	@return A LineSegment with point a at pos, and point b at pos + d * dir.
	@see pos, dir, Line::Line, class LineSegment, ToRay(). */
	LineSegment ToLineSegment(float d) const;

	/// Converts this Line to a LineSegment.
	/** @param dStart Specifies the position of the first endpoint along this Line. This parameter may be negative,
	in which case the starting point lies to the opposite direction of the Line.
	@param dEnd Specifies the position of the second endpoint along this Line. This parameter may also be negative.
	@return A LineSegment with point a at pos + dStart * dir, and point b at pos + dEnd * dir.
	@see pos, dir, Line::Line, class LineSegment, ToLine(). */
	LineSegment ToLineSegment(float dStart, float dEnd) const;

	/// Projects this Line onto the given 1D axis direction vector.
	/** This function collapses this Line onto an 1D axis for the purposes of e.g. separate axis test computations.
	The function returns a 1D range [outMin, outMax] denoting the interval of the projection.
	@param direction The 1D axis to project to. This vector may be unnormalized, in which case the output
	of this function gets scaled by the length of this vector.
	@param outMin [out] Returns the minimum extent of this object along the projection axis.
	@param outMax [out] Returns the maximum extent of this object along the projection axis. */
	void ProjectToAxis(const float3 &direction, float &outMin, float &outMax) const;

	/// Tests if the given three points are collinear.
	/** This function tests whether the given three functions all lie on the same line.
	@param epsilon The comparison threshold to use to account for floating-point inaccuracies. */
	static bool AreCollinear(const float3 &p1, const float3 &p2, const float3 &p3, float epsilon = 1e-3f);

	static float3 ClosestPointLineLine(float3 start0, float3 end0, float3 start1, float3 end1, float *d, float *d2);
};

class LineSegment
{
public:
	/// The starting point of this line segment.
	float3 a;
	/// The end point of this line segment. [similarOverload: a]
	float3 b;

	/// The default constructor does not initialize any members of this class.
	/** This means that the values of the members a and b are undefined after creating a new LineSegment using this
	default constructor. Remember to assign to them before use.
	@see a, b. */
	LineSegment() {}

	/// Constructs a line segment through the given end points.
	/** @see a, b. */
	LineSegment(const float3 &a, const float3 &b);

	/// Constructs a line segment from a ray or a line.
	/** This constructor takes the ray/line origin position as the starting point of this line segment, and defines the end point
	of the line segment using the given distance parameter.
	@param d The distance along the ray/line for the end point of this line segment. This will set b = ray.pos + d * ray.dir
	as the end point. When converting a ray to a line segment, it is possible to pass in a d value < 0, but in that case
	the resulting line segment will not lie on the ray.
	@see a, b, classes Ray, Line, Line::GetPoint(), Ray::GetPoint(). */
	explicit LineSegment(const Ray &ray, float d);
	explicit LineSegment(const Line &line, float d);

	/// Returns a point on the line.
	/** @param d The normalized distance along the line segment to compute. If a value in the range [0, 1] is passed, then the
	returned point lies along this line segment. If some other value is specified, the returned point lies on the
	line defined by this line segment, but not inside the interval from a to b.
	@note The meaning of d here differs from Line::GetPoint and Ray::GetPoint. For the class LineSegment,
	GetPoint(0) returns a, and GetPoint(1) returns b. This means that GetPoint(1) will not generally be exactly one unit
	away from the starting point of this line segment, as is the case with Line and Ray.
	@return (1-d)*a + d*b.
	@see a, b, Line::GetPoint(), Ray::GetPoint(). */
	float3 GetPoint(float d) const;

	/// Returns the center point of this line segment.
	/** This function is the same as calling GetPoint(0.5f), but provided here as conveniency.
	@see GetPoint(). */
	float3 CenterPoint() const;

	/// Reverses the direction of this line segment.
	/** This function swaps the start and end points of this line segment so that it runs from b to a.
	This does not have an effect on the set of points represented by this line segment, but it reverses
	the direction of the vector returned by Dir().
	@note This function operates in-place.
	@see a, b, Dir(). */
	void Reverse();

	/// Returns the normalized direction vector that points in the direction a->b.
	/** @note The returned vector is normalized, meaning that its length is 1, not |b-a|.
	@see a, b. */
	float3 Dir() const;

	/// Computes an extreme point of this LineSegment in the given direction.
	/** An extreme point is a farthest point along this LineSegment in the given direction. Given a direction,
	this point is not necessarily unique.
	@param direction The direction vector of the direction to find the extreme point. This vector may
	be unnormalized, but may not be null.
	@return An extreme point of this LineSegment in the given direction. The returned point is always
	either a or b.
	@see a, b.*/
	float3 ExtremePoint(const float3 &direction) const;

	/// Translates this LineSegment in world space.
	/** @param offset The amount of displacement to apply to this LineSegment, in world space coordinates.
	@see Transform(). */
	void Translate(const float3 &offset);

	/// Applies a transformation to this line.
	/** This function operates in-place.
	@see Translate(), classes float3x3, float3x4, float4x4, Quat, Transform(). */
	void Transform(const float3x3 &transform);
	void Transform(const Quat &transform);

	/// Computes the length of this line segment.
	/** @return |b-a|.
	@see a, b. */
	float Length() const;
	/// Computes the squared length of this line segment.
	/** Calling this function is faster than calling Length(), since this function avoids computing a square root.
	If you only need to compare lengths to each other and are not interested in the actual length values,
	you can compare by using LengthSq(), instead of Length(), since Sqrt() is an order-preserving
	(monotonous and non-decreasing) function. [similarOverload: Length] */
	float LengthSq() const;

	/// Tests if this line segment is finite.
	/** A line segment is <b><i>finite</i></b> if its endpoints a and b do not contain floating-point NaNs or +/-infs
	in them.
	@return True if both a and b have finite floating-point values. */
	bool IsFinite() const;

	/// Tests if this line segment represents the same set of points than the given line segment.
	/** @param distanceThreshold Specifies how much distance threshold to allow in the comparison.
	@return True if a == rhs.a && b == rhs.b, or, a == rhs.b && b = rhs.a, within the given epsilon. */
	bool Equals(const LineSegment &rhs, float distanceThreshold = 1e-3f) const;

	/// Tests if the given point or line segment is contained on this line segment.
	/** @param distanceThreshold Because a line segment is an one-dimensional object in 3D space, an epsilon value
	is used as a threshold for this test. This effectively transforms this line segment to a capsule with
	the radius indicated by this value.
	@return True if this line segment contains the given point or line segment.
	@see Intersects, ClosestPoint(), Distance(). */
	bool Contains(const float3 &point, float distanceThreshold = 1e-3f) const;
	bool Contains(const LineSegment &lineSegment, float distanceThreshold = 1e-3f) const;

	/// Computes the closest point on this line segment to the given object.
	/** @param d [out] If specified, this parameter receives the normalized distance along
	this line segment which specifies the closest point on this line segment to
	the specified point. This pointer may be null.
	@return The closest point on this line segment to the given object.
	@see Contains(), Distance(), Intersects(). */
	float3 ClosestPoint(const float3 &point, float *d = 0) const;
	/** @param d2 [out] If specified, this parameter receives the (normalized, in case of line segment)
	distance along the other line object which specifies the closest point on that line to
	this line segment. This pointer may be null. */
	float3 ClosestPoint(const Ray &other, float *d = 0, float *d2 = 0) const;
	float3 ClosestPoint(const Line &other, float *d = 0, float *d2 = 0) const;
	float3 ClosestPoint(const LineSegment &other, float *d = 0, float *d2 = 0) const;

	/// Computes the distance between this line segment and the given object.
	/** @param d [out] If specified, this parameter receives the normalized distance along
	this line segment which specifies the closest point on this line segment to
	the specified point. This pointer may be null.
	@return The distance between this line segment and the given object.
	@see Constains(), ClosestPoint(), Intersects(). */
	float Distance(const float3 &point, float *d = 0) const;
	/** @param d2 [out] If specified, this parameter receives the (normalized, in case of line segment)
	distance along the other line object which specifies the closest point on that line to
	this line segment. This pointer may be null. */
	float Distance(const Ray &other, float *d = 0, float *d2 = 0) const;
	float Distance(const Line &other, float *d = 0, float *d2 = 0) const;
	float Distance(const LineSegment &other, float *d = 0, float *d2 = 0) const;
	float Distance(const Plane &other) const;
	float Distance(const Sphere &other) const;
	float Distance(const Capsule &other) const;

	/// Tests whether this line segment and the given object intersect.	
	/** Both objects are treated as "solid", meaning that if one of the objects is fully contained inside
	another, this function still returns true. (for example, if this line segment is contained inside a sphere)
	@todo Output intersection point. */
	bool Intersects(const Plane &plane) const;
	/** @param d [out] If specified, this parameter receives the normalized distance along
	this line segment which specifies the closest point on this line segment to
	the specified point. This pointer may be null. */
	bool Intersects(const Plane &plane, float *d) const;
	/** @param intersectionPoint [out] If specified, receives the point of intersection. This pointer may be null. */
	bool Intersects(const Triangle &triangle, float *d, float3 *intersectionPoint) const;
	/** @param intersectionNormal [out] If specified, receives the normal vector of the other object at the point of intersection.
	This pointer may be null. */
	bool Intersects(const Sphere &s, float3 *intersectionPoint = 0, float3 *intersectionNormal = 0, float *d = 0) const;
	/** @param dNear [out] If specified, receives the parametric distance along this line segment denoting where the line entered the
	bounding box object.
	@param dFar [out] If specified, receives the parametric distance along this line segment denoting where the line exited the
	bounding box object. */
	bool Intersects(const AABB &aabb, float &dNear, float &dFar) const;
	bool Intersects(const AABB &aabb) const;
	bool Intersects(const OBB &obb, float &dNear, float &dFar) const;
	bool Intersects(const OBB &obb) const;
	bool Intersects(const Capsule &capsule) const;
	bool Intersects(const Polygon &polygon) const;
	bool Intersects(const Frustum &frustum) const;
	bool Intersects(const Polyhedron &polyhedron) const;
	/** @param epsilon If testing intersection between two line segments, a distance threshold value is used to account
	for floating-point inaccuracies. */
	bool Intersects(const LineSegment &lineSegment, float epsilon = 1e-3f) const;
	/// Tests if this line segment intersects the given disc.
	/// @todo This signature will be moved to bool Intersects(const Disc &disc) const;
	bool IntersectsDisc(const Circle &disc) const;

	/// Converts this LineSegment to a Ray.
	/** The pos member of the returned Ray will be equal to a, and the dir member equal to Dir().
	@see class Ray, ToLine(). */
	Ray ToRay() const;
	/// Converts this LineSegment to a Line.
	/** The pos member of the returned Line will be equal to a, and the dir member equal to Dir().
	@see class Line, ToRay(). */
	Line ToLine() const;

	/// Projects this LineSegment onto the given 1D axis direction vector.
	/** This function collapses this LineSegment onto an 1D axis for the purposes of e.g. separate axis test computations.
	The function returns a 1D range [outMin, outMax] denoting the interval of the projection.
	@param direction The 1D axis to project to. This vector may be unnormalized, in which case the output
	of this function gets scaled by the length of this vector.
	@param outMin [out] Returns the minimum extent of this object along the projection axis.
	@param outMax [out] Returns the maximum extent of this object along the projection axis. */
	void ProjectToAxis(const float3 &direction, float &outMin, float &outMax) const;
};

class Polyhedron
{
public:
	/// Stores a list of indices of a single face of a Polyhedron.
	struct Face
	{
		/// Specifies the indices of the corner vertices of this polyhedron.
		/// Indices point to the polyhedron vertex array.
		/// The face vertices should all lie on the same plane.
		/// The positive direction of the plane (the direction the face outwards normal points to)
		/// is the one where the vertices are wound in counter-clockwise order.
		std::vector<int> v;

		/// Reverses the winding order of this face. This has the effect of reversing the direction
		/// the normal of this face points to.
		void FlipWindingOrder();

		std::string ToString() const;
	};

	/// Specifies the vertices of this polyhedron.
	std::vector<float3> v;

	/// Specifies the individual faces of this polyhedron.  [similarOverload: v]
	/** Each face is described by a list of indices to the vertex array. The indices define a
	simple polygon in counter-clockwise winding order. */
	std::vector<Face> f;

	/// The default constructor creates a null polyhedron.
	/** A null polyhedron has 0 vertices and 0 faces.
	@see IsNull(). */
	Polyhedron() {}

	/// Returns the number of vertices in this polyhedron.
	/** The running time of this function is O(1).
	@see NumFaces(), NumEdges(), EulerFormulaHolds(). */
	int NumVertices() const { return (int)v.size(); }

	/// Returns the number of faces in this polyhedron.
	/** The running time of this function is O(1).
	@see NumVertices(), NumEdges(), EulerFormulaHolds(), FacePolygon(), FacePlane(). */
	int NumFaces() const { return (int)f.size(); }

	/// Returns the number of (unique) edges in this polyhedron.
	/** This function will enumerate through all faces of this polyhedron to compute the number of unique edges.
	The running time is linear to the number of faces and vertices in this Polyhedron.
	@see NumVertices(), NumFaces(), EulerFormulaHolds(), Edge(), Edges(), EdgeIndices(). */
	int NumEdges() const;

	/// Returns a pointer to an array of vertices of this polyhedron. The array contains NumVertices() elements.
	/// @note Do NOT hold on to this pointer, since it is an alias to the underlying std::vector owned by this polyhedron. Calling any non-const Polyhedron member function may invalidate the pointer!
	float3 *VertexArrayPtr() { return !v.empty() ? &v[0] : 0; }
	const float3 *VertexArrayPtr() const { return !v.empty() ? &v[0] : 0; }

	/// Returns the <i>i</i>th vertex of this polyhedron.
	/** @param vertexIndex The vertex to get, in the range [0, NumVertices()-1].
	@see NumVertices(). */
	float3 Vertex(int vertexIndex) const;

	/// Returns the <i>i</i>th edge of this polyhedron.
	/** Performance warning: Use this function only if you are interested in a single edge of this Polyhedron.
	This function calls the Edges() member function to receive a list of all the edges, so has
	a complexity of O(|V|log|V|), where |V| is the number of vertices in the polyhedron.
	@param edgeIndex The index of the edge to get, in the range [0, NumEdges()-1].
	@see NumEdges(), Edges(), EdgeIndices(). */
	LineSegment Edge(int edgeIndex) const;

	/// Returns all the (unique) edges of this polyhedron.
	/** Has complexity of O(|V|log|V|), where |V| is the number of vertices in the polyhedron.
	@todo Support this in linear time.
	@see NumEdges(), Edge(), EdgeIndices(). */
	std::vector<LineSegment> Edges() const;

	std::vector<Polygon> Faces() const;

	/// Returns all the (unique) edges of this polyhedron, as indices to the polyhedron vertex array.
	/** Has complexity of O(|V|log|V|), where |V| is the number of vertices in the polyhedron.
	@todo Support this in linear time.
	@see NumEdges(), Edge(), Edges(). */
	std::vector<std::pair<int, int> > EdgeIndices() const;

	/// Returns a polygon representing the given face.
	/** The winding order of the polygon will be the same as in the input. The normal of the polygon
	points outwards from this polyhedron, i.e. towards the space that is not part of the polyhedron.
	This function constructs a new Polygon object, so it has a time and space complexity of
	O(|V|), where |V| is the number of vertices in this polyhedron.
	@param faceIndex The index of the face to get, in the range [0, NumFaces()-1].
	@see NumFaces(), FacePlane(). */
	Polygon FacePolygon(int faceIndex) const;

	/// Returns the plane of the given polyhedron face.
	/** The normal of the plane points outwards from this polyhedron, i.e. towards the space that
	is not part of the polyhedron.
	This function assumes that the given face of the polyhedron is planar, as should be for all
	well-formed polyhedron.
	@param faceIndex The index of the face to get, in the range [0, NumFaces()-1].
	@see NumFaces(), FacePolygon(). */
	Plane FacePlane(int faceIndex) const;

	float3 FaceNormal(int faceIndex) const;

	/// Returns the index of the vertex of this polyhedron that reaches farthest in the given direction.
	/** @param direction The direction vector to query for. This vector can be unnormalized.
	@return The supporting point of this polyhedron that reaches farthest in the given direction.
	The supporting point for a given direction is not necessarily unique, but this function
	will always return one of the vertices of this polyhedron.
	@see v, NumVertices(), Vertex(). */
	int ExtremeVertex(const float3 &direction) const;
	// float3 SupportingPoint(const float3 &dir) const;
	// bool IsSupportingPlane(const Plane &plane) const;

	/// Computes an extreme point of this Polyhedron in the given direction.
	/** An extreme point is a farthest point of this Polyhedron in the given direction. Given a direction,
	this point is not necessarily unique.
	@param direction The direction vector of the direction to find the extreme point. This vector may
	be unnormalized, but may not be null.
	@return An extreme point of this Polyhedron in the given direction. The returned point is always a
	corner point of this Polyhedron.
	@see CornerPoint(). */
	float3 ExtremePoint(const float3 &direction) const;

	/// Projects this Polyhedron onto the given 1D axis direction vector.
	/** This function collapses this Polyhedron onto an 1D axis for the purposes of e.g. separate axis test computations.
	The function returns a 1D range [outMin, outMax] denoting the interval of the projection.
	@param direction The 1D axis to project to. This vector may be unnormalized, in which case the output
	of this function gets scaled by the length of this vector.
	@param outMin [out] Returns the minimum extent of this object along the projection axis.
	@param outMax [out] Returns the maximum extent of this object along the projection axis. */
	void ProjectToAxis(const float3 &direction, float &outMin, float &outMax) const;

	/// Returns the arithmetic mean of all the corner vertices.
	/** @bug This is not the proper centroid of the polyhedron! */
	/** @see SurfaceArea(), Volume(). */
	float3 Centroid() const;

	/// Computes the total surface area of the faces of this polyhedron.
	/** @see Centroid(), Volume(). */
	float SurfaceArea() const;

	/// Computes the internal volume of this polyhedron.
	/** @see Centroid(), SurfaceArea(). */
	float Volume() const;

	/// Returns the smallest AABB that encloses this polyhedron.
	/// @todo Add MinimalEnclosingSphere() and MinimalEnclosingOBB().
	AABB MinimalEnclosingAABB() const;

	void MergeAdjacentPlanarFaces();

	/// Tests if the faces in this polyhedron refer to valid existing vertices.
	/** This function performs sanity checks on the face indices array.
	1) Each vertex index for each face must be in the range [0, NumVertices()-1], i.e. refer to a vertex
	that exists in the array.
	2) Each face must contain at least three vertices. If a face contains two or one vertex, it is
	degenerate.
	3) Each face may refer to a vertex at most once.
	@return True if the abovementioned conditions hold. Note that this does not guarantee that the
	polyhedron is completely well-formed, but if false is returned, the polyhedron is definitely
	ill-formed.
	@see IsNull(), IsClosed(), IsConvex(). */
	bool FaceIndicesValid() const;

	/// Flips the winding order of all faces in this polyhedron.
	void FlipWindingOrder();

	/// Assuming that this polyhedron is convex, reorients all faces of this polyhedron
	/// so that each face plane has its normal pointing outwards. That is, the "negative" side of the
	/// polyhedron lies inside the polyhedron, and the positive side of the polyhedron is outside the convex
	/// shape.
	void OrientNormalsOutsideConvex();

	/// Removes from the vertex array all vertices that are not referred to by any of the faces of this polyhedron.
	void RemoveRedundantVertices();

	/// Returns true if this polyhedron has 0 vertices and 0 faces.
	/** @see FaceIndicesValid(), IsClosed(), IsConvex(). */
	bool IsNull() const { return v.empty() && f.empty(); }

	/// Returns true if this polyhedron is closed and does not have any gaps.
	/** \note This function performs a quick check, which might not be complete.
	@see FaceIndicesValid(), IsClosed(), IsConvex(). */
	bool IsClosed() const;

	// Returns true if this polyhedron forms a single connected solid volume.
	//	bool IsConnected() const;

	/// Returns true if this polyhedron is convex.
	/** The running time is O(F*V) ~ O(V^2).
	@see FaceIndicesValid(), IsClosed(), IsConvex().*/
	bool IsConvex() const;

	/// Returns true if the Euler formula (V + F - E == 2) holds for this Polyhedron.
	/** @see NumVertices(), NumEdges(), NumFaces(). */
	bool EulerFormulaHolds() const;

	/// Tests whether all the faces of this polyhedron are non-degenerate (have at least 3 vertices)
	/// and in case they have more than 3 vertices, tests that the faces are planar.
	bool FacesAreNondegeneratePlanar(float epsilon = 1e-2f) const;

	/// Clips the line/ray/line segment specified by L(t) = ptA + t * dir, tFirst <= t <= tLast,
	/// inside this <b>convex</b> polyhedron.
	/** The implementation of this function is adapted from Christer Ericson's Real-time Collision Detection, p. 199.
	@param ptA The first endpoint of the line segment.
	@param dir The direction of the line segment. This member can be unnormalized.
	@param tFirst [in, out] As input, takes the parametric distance along the line segment which
	specifies the starting point of the line segment. As output, the starting point of the line segment
	after the clipping operation is returned here.
	@param tLast [in, out] As input, takes the parametric distance along the line segment which
	specifies the ending point of the line segment. As output, the endingpoint of the line segment
	after the clipping operation is returned here.
	@note To clip a line, pass in tFirst=-FLOAT_INF, tLast=FLOAT_INF. To clip a ray, pass in tFirst=0 and tLast = FLOAT_INF.
	To clip a line segment, pass in tFirst=0, tLast=1, and an unnormalized dir = lineSegment.b-lineSegment.a.
	@return True if the outputted range [tFirst, tLast] did not become degenerate, and these two variables contain
	valid data. If false, the whole line segment was clipped away (it was completely outside this polyhedron).
	@see FLOAT_INF. */
	bool ClipLineSegmentToConvexPolyhedron(const float3 &ptA, const float3 &dir, float &tFirst, float &tLast) const;

	/// Tests if the given object is fully contained inside this polyhedron.
	/** This function treats this polyhedron as a non-convex object. If you know this polyhedron
	to be convex, you can use the faster ContainsConvex() function.
	@see ContainsConvex(), ClosestPoint(), ClosestPointConvex(), Distance(), Intersects(), IntersectsConvex().
	@todo Add Contains(Circle/Disc/Sphere/Capsule). */
	bool Contains(const float3 &point) const;
	bool Contains(const LineSegment &lineSegment) const;
	bool Contains(const Triangle &triangle) const;
	bool Contains(const Polygon &polygon) const;
	bool Contains(const AABB &aabb) const;
	bool Contains(const OBB &obb) const;
	bool Contains(const Frustum &frustum) const;
	bool Contains(const Polyhedron &polyhedron) const;

	/// Tests if the given face of this Polyhedron contains the given point.
	bool FaceContains(int faceIndex, const float3 &worldSpacePoint, float polygonThickness = 1e-3f) const;

	/// Tests if the given object is fully contained inside this <b>convex</b> polyhedron.
	/** This function behaves exactly like Contains(), except this version of the containment test
	assumes this polyhedron is convex, and uses a faster method of testing containment.
	@see Contains(), ClosestPoint(), ClosestPointConvex(), Distance(), Intersects(), IntersectsConvex().
	@todo Add ContainsConvex(Polygon/AABB/OBB/Frustum/Polyhedron/Circle/Disc/Sphere/Capsule). */
	bool ContainsConvex(const float3 &point) const;
	bool ContainsConvex(const LineSegment &lineSegment) const;
	bool ContainsConvex(const Triangle &triangle) const;

	/// Computes the closest point on this polyhedron to the given object.
	/** If the other object intersects this polyhedron, this function will return an arbitrary point inside
	the region of intersection.
	@param lineSegment The line segment to find the closest point to.
	@param lineSegmentPt [out] If specified, returns the closest point on the line segment to this
	polyhedron. This pointer may be null.
	@todo Make lineSegmentPt an out-reference instead of an out-pointer.
	@see Contains(), ContainsConvex(), ClosestPointConvex(), Distance(), Intersects(), IntersectsConvex().
	@todo Add ClosestPoint(Line/Ray/Plane/Triangle/Polygon/Circle/Disc/AABB/OBB/Sphere/Capsule/Frustum/Polyhedron). */
	float3 ClosestPoint(const LineSegment &lineSegment, float3 *lineSegmentPt) const;
	float3 ClosestPoint(const LineSegment &lineSegment) const;
	/** @param point The point to find the closest point to. */
	float3 ClosestPoint(const float3 &point) const;

	/// Returns the closest point on this <b>convex</b> polyhedron to the given point.
	/** This function behaves exactly like ClosestPoint(), except this version of the test assumes
	this polyhedron is convex, and uses a faster method of finding the closest point.
	@see Contains(), ContainsConvex(), ClosestPoint(), Distance(), Intersects(), IntersectsConvex().
	@todo Add ClosestPointConvex(Line/LineSegment/Ray/Plane/Triangle/Polygon/Circle/Disc/AABB/OBB/Sphere/Capsule/Frustum/Polyhedron). */
	float3 ClosestPointConvex(const float3 &point) const;

	/// Returns the distance between this polyhedron and the given object.
	/** This function finds the nearest pair of points on this and the given object, and computes their distance.
	If the two objects intersect, or one object is contained inside the other, the returned distance is zero.
	@see Contains(), ContainsConvex(), ClosestPoint(), ClosestPointConvex(), Intersects(), IntersectsConvex().
	@todo Add Distance(Line/LineSegment/Ray/Plane/Triangle/Polygon/Circle/Disc/AABB/OBB/Sphere/Capsule/Frustum/Polyhedron). */
	float Distance(const float3 &point) const;

	/// Tests whether this polyhedron and the given object intersect.
	/** Both objects are treated as "solid", meaning that if one of the objects is fully contained inside
	another, this function still returns true. (e.g. in case a line segment is contained inside this polyhedron,
	or this polyhedron is contained inside a sphere, etc.)
	@return True if an intersection occurs or one of the objects is contained inside the other, false otherwise.
	@see Contains(), ContainsConvex(), ClosestPoint(), ClosestPointConvex(), Distance(), IntersectsConvex().
	@todo Add Intersects(Circle/Disc). */
	bool Intersects(const LineSegment &lineSegment) const;
	bool Intersects(const Line &line) const;
	bool Intersects(const Ray &ray) const;
	bool Intersects(const Plane &plane) const;
	bool Intersects(const Polyhedron &polyhedron) const;
	bool Intersects(const AABB &aabb) const;
	bool Intersects(const OBB &obb) const;
	bool Intersects(const Triangle &triangle) const;
	bool Intersects(const Polygon &polygon) const;
	bool Intersects(const Frustum &frustum) const;
	bool Intersects(const Sphere &sphere) const;
	bool Intersects(const Capsule &capsule) const;

	/// Tests whether this <b>convex</b> polyhedron and the given object intersect.
	/** This function is exactly like Intersects(), but this version assumes that this polyhedron is convex,
	and uses a faster method of testing the intersection.
	@return True if an intersection occurs or one of the objects is contained inside the other, false otherwise.
	@see Contains(), ContainsConvex(), ClosestPoint(), ClosestPointConvex(), Distance(), Intersects().
	@todo Add Intersects(Circle/Disc). */
	bool IntersectsConvex(const Line &line) const;
	bool IntersectsConvex(const Ray &ray) const;
	bool IntersectsConvex(const LineSegment &lineSegment) const;

	void MergeConvex(const float3 &point);

	/// Translates this Polyhedron in world space.
	/** @param offset The amount of displacement to apply to this Polyhedron, in world space coordinates.
	@see Transform(). */
	void Translate(const float3 &offset);

	/// Applies a transformation to this Polyhedron.
	/** This function operates in-place.
	@see Translate(), classes float3x3, float3x4, float4x4, Quat. */
	void Transform(const float3x3 &transform);
	void Transform(const Quat &transform);

	/// Creates a Polyhedron object that represents the convex hull of the given point array.
	/// \todo This function is strongly WIP!
	static Polyhedron ConvexHull(const float3 *pointArray, int numPoints);

	static Polyhedron Tetrahedron(const float3 &centerPos = float3(0, 0, 0), float scale = 1.f, bool ccwIsFrontFacing = true);
	static Polyhedron Octahedron(const float3 &centerPos = float3(0, 0, 0), float scale = 1.f, bool ccwIsFrontFacing = true);
	static Polyhedron Hexahedron(const float3 &centerPos = float3(0, 0, 0), float scale = 1.f, bool ccwIsFrontFacing = true);
	static Polyhedron Icosahedron(const float3 &centerPos = float3(0, 0, 0), float scale = 1.f, bool ccwIsFrontFacing = true);
	static Polyhedron Dodecahedron(const float3 &centerPos = float3(0, 0, 0), float scale = 1.f, bool ccwIsFrontFacing = true);

	std::vector<Triangle> Triangulate() const;

	std::string ToString() const;
};

class ALIGN16 ScaleOp
{
public:
	/// The scale factor along the x axis.
	float x;
	/// The scale factor along the y axis.
	float y;
	/// The scale factor along the z axis.
	float z;

	float padding;

	ScaleOp(const float3 &scale);

	float3x4 ToFloat3x4() const;
};

class ALIGN16 TranslateOp
{
public:
	/// The x offset of translation.
	float x;
	/// The y offset of translation.
	float y;
	/// The z offset of translation.
	float z;

	float padding;

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

float3 operator *(float scalar, const float3 &rhs);
Line operator *(const float3x4 &transform, const Line &line);
inline float Dot(const float3 &a, const float3 &b) { return a.Dot(b); }
inline float3 Abs(const float3 &a) { return a.Abs(); }
inline float3 Cross(const float3 &a, const float3 &b) { return a.Cross(b); }
#define DOT3(v1, v2) ((v1)[0] * (v2)[0] + (v1)[1] * (v2)[1] + (v1)[2] * (v2)[2])
#define assume(x) if(!x) throw std::exception();
}