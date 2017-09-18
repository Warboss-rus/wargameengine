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
	${OBJECTDIR}/LogWriter.o \
	${OBJECTDIR}/Module.o \
	${OBJECTDIR}/NetSocket.o \
	${OBJECTDIR}/Network.o \
	${OBJECTDIR}/OSSpecific.o \
	${OBJECTDIR}/Plugin.o \
	${OBJECTDIR}/PluginManager.o \
	${OBJECTDIR}/Ruler.o \
	${OBJECTDIR}/SoundPlayer.o \
	${OBJECTDIR}/ThreadPool.o \
	${OBJECTDIR}/TranslationManager.o \
	${OBJECTDIR}/UI/UIButton.o \
	${OBJECTDIR}/UI/UICheckBox.o \
	${OBJECTDIR}/UI/UIComboBox.o \
	${OBJECTDIR}/UI/UIEdit.o \
	${OBJECTDIR}/UI/UIElement.o \
	${OBJECTDIR}/UI/UIList.o \
	${OBJECTDIR}/UI/UIPanel.o \
	${OBJECTDIR}/UI/UIRadioGroup.o \
	${OBJECTDIR}/UI/UIScrollBar.o \
	${OBJECTDIR}/UI/UIStaticText.o \
	${OBJECTDIR}/UI/UIText.o \
	${OBJECTDIR}/UI/UITheme.o \
	${OBJECTDIR}/controller/CommandChangeGlobalProperty.o \
	${OBJECTDIR}/controller/CommandChangeProperty.o \
	${OBJECTDIR}/controller/CommandCompound.o \
	${OBJECTDIR}/controller/CommandCreateObject.o \
	${OBJECTDIR}/controller/CommandDeleteObject.o \
	${OBJECTDIR}/controller/CommandHandler.o \
	${OBJECTDIR}/controller/CommandMoveObject.o \
	${OBJECTDIR}/controller/CommandRotateObject.o \
	${OBJECTDIR}/controller/GameController.o \
	${OBJECTDIR}/controller/LUARegisterFunctions.o \
	${OBJECTDIR}/controller/LUARegisterObject.o \
	${OBJECTDIR}/controller/LUARegisterUI.o \
	${OBJECTDIR}/controller/LUAScriptHandler.o \
	${OBJECTDIR}/controller/TimedCallback.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/model/Bounding.o \
	${OBJECTDIR}/model/GameModel.o \
	${OBJECTDIR}/model/Landscape.o \
	${OBJECTDIR}/model/MovementLimiter.o \
	${OBJECTDIR}/model/Object.o \
	${OBJECTDIR}/model/ObjectGroup.o \
	${OBJECTDIR}/model/Projectile.o \
	${OBJECTDIR}/nv_dds.o \
	${OBJECTDIR}/tinyxml.o \
	${OBJECTDIR}/tinyxmlerror.o \
	${OBJECTDIR}/tinyxmlparser.o \
	${OBJECTDIR}/view/3dModel.o \
	${OBJECTDIR}/view/CameraFirstPerson.o \
	${OBJECTDIR}/view/CameraStrategy.o \
	${OBJECTDIR}/view/ColladaModelFactory.o \
	${OBJECTDIR}/view/GameView.o \
	${OBJECTDIR}/view/Input.o \
	${OBJECTDIR}/view/MaterialManager.o \
	${OBJECTDIR}/view/ModelManager.o \
	${OBJECTDIR}/view/OBJModelFactory.o \
	${OBJECTDIR}/view/ParticleEffect.o \
	${OBJECTDIR}/view/ParticleModel.o \
	${OBJECTDIR}/view/ParticleSystem.o \
	${OBJECTDIR}/view/ParticleTracer.o \
	${OBJECTDIR}/view/SelectionTools.o \
	${OBJECTDIR}/view/ShaderManager.o \
	${OBJECTDIR}/view/SkyBox.o \
	${OBJECTDIR}/view/TextWriter.o \
	${OBJECTDIR}/view/TextureManager.o \
	${OBJECTDIR}/view/WBMModelFactory.o


# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../../LUA/LUA/liblua.a ../../MathLib/MathLib/dist/Debug/GNU-Linux-x86/libmathlib.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wargameengine

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wargameengine: ../../LUA/LUA/liblua.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wargameengine: ../../MathLib/MathLib/dist/Debug/GNU-Linux-x86/libmathlib.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wargameengine: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wargameengine ${OBJECTFILES} ${LDLIBSOPTIONS} -lGL -lGLU -lGLEW -lglut -ldl -lm `freetype-config --libs` -lopenal -pthread

${OBJECTDIR}/LogWriter.o: LogWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogWriter.o LogWriter.cpp

${OBJECTDIR}/Module.o: Module.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Module.o Module.cpp

${OBJECTDIR}/NetSocket.o: NetSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NetSocket.o NetSocket.cpp

${OBJECTDIR}/Network.o: Network.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Network.o Network.cpp

${OBJECTDIR}/OSSpecific.o: OSSpecific.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OSSpecific.o OSSpecific.cpp

${OBJECTDIR}/Plugin.o: Plugin.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Plugin.o Plugin.cpp

${OBJECTDIR}/PluginManager.o: PluginManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/PluginManager.o PluginManager.cpp

${OBJECTDIR}/Ruler.o: Ruler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Ruler.o Ruler.cpp

${OBJECTDIR}/SoundPlayer.o: SoundPlayer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SoundPlayer.o SoundPlayer.cpp

${OBJECTDIR}/ThreadPool.o: ThreadPool.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ThreadPool.o ThreadPool.cpp

${OBJECTDIR}/TranslationManager.o: TranslationManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TranslationManager.o TranslationManager.cpp

${OBJECTDIR}/UI/UIButton.o: UI/UIButton.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UIButton.o UI/UIButton.cpp

${OBJECTDIR}/UI/UICheckBox.o: UI/UICheckBox.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UICheckBox.o UI/UICheckBox.cpp

${OBJECTDIR}/UI/UIComboBox.o: UI/UIComboBox.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UIComboBox.o UI/UIComboBox.cpp

${OBJECTDIR}/UI/UIEdit.o: UI/UIEdit.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UIEdit.o UI/UIEdit.cpp

${OBJECTDIR}/UI/UIElement.o: UI/UIElement.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UIElement.o UI/UIElement.cpp

${OBJECTDIR}/UI/UIList.o: UI/UIList.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UIList.o UI/UIList.cpp

${OBJECTDIR}/UI/UIPanel.o: UI/UIPanel.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UIPanel.o UI/UIPanel.cpp

${OBJECTDIR}/UI/UIRadioGroup.o: UI/UIRadioGroup.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UIRadioGroup.o UI/UIRadioGroup.cpp

${OBJECTDIR}/UI/UIScrollBar.o: UI/UIScrollBar.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UIScrollBar.o UI/UIScrollBar.cpp

${OBJECTDIR}/UI/UIStaticText.o: UI/UIStaticText.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UIStaticText.o UI/UIStaticText.cpp

${OBJECTDIR}/UI/UIText.o: UI/UIText.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UIText.o UI/UIText.cpp

${OBJECTDIR}/UI/UITheme.o: UI/UITheme.cpp 
	${MKDIR} -p ${OBJECTDIR}/UI
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/UI/UITheme.o UI/UITheme.cpp

${OBJECTDIR}/controller/CommandChangeGlobalProperty.o: controller/CommandChangeGlobalProperty.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/CommandChangeGlobalProperty.o controller/CommandChangeGlobalProperty.cpp

${OBJECTDIR}/controller/CommandChangeProperty.o: controller/CommandChangeProperty.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/CommandChangeProperty.o controller/CommandChangeProperty.cpp

${OBJECTDIR}/controller/CommandCompound.o: controller/CommandCompound.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/CommandCompound.o controller/CommandCompound.cpp

${OBJECTDIR}/controller/CommandCreateObject.o: controller/CommandCreateObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/CommandCreateObject.o controller/CommandCreateObject.cpp

${OBJECTDIR}/controller/CommandDeleteObject.o: controller/CommandDeleteObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/CommandDeleteObject.o controller/CommandDeleteObject.cpp

${OBJECTDIR}/controller/CommandHandler.o: controller/CommandHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/CommandHandler.o controller/CommandHandler.cpp

${OBJECTDIR}/controller/CommandMoveObject.o: controller/CommandMoveObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/CommandMoveObject.o controller/CommandMoveObject.cpp

${OBJECTDIR}/controller/CommandRotateObject.o: controller/CommandRotateObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/CommandRotateObject.o controller/CommandRotateObject.cpp

${OBJECTDIR}/controller/GameController.o: controller/GameController.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/GameController.o controller/GameController.cpp

${OBJECTDIR}/controller/LUARegisterFunctions.o: controller/LUARegisterFunctions.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/LUARegisterFunctions.o controller/LUARegisterFunctions.cpp

${OBJECTDIR}/controller/LUARegisterObject.o: controller/LUARegisterObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/LUARegisterObject.o controller/LUARegisterObject.cpp

${OBJECTDIR}/controller/LUARegisterUI.o: controller/LUARegisterUI.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/LUARegisterUI.o controller/LUARegisterUI.cpp

${OBJECTDIR}/controller/LUAScriptHandler.o: controller/LUAScriptHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/LUAScriptHandler.o controller/LUAScriptHandler.cpp

${OBJECTDIR}/controller/TimedCallback.o: controller/TimedCallback.cpp 
	${MKDIR} -p ${OBJECTDIR}/controller
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/controller/TimedCallback.o controller/TimedCallback.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/model/Bounding.o: model/Bounding.cpp 
	${MKDIR} -p ${OBJECTDIR}/model
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/model/Bounding.o model/Bounding.cpp

${OBJECTDIR}/model/GameModel.o: model/GameModel.cpp 
	${MKDIR} -p ${OBJECTDIR}/model
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/model/GameModel.o model/GameModel.cpp

${OBJECTDIR}/model/Landscape.o: model/Landscape.cpp 
	${MKDIR} -p ${OBJECTDIR}/model
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/model/Landscape.o model/Landscape.cpp

${OBJECTDIR}/model/MovementLimiter.o: model/MovementLimiter.cpp 
	${MKDIR} -p ${OBJECTDIR}/model
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/model/MovementLimiter.o model/MovementLimiter.cpp

${OBJECTDIR}/model/Object.o: model/Object.cpp 
	${MKDIR} -p ${OBJECTDIR}/model
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/model/Object.o model/Object.cpp

${OBJECTDIR}/model/ObjectGroup.o: model/ObjectGroup.cpp 
	${MKDIR} -p ${OBJECTDIR}/model
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/model/ObjectGroup.o model/ObjectGroup.cpp

${OBJECTDIR}/model/Projectile.o: model/Projectile.cpp 
	${MKDIR} -p ${OBJECTDIR}/model
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/model/Projectile.o model/Projectile.cpp

${OBJECTDIR}/nv_dds.o: nv_dds.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/nv_dds.o nv_dds.cpp

${OBJECTDIR}/tinyxml.o: tinyxml.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tinyxml.o tinyxml.cpp

${OBJECTDIR}/tinyxmlerror.o: tinyxmlerror.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tinyxmlerror.o tinyxmlerror.cpp

${OBJECTDIR}/tinyxmlparser.o: tinyxmlparser.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tinyxmlparser.o tinyxmlparser.cpp

${OBJECTDIR}/view/3dModel.o: view/3dModel.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/3dModel.o view/3dModel.cpp

${OBJECTDIR}/view/CameraFirstPerson.o: view/CameraFirstPerson.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/CameraFirstPerson.o view/CameraFirstPerson.cpp

${OBJECTDIR}/view/CameraStrategy.o: view/CameraStrategy.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/CameraStrategy.o view/CameraStrategy.cpp

${OBJECTDIR}/view/ColladaModelFactory.o: view/ColladaModelFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/ColladaModelFactory.o view/ColladaModelFactory.cpp

${OBJECTDIR}/view/GameView.o: view/GameView.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/GameView.o view/GameView.cpp

${OBJECTDIR}/view/Input.o: view/Input.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/Input.o view/Input.cpp

${OBJECTDIR}/view/MaterialManager.o: view/MaterialManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/MaterialManager.o view/MaterialManager.cpp

${OBJECTDIR}/view/ModelManager.o: view/ModelManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/ModelManager.o view/ModelManager.cpp

${OBJECTDIR}/view/OBJModelFactory.o: view/OBJModelFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/OBJModelFactory.o view/OBJModelFactory.cpp

${OBJECTDIR}/view/ParticleEffect.o: view/ParticleEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/ParticleEffect.o view/ParticleEffect.cpp

${OBJECTDIR}/view/ParticleModel.o: view/ParticleModel.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/ParticleModel.o view/ParticleModel.cpp

${OBJECTDIR}/view/ParticleSystem.o: view/ParticleSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/ParticleSystem.o view/ParticleSystem.cpp

${OBJECTDIR}/view/ParticleTracer.o: view/ParticleTracer.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/ParticleTracer.o view/ParticleTracer.cpp

${OBJECTDIR}/view/SelectionTools.o: view/SelectionTools.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/SelectionTools.o view/SelectionTools.cpp

${OBJECTDIR}/view/ShaderManager.o: view/ShaderManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/ShaderManager.o view/ShaderManager.cpp

${OBJECTDIR}/view/SkyBox.o: view/SkyBox.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/SkyBox.o view/SkyBox.cpp

${OBJECTDIR}/view/TextWriter.o: view/TextWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/TextWriter.o view/TextWriter.cpp

${OBJECTDIR}/view/TextureManager.o: view/TextureManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/TextureManager.o view/TextureManager.cpp

${OBJECTDIR}/view/WBMModelFactory.o: view/WBMModelFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/view
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../LUA/LUA -I../../MathLib/MathLib -I/usr/include/freetype2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/view/WBMModelFactory.o view/WBMModelFactory.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wargameengine

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
