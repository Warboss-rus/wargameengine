#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Algorithm/Random/LCG.o \
	${OBJECTDIR}/Geometry/AABB.o \
	${OBJECTDIR}/Geometry/Capsule.o \
	${OBJECTDIR}/Geometry/Circle.o \
	${OBJECTDIR}/Geometry/Frustum.o \
	${OBJECTDIR}/Geometry/Line.o \
	${OBJECTDIR}/Geometry/LineSegment.o \
	${OBJECTDIR}/Geometry/OBB.o \
	${OBJECTDIR}/Geometry/PBVolume.o \
	${OBJECTDIR}/Geometry/Plane.o \
	${OBJECTDIR}/Geometry/Polygon.o \
	${OBJECTDIR}/Geometry/Polyhedron.o \
	${OBJECTDIR}/Geometry/Ray.o \
	${OBJECTDIR}/Geometry/Sphere.o \
	${OBJECTDIR}/Geometry/Triangle.o \
	${OBJECTDIR}/Geometry/TriangleMesh.o \
	${OBJECTDIR}/Math/BitOps.o \
	${OBJECTDIR}/Math/MathFunc.o \
	${OBJECTDIR}/Math/MathLog.o \
	${OBJECTDIR}/Math/MathOps.o \
	${OBJECTDIR}/Math/Polynomial.o \
	${OBJECTDIR}/Math/Quat.o \
	${OBJECTDIR}/Math/SSEMath.o \
	${OBJECTDIR}/Math/TransformOps.o \
	${OBJECTDIR}/Math/float2.o \
	${OBJECTDIR}/Math/float3.o \
	${OBJECTDIR}/Math/float3x3.o \
	${OBJECTDIR}/Math/float3x4.o \
	${OBJECTDIR}/Math/float4.o \
	${OBJECTDIR}/Math/float4x4.o \
	${OBJECTDIR}/Time/Clock.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmathlib.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmathlib.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmathlib.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmathlib.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmathlib.a

${OBJECTDIR}/Algorithm/Random/LCG.o: Algorithm/Random/LCG.cpp 
	${MKDIR} -p ${OBJECTDIR}/Algorithm/Random
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Algorithm/Random/LCG.o Algorithm/Random/LCG.cpp

${OBJECTDIR}/Geometry/AABB.o: Geometry/AABB.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/AABB.o Geometry/AABB.cpp

${OBJECTDIR}/Geometry/Capsule.o: Geometry/Capsule.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/Capsule.o Geometry/Capsule.cpp

${OBJECTDIR}/Geometry/Circle.o: Geometry/Circle.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/Circle.o Geometry/Circle.cpp

${OBJECTDIR}/Geometry/Frustum.o: Geometry/Frustum.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/Frustum.o Geometry/Frustum.cpp

${OBJECTDIR}/Geometry/Line.o: Geometry/Line.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/Line.o Geometry/Line.cpp

${OBJECTDIR}/Geometry/LineSegment.o: Geometry/LineSegment.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/LineSegment.o Geometry/LineSegment.cpp

${OBJECTDIR}/Geometry/OBB.o: Geometry/OBB.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/OBB.o Geometry/OBB.cpp

${OBJECTDIR}/Geometry/PBVolume.o: Geometry/PBVolume.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/PBVolume.o Geometry/PBVolume.cpp

${OBJECTDIR}/Geometry/Plane.o: Geometry/Plane.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/Plane.o Geometry/Plane.cpp

${OBJECTDIR}/Geometry/Polygon.o: Geometry/Polygon.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/Polygon.o Geometry/Polygon.cpp

${OBJECTDIR}/Geometry/Polyhedron.o: Geometry/Polyhedron.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/Polyhedron.o Geometry/Polyhedron.cpp

${OBJECTDIR}/Geometry/Ray.o: Geometry/Ray.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/Ray.o Geometry/Ray.cpp

${OBJECTDIR}/Geometry/Sphere.o: Geometry/Sphere.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/Sphere.o Geometry/Sphere.cpp

${OBJECTDIR}/Geometry/Triangle.o: Geometry/Triangle.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/Triangle.o Geometry/Triangle.cpp

${OBJECTDIR}/Geometry/TriangleMesh.o: Geometry/TriangleMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/Geometry
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Geometry/TriangleMesh.o Geometry/TriangleMesh.cpp

${OBJECTDIR}/Math/BitOps.o: Math/BitOps.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/BitOps.o Math/BitOps.cpp

${OBJECTDIR}/Math/MathFunc.o: Math/MathFunc.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/MathFunc.o Math/MathFunc.cpp

${OBJECTDIR}/Math/MathLog.o: Math/MathLog.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/MathLog.o Math/MathLog.cpp

${OBJECTDIR}/Math/MathOps.o: Math/MathOps.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/MathOps.o Math/MathOps.cpp

${OBJECTDIR}/Math/Polynomial.o: Math/Polynomial.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/Polynomial.o Math/Polynomial.cpp

${OBJECTDIR}/Math/Quat.o: Math/Quat.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/Quat.o Math/Quat.cpp

${OBJECTDIR}/Math/SSEMath.o: Math/SSEMath.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/SSEMath.o Math/SSEMath.cpp

${OBJECTDIR}/Math/TransformOps.o: Math/TransformOps.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/TransformOps.o Math/TransformOps.cpp

${OBJECTDIR}/Math/float2.o: Math/float2.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/float2.o Math/float2.cpp

${OBJECTDIR}/Math/float3.o: Math/float3.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/float3.o Math/float3.cpp

${OBJECTDIR}/Math/float3x3.o: Math/float3x3.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/float3x3.o Math/float3x3.cpp

${OBJECTDIR}/Math/float3x4.o: Math/float3x4.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/float3x4.o Math/float3x4.cpp

${OBJECTDIR}/Math/float4.o: Math/float4.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/float4.o Math/float4.cpp

${OBJECTDIR}/Math/float4x4.o: Math/float4x4.cpp 
	${MKDIR} -p ${OBJECTDIR}/Math
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Math/float4x4.o Math/float4x4.cpp

${OBJECTDIR}/Time/Clock.o: Time/Clock.cpp 
	${MKDIR} -p ${OBJECTDIR}/Time
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Time/Clock.o Time/Clock.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmathlib.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
