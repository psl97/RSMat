#include "../include/RSMat.h"


int main()
{
	RSMat img("./RSMat/test/demo.tif");
	RSMat red = img.extractChannel(3).convertTo(GDT_Float32);
	RSMat nir = img.extractChannel(4).convertTo(GDT_Float32);
	RSMat ndvi = (nir - red) / (nir + red);
	ndvi.SetAttributes(red);
	ndvi.WriteImage("./RSMat/test/ndvi.tif");
	return 0;
}