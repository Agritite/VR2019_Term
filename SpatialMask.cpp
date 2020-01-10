//VR2019 Term Project
#include "SpatialMask.h"
#include <algorithm>

cv::Mat SpatialMask(const cv::Mat& input3b_bgr, const size_t ksize)
{
	CV_Assert(input3b_bgr.type() == CV_8UC3);
	const size_t rows = input3b_bgr.rows, cols = input3b_bgr.cols;
	cv::Mat output3b_bgr = input3b_bgr.clone();
	for(size_t i = 0 ; i + ksize < rows ; i++)
	{
		for(size_t j = 0 ; j + ksize < cols ; j++)
		{
			cv::Mat_<int> mask(ksize, ksize);
			short count[21] = {0};
			for(size_t s = 0 ; s < ksize ; s++)
			{
				int *const ps = mask.ptr<int>(s);
				const cv::Vec3b *const pi = input3b_bgr.ptr<cv::Vec3b>(i + s);
				for(size_t t = 0 ; t < ksize ; t++)
				{
					double tmp = pi[j + t][0] + pi[j + t][1] + pi[j + t][1];
					ps[t] = static_cast<int>((tmp / (3 * 255)) * 20);
					if(ps[t] > 20)
						ps[t] = 20;
					count[ps[t]]++;
				}
			}
			int maxlevel = std::max_element(count, count + 21) - count;
			int avg[3] = {0};
			for(size_t s = 0 ; s < ksize ; s++)
			{
				int *const ps = mask.ptr<int>(s);
				const cv::Vec3b *const pi = input3b_bgr.ptr<cv::Vec3b>(i + s);
				for(size_t t = 0 ; t < ksize ; t++)
				{
					if(ps[t] == maxlevel)
					{
						avg[0] += pi[j + t][0];
						avg[1] += pi[j + t][1];
						avg[2] += pi[j + t][2];
					}
				}
			}
			avg[0] /= count[maxlevel];
			avg[1] /= count[maxlevel];
			avg[2] /= count[maxlevel];
			cv::Vec3b *const po = output3b_bgr.ptr<cv::Vec3b>(i + ksize / 2);
			po[j + ksize / 2][0] = avg[0];
			po[j + ksize / 2][1] = avg[1];
			po[j + ksize / 2][2] = avg[2];
		}
	}
	return output3b_bgr;
}