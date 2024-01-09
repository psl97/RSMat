#ifndef CONST_H 
#define CONST_H

#include <limits>
#include <string>
#include <vector>
#include <complex>
#include "gdal_priv.h"
#include "gdal_alg_priv.h"
#include "gdalwarper.h"
#include "ogrsf_frmts.h"
#include "opencv2/core/core.hpp"

typedef unsigned char Byte;
typedef unsigned short UInt16;
typedef short Int16;
typedef unsigned int UInt32;
typedef int Int32;
typedef float Float;
typedef double Double;
typedef std::complex<short> CInt16;
typedef std::complex<int> CInt32;
typedef std::complex<float> CFloat;
typedef std::complex<double> CDouble;

typedef enum {
	MDT_Unknown = -1,
	MDT_Byte = CV_8U,
	MDT_Int8 = CV_8S,
	MDT_UInt16 = CV_16U,
	MDT_Int16 = CV_16S,
	MDT_Int32 = CV_32S,
	MDT_Float32 = CV_32F,
	MDT_Float64 = CV_64F,
} MatDataType;


#ifndef __cplusplus
#define nullptr ((void*)0)
#endif
#define RELEASE(x)	if(x != nullptr) { delete[]x; x = nullptr; }

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef RAD2DEG
#define RAD2DEG(x) (double)(180.0/M_PI) * x
#endif

#ifndef DEG2RAD
#define DEG2RAD(x) (double)(M_PI/180.0) * x
#endif


#endif // _CONST_H
