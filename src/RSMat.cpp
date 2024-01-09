#include "../include/RSMat.h"

RSMat::RSMat(): 
	Mat() {}
RSMat::RSMat(int rows, int cols, int type): 
	Mat(rows, cols, type) {}
RSMat::RSMat(Size size, int type): 
	Mat(size, type) {}
RSMat::RSMat(int rows, int cols, int type, const Scalar& s): 
	Mat(rows, cols, type, s) {}
RSMat::RSMat(Size size, int type, const Scalar& s): 
	Mat(size, type, s) {}
RSMat::RSMat(int ndims, const int* sizes, int type): 
	Mat(ndims, sizes, type) {}
RSMat::RSMat(const std::vector<int>& sizes, int type): 
	Mat(sizes, type) {}
RSMat::RSMat(int ndims, const int* sizes, int type, const Scalar& s): 
	Mat(ndims, sizes, type, s) {}
RSMat::RSMat(const std::vector<int>& sizes, int type, const Scalar& s): 
	Mat(sizes, type, s) {}
RSMat::RSMat(const Mat& m): 
	Mat(m) {}
RSMat::RSMat(const RSMat& m):
	Mat(m) { SetAttributes(m); }
RSMat::RSMat(int rows, int cols, int type, void* data, size_t step): 
	Mat(rows, cols, type, data, step) {}
RSMat::RSMat(Size size, int type, void* data, size_t step): 
	Mat(size, type, data, step) {}
RSMat::RSMat(int ndims, const int* sizes, int type, void* data, const size_t* steps): 
	Mat(ndims, sizes, type, data, steps) {}
RSMat::RSMat(const std::vector<int>& sizes, int type, void* data, const size_t* steps):
	Mat(sizes, type, data, steps) {}
RSMat::RSMat(const Mat& m, const Range& rowRange, const Range& colRange):
	Mat(m, rowRange, colRange) {}
RSMat::RSMat(const Mat& m, const Rect& roi):
	Mat(m, roi) {}
RSMat::RSMat(const Mat& m, const Range* ranges):
	Mat(m, ranges) {}
RSMat::RSMat(const Mat& m, const std::vector<Range>& ranges):
	Mat(m, ranges) {}

RSMat::RSMat(string fileName, int xSize, int ySize, int xOffset, int yOffset,
	int ChosenBandCount, int* ChosenpanBandMap, GDALRasterIOExtraArg* psExtraArg)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

	GDALDataset* Dataset = nullptr;
	Dataset = (GDALDataset*)GDALOpen(fileName.c_str(), GA_ReadOnly);
	if (Dataset == nullptr)
	{
		cout << "Can not open the file dataset " + fileName << endl;
		GDALClose(Dataset);
	}

	
	int flag;
	flag = GetRSImgInfo(fileName);
	if (flag != 0)
	{
		cout << "Get GeoImage Info failed" << endl;
		throw - 1;
	}

	if (xOffset < 0 || xOffset > width - 1)
		xOffset = 0;
	if (yOffset < 0 || yOffset > height - 1)
		yOffset = 0;

	if (xSize <= 0 || xSize > width)
		xSize = width;
	if (ySize <= 0 || ySize > height)
		ySize = height;

	if (xOffset + xSize > width)
		xSize = width - xOffset;
	if (yOffset + ySize > height)
		ySize = height - yOffset;

	if (xSize != width)
		width = xSize;
	if (ySize != height)
		height = ySize;

	int* panBandMap = nullptr;
	int nBandCount = 0;
	if (ChosenBandCount == 0)
	{
		nBandCount = bandNum;
		panBandMap = new int[nBandCount];
		for (int i = 0; i < nBandCount; ++i)
			panBandMap[i] = i + 1;
	}
	else
	{
		nBandCount = ChosenBandCount;
		bandNum = ChosenBandCount;
		panBandMap = ChosenpanBandMap;
	}

	if (xOffset != 0 || yOffset != 0)
		ImageRowCol2Projection(geoTransform, xOffset, yOffset, geoTransform[0], geoTransform[3]);

	void* pafScan = nullptr;
	switch (GDType)
	{
	case GDT_Byte:
		pafScan = new Byte[width * height]; break;
	case GDT_UInt16:
		pafScan = new UInt16[width * height]; break;
	case GDT_Int16:
		pafScan = new Int16[width * height]; break;
	case GDT_UInt32:
		pafScan = new UInt32[width * height]; break;
	case GDT_Int32:
		pafScan = new Int32[width * height]; break;
	case GDT_Float32:
		pafScan = new Float[width * height]; break;
	case GDT_Float64:
		pafScan = new Double[width * height]; break;
	default:
		cout << "Do not support the complex dataset " + fileName << endl;	
	}
	
	GDALRasterBand* pBand = nullptr;
	Mat A;
	vector<Mat> imgMat;
	for (int i = 0; i < nBandCount; i++)
	{
		pBand = Dataset->GetRasterBand(panBandMap[i]);
		pBand->RasterIO(GF_Read, xOffset, yOffset, width, height, pafScan,
			width, height, GDType, 0, 0);
		MDType = GetMDType(GDType);
		A = Mat(height, width, MDType, pafScan);
		imgMat.push_back(A.clone());
		A.release();
	}

	cv::merge(imgMat, *this);
	for (int i = 0; i < imgMat.size(); i++)
		imgMat[i].release();
	vector<Mat>().swap(imgMat);

	GDALClose((GDALDatasetH)Dataset);
}

int RSMat::GetRSImgInfo(string fileName)
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

	GDALDataset* Dataset = nullptr;
	Dataset = (GDALDataset*)GDALOpen(fileName.c_str(), GA_ReadOnly);
	if (Dataset == nullptr)
	{
		cout << "Can not open the file dataset " + fileName << endl;
		GDALClose(Dataset);
		return -1;
	}

	bandNum = Dataset->GetRasterCount();
	if (bandNum == 0)
	{
		cout << "The file dataset has no band information" << endl;
		GDALClose(Dataset);
		return -2;
	}

	GDType = GDALGetRasterDataType(GDALGetRasterBand(Dataset, 1));
	width = Dataset->GetRasterXSize();
	height = Dataset->GetRasterYSize();
	proj = string(Dataset->GetProjectionRef());
	Dataset->GetGeoTransform(geoTransform);
	GDALClose((GDALDatasetH)Dataset);

	return 0;
}

bool RSMat::GetTileOrganizationInfo(GDALTileOrganization Interleave, GDALDataType DataType,
	int bandNum, int width, int height, int& nPixelSpace, int& nLineSpace, int& nBandSpace)
{
	switch (Interleave)
	{
	case GTO_TIP:
		nBandSpace = GDALGetDataTypeSizeBytes(DataType);
		nPixelSpace = nBandSpace * bandNum;
		nLineSpace = nPixelSpace * width;
		return true;
	case GTO_BSQ:
		nPixelSpace = GDALGetDataTypeSizeBytes(DataType);
		nLineSpace = nPixelSpace * width;
		nBandSpace = nLineSpace * height;
		return true;
	case GTO_BIT:
		nPixelSpace = GDALGetDataTypeSizeBytes(DataType);
		nLineSpace = nPixelSpace * width;
		nBandSpace = nLineSpace;
		return true;
	default:
		printf("Unknown Interleave\n");
		return false;
	}

}

bool RSMat::ImageRowCol2Projection(double* GeoTransform, int x, int y, double& dProjX, double& dProjY)
{
	try
	{
		dProjX = GeoTransform[0] + GeoTransform[1] * x + GeoTransform[2] * y;
		dProjY = GeoTransform[3] + GeoTransform[4] * y + GeoTransform[5] * y;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool RSMat::Projection2ImageRowCol(double* GeoTransform, double dProjX, double dProjY, int& x, int& y)
{
	try
	{
		double dTemp = GeoTransform[1] * GeoTransform[5] - GeoTransform[2] * GeoTransform[4];
		double dX = 0.0, dY = 0.0;
		dX = (GeoTransform[5] * (dProjX - GeoTransform[0]) -
			GeoTransform[2] * (dProjY - GeoTransform[3])) / dTemp + 0.5;
		dY = (GeoTransform[1] * (dProjY - GeoTransform[3]) -
			GeoTransform[4] * (dProjX - GeoTransform[0])) / dTemp + 0.5;

		x = static_cast<int>(dX);
		y = static_cast<int>(dY);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

int RSMat::WriteImage(string fileName, int ChosenBandCount, int* ChosenpanBandMap, const char* pszFormat)
{

	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

	int* panBandMap = nullptr;
	int nBandCount = 0;
	if (ChosenBandCount == 0)
	{
		nBandCount = bandNum;
		panBandMap = new int[nBandCount];
		for (int i = 0; i < nBandCount; ++i)
			panBandMap[i] = i + 1;
	}
	else
	{
		nBandCount = ChosenBandCount;
		panBandMap = ChosenpanBandMap;
	}

	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	GDALDataset* Dataset = poDriver->Create(fileName.c_str(), width, height, nBandCount, GDType, NULL);
	if (Dataset == nullptr)
	{
		cout << "Can not open the file dataset " + fileName << endl;
		GDALClose(Dataset);
		return -1;
	}

	Dataset->SetGeoTransform(geoTransform);
	Dataset->SetProjection(proj.c_str());

	GDALRasterBand* pBand = nullptr;
	cv::Mat strechimage;
	for (int i = 0; i < nBandCount; i++)
	{
		pBand = Dataset->GetRasterBand(panBandMap[i]);	
		strechimage = extractChannel(panBandMap[i] - 1).clone();
		//for (int col = 0; col < ImageInfo->width * ImageInfo->height; col++) {
		//	pafScan[col] = strechimage.at<_Ty>(0, col);
		//}
		pBand->RasterIO(GF_Write, 0, 0, width, height, strechimage.data,
			width, height, GDType, 0, 0);
		strechimage.release();
	}

	RELEASE(panBandMap);
	GDALClose(Dataset);
	return 0;
}

RSMat RSMat::clone() const
{
	RSMat m = Mat::clone();
	m.SetAttributes(*this);
	return m;
}

void RSMat::release()
{
	Mat::release();
}

RSMat RSMat::extractChannel(int i) const
{
	RSMat n;
	cv::extractChannel(*this, n, i);
	n.SetAttributes(*this);
	n.SetChannels();
	return n;
}

RSMat RSMat::convertTo(GDALDataType dstGDType, double alpha, double beta)
{
	RSMat dst;
	this->Mat::convertTo(dst, GetMDType(dstGDType), alpha, beta);
	dst.SetAttributes(*this); 
	//dst.SetMDType(GetMDType(dstGDType));
	//dst.SetGDType(dstGDType);
	return dst;
}

MatDataType RSMat::GetMDType(GDALDataType GDType)
{
	MatDataType MDType = MDT_Unknown;
	if (GDType == GDT_Byte)
		MDType = MDT_Byte;
	else if (GDType == GDT_UInt16)
		MDType = MDT_UInt16;
	else if (GDType == GDT_Int16)
		MDType = MDT_Int16;
	else if (GDType == GDT_UInt32) // OPENCV do not support CV_32U
		MDType = MDT_Float32;
	else if (GDType == GDT_Int32)
		MDType = MDT_Int32;
	else if (GDType == GDT_Float32)
		MDType = MDT_Float32;
	else if (GDType == GDT_Float64)
		MDType = MDT_Float64;
	else
		MDType = MDT_Unknown;
	return MDType;
}

GDALDataType RSMat::GetGDType(MatDataType MDType)
{
	GDALDataType GDType = GDT_Unknown;
	if (MDType == MDT_Byte)
		GDType = GDT_Byte;
	else if (MDType == MDT_UInt16)
		GDType = GDT_UInt16;
	else if (MDType == MDT_Int16)
		GDType = GDT_Int16;
	else if (MDType == MDT_Int32) // OPENCV do not support CV_32U
		GDType = GDT_Int32;
	//else if (MDType == MDT_Int32)
	//	GDType = GDT_Int32;
	else if (MDType == MDT_Float32)
		GDType = GDT_Float32;
	else if (MDType == MDT_Float64)
		GDType = GDT_Float64;
	else
		GDType = GDT_Unknown;
	return GDType;
}


