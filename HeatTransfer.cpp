//VR2019 Term Project
#include "HeatTransfer.h"
#include <cmath>

inline float pow2(const float in)
{
	return in * in;
}

cv::Mat HeatTransfer(const cv::Mat& input3f_bgr, const size_t n, const float timestep, const float alpha)
{
	CV_Assert(input3f_bgr.type() == CV_32FC3);
	cv::Mat GradX, GradY, diffK[3], BGRf[3];
	cv::split(input3f_bgr, BGRf);
	for(size_t ci = 0 ; ci < 3 ; ci++)
		diffK[ci].create(input3f_bgr.size(), CV_32F);
	for(size_t it = 0 ; it < n ; it++)
	{
		for(size_t ci = 0 ; ci < 3 ; ci++)
		{
			cv::Sobel(BGRf[ci], GradX, CV_32F, 1, 0);
			cv::Sobel(BGRf[ci], GradY, CV_32F, 0, 1);
			for(size_t i = 0 ; i < BGRf[ci].rows ; i++)
			{
				const float* const pgxi = GradX.ptr<float>(i);
				const float* const pgyi = GradY.ptr<float>(i);
				float* const pdki = diffK[ci].ptr<float>(i);
				for(size_t j = 0 ; j < BGRf[ci].cols ; j++)
					pdki[j] = exp(-pow2(sqrt(pow2(pgxi[j]) + pow2(pgyi[j])) / alpha));
			}

			cv::Sobel(BGRf[ci], GradX, CV_32F, 2, 0);
			cv::Sobel(BGRf[ci], GradY, CV_32F, 0, 2);
			for(size_t i = 0 ; i < BGRf[ci].rows ; i++)
			{
				const float* const pgxi = GradX.ptr<float>(i);
				const float* const pgyi = GradY.ptr<float>(i);
				const float* const pdki = diffK[ci].ptr<float>(i);
				float* const bgrfi = BGRf[ci].ptr<float>(i);
				for(size_t j = 0 ; j < BGRf[ci].cols ; j++)
					bgrfi[j] += timestep * pdki[j] * (pgxi[j] + pgyi[j]);
			}
		}
	}
	cv::Mat output3f_bgr;
	cv::merge(BGRf, 3, output3f_bgr);
	return output3f_bgr;
}