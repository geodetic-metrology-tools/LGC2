include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(MathematicalConcepts_SOURCES 
	Quad.cpp
	T2DCartesianCoordSys.cpp
	T2DCartesianPlusHSys.cpp
	T3DCartesianCoordSys.cpp
	T3DMatrix.cpp
	TACoordinateSystem.cpp
	TACoordSysMatrix.cpp
	TACoordSysVector.cpp
	TAngle.cpp
	TANumericValue.cpp
	TColumnVector.cpp
	TCoordSysFactory.cpp
	TDist.cpp
	TDouble.cpp
	TFreeVector.cpp
	TGeodeticCoordSys.cpp
	TLength.cpp
	TMatrix.cpp
	TMatrixImpl.cpp
	TPositionVector.cpp
	TRotationMatrix.cpp
	TScalar.cpp
	TSparseMatrix.cpp
)

# Create a variable containing all .h files:
set(MathematicalConcepts_HEADERS 
	${SURVEYLIB_INCLUDE_PATH}/Quad.h
	${SURVEYLIB_INCLUDE_PATH}/T2DCartesianCoordSys.h
	${SURVEYLIB_INCLUDE_PATH}/T2DCartesianPlusHSys.h
	${SURVEYLIB_INCLUDE_PATH}/T3DCartesianCoordSys.h
	${SURVEYLIB_INCLUDE_PATH}/T3DMatrix.h
	${SURVEYLIB_INCLUDE_PATH}/TACoordinateSystem.h
	${SURVEYLIB_INCLUDE_PATH}/TACoordSysMatrix.h
	${SURVEYLIB_INCLUDE_PATH}/TACoordSysVector.h
	${SURVEYLIB_INCLUDE_PATH}/TAngle.h
	${SURVEYLIB_INCLUDE_PATH}/TANumericValue.h
	${SURVEYLIB_INCLUDE_PATH}/TColumnVector.h
	${SURVEYLIB_INCLUDE_PATH}/TCoordSysFactory.h
	${SURVEYLIB_INCLUDE_PATH}/TDist.h
	${SURVEYLIB_INCLUDE_PATH}/TDouble.h
	${SURVEYLIB_INCLUDE_PATH}/TFreeVector.h
	${SURVEYLIB_INCLUDE_PATH}/TGeodeticCoordSys.h
	${SURVEYLIB_INCLUDE_PATH}/TLength.h
	${SURVEYLIB_INCLUDE_PATH}/TMatrix.h
	${SURVEYLIB_INCLUDE_PATH}/TMatrixImpl.h
	${SURVEYLIB_INCLUDE_PATH}/TPositionVector.h
	${SURVEYLIB_INCLUDE_PATH}/TRotationMatrix.h
	${SURVEYLIB_INCLUDE_PATH}/TScalar.h
	${SURVEYLIB_INCLUDE_PATH}/TSparseMatrix.h
	${SURVEYLIB_INCLUDE_PATH}/TVCoordinateSystem.h
	${SURVEYLIB_INCLUDE_PATH}/TVNumericValue.h
)

# Create a static library from sources:
add_library(MathematicalConcepts STATIC ${MathematicalConcepts_SOURCES} ${MathematicalConcepts_HEADERS})
#install(FILES ${MathematicalConcepts_HEADERS} DESTINATION ${INSTALL_PREFIX}/include)
#####