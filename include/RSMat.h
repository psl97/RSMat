#pragma once
#include<iostream>
#include<stdio.h>
#include<cassert>
#include<complex>
#include <vector>
#include"const.h"
#include"gdal.h"
#include"gdal_priv.h"
#include"opencv2/core/core.hpp"

using namespace std; 
using namespace cv;

class RSMat : public Mat
{
public:
	
	int width = 0;
	int height = 0;
	int bandNum = 0;
	MatDataType MDType = MDT_Unknown; 
	GDALDataType GDType = GDT_Unknown;
	GDALTileOrganization Interleave = GTO_BSQ; // TODO: This field is not currently in use
	string proj = "";
	double geoTransform[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

	RSMat();
	RSMat(int rows, int cols, int type);
	RSMat(Size size, int type);
	RSMat(int rows, int cols, int type, const Scalar& s);
	RSMat(Size size, int type, const Scalar& s);
	RSMat(int ndims, const int* sizes, int type);
	RSMat(const vector<int>& sizes, int type);
	RSMat(int ndims, const int* sizes, int type, const Scalar& s);
	RSMat(const vector<int>& sizes, int type, const Scalar& s);
	RSMat(const Mat& m);
	RSMat(const RSMat& m);
	RSMat(const vector<Mat>& m);
	RSMat(const MatExpr& m) : Mat(m) {};
	RSMat(int rows, int cols, int type, void* data, size_t step = AUTO_STEP);
	RSMat(Size size, int type, void* data, size_t step = AUTO_STEP);
	RSMat(int ndims, const int* sizes, int type, void* data, const size_t* steps = 0);
	RSMat(const vector<int>& sizes, int type, void* data, const size_t* steps = 0);
	RSMat(const Mat& m, const Range& rowRange, const Range& colRange = Range::all());
	RSMat(const Mat& m, const Rect& roi);
	RSMat(const Mat& m, const Range* ranges);
	RSMat(const Mat& m, const vector<Range>& ranges);
	RSMat(string fileName, int xSize = 0, int ySize = 0, int xOffset = 0, int yOffset = 0,
		int ChosenBandCount = 0, int* ChosenpanBandMap = nullptr, GDALRasterIOExtraArg* psExtraArg = nullptr);

	int GetXSize() const { return width; }
	int GetYSize() const { return height; }
	int GetBandNum() const { return bandNum; }
	string GetProject() const { return proj; }
	MatDataType GetMDType() const { return MDType; }
	GDALDataType GetGDType() const { return GDType; }
	GDALTileOrganization GetInterleave() const { return Interleave; }
	double* GetGeoTransform() const {
		double* Transform = new double[6];
		for (int i = 0; i < 6; i++)
			Transform[i] = geoTransform[i];

		return Transform;
	}

	void SetXSize(){ width = Mat::cols; }
	void SetYSize() { height =  Mat::rows; }
	void SetChannels() { bandNum = Mat::channels(); }
	void SetProj(string proj_wkt) { proj = proj_wkt.substr(); }
	void SetTransform(double* GeoTransform) {
		for (int i = 0; i < 6; i++)
			geoTransform[i] = *(GeoTransform + i);
	}

	void SetMDType() { MDType = (MatDataType)Mat::type(); }
	void SetMDType(MatDataType DT) { MDType = DT; }
	void SetGDType(GDALDataType DT) { GDType = DT; }
	void SetTileOrganization(GDALTileOrganization TO) { Interleave = TO; }
	void SetAttributes(const RSMat& m) {
		SetProj(m.GetProject());
		SetTransform(m.GetGeoTransform());
		SetTileOrganization(m.GetInterleave());
		SetXSize();
		SetYSize();
		SetChannels();
		SetMDType();
		SetGDType(GetGDType(MDType));
		
	}

	int GetRSImgInfo(string fileName);

	bool GetTileOrganizationInfo(GDALTileOrganization Interleave, GDALDataType DataType,
		int bandNum, int width, int height, int& nPixelSpace, int& nLineSpace, int& nBandSpace);

	bool ImageRowCol2Projection(double* GeoTransform, int x, int y, double& dProjX, double& dProjY);

	bool Projection2ImageRowCol(double* GeoTransform, double dProjX, double dProjY, int& x, int& y);

	int WriteImage(string filname, int ChosenBandCount = 0, int* ChosenpanBandMap = nullptr, const char* pszFormat = "GTiff");

	MatDataType GetMDType(GDALDataType GDType);
	
	GDALDataType GetGDType(MatDataType MDType);
	
	RSMat convertTo(GDALDataType GDType, double alpha = 1.0, double beta = 0.0);
	
	RSMat extractChannel(int i) const;
	
	RSMat clone() const;
	
	void release();
};



