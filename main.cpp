//VR2019 Term Project
#include "main.h"

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		//syntax
		std::cerr << "usage: watercolor [inputname] [outputname]\n";
		return -1;
	}
	const std::string inputfilename(argv[1]), outputfilename(argv[2]);
	{
		std::ifstream f(inputfilename.c_str());
		if(!f.good())
		{
			std::cerr << "error: file does not exist\n";
			return -1;
		}
	}
	cv::Mat InputImage = cv::imread(inputfilename);
	if(InputImage.data == nullptr)
	{
			std::cerr << "error: cannot read image\n";
			return -1;
	}
	InputImage.convertTo(InputImage, CV_32FC3, 1.0 / 255);
	//Heat Equation is applied under float
	cv::Mat OutputImage = HeatTransfer(InputImage, 5, 0.1, 1);
	//arguments: input image, iterations, time step, alpha
	OutputImage.convertTo(OutputImage, CV_8UC3, 255);
	//convert back to 24 bit binary

	OutputImage = SpatialMask(OutputImage, 9);
	//argument: input image, mask size k*k

	//OutputImage = Quantize(OutputImage);
	/*optional, quantizes RGB channel into 12 possible values each,
	reducing color to 1728, then furthur reduce color to an average
	of 85 by replacing the least occuring 5% colors to the closest 
	Euclidian-distance color; Might improve some images.*/

	if(!cv::imwrite(outputfilename, OutputImage))
	{
		std::cerr << "error: cannot save image\n";
		return -1;
	}
	return 0;
}