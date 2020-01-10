//VR2019 Term Project
#include "quantize.h"
#include <unordered_map>
#include <queue>

struct vec3b_hash
{
	size_t operator()(const cv::Vec3b& k) const;
};

template<typename T>
using vec3b_hashmap = std::unordered_map<cv::Vec3b, T, vec3b_hash>;

struct vec3b_heap_cmp
{
	bool operator()(const vec3b_hashmap<size_t>::iterator&, const vec3b_hashmap<size_t>::iterator&) const;
};

template<typename T>
using vec3b_heap = std::priority_queue<T, std::vector<T>, vec3b_heap_cmp>;

size_t vec3b_hash::operator()(const cv::Vec3b& k) const
{
	uint32_t imm = ((uint32_t)k[0] << 16) | (uint32_t)(k[1] << 8) | (uint32_t)k[2];
	return std::hash<uint32_t>()(imm);
}

bool vec3b_heap_cmp::operator()(const vec3b_hashmap<size_t>::iterator& a, const vec3b_hashmap<size_t>::iterator& b) const
{
	return a->second > b->second;
}

uint32_t vec_dist2(const cv::Vec3b& a, const cv::Vec3b& b)
{
	return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]);
}

cv::Mat Quantize(const cv::Mat& input3b_bgr, const double ratio)
{
	CV_Assert(input3b_bgr.type() == CV_8UC3);
	cv::Mat output3b_bgr = input3b_bgr.clone();

	//Quantize each channel into 0~11, reducing colors to 12^3=1728.
	for(size_t i = 0 ; i < output3b_bgr.rows ; i++)
	{
		for(size_t j = 0 ; j < output3b_bgr.cols ; j++)
		{
			cv::Vec3b& pixel = output3b_bgr.at<cv::Vec3b>(i, j);
			for(uint8_t k = 0 ; k < 3 ; k++)
			{		
				uint16_t extend = pixel[k];
				extend = (extend * 12 / 0x100) * (0x100 / 12);
				pixel[k] = extend;
			}
		}
	}

	//Count color occurences.
	vec3b_hashmap<size_t> color_count;
	for(size_t i = 0 ; i < output3b_bgr.rows ; i++)
	{
		for(size_t j = 0 ; j < output3b_bgr.cols ; j++)
		{
			cv::Vec3b& pixel = output3b_bgr.at<cv::Vec3b>(i, j);
			if(color_count.find(pixel) == color_count.end())
				color_count[pixel] = 0;
			color_count[pixel]++;
		}
	}

	//Create a heap of colors sorted by occurences.
	vec3b_heap<vec3b_hashmap<size_t>::iterator> shrink;
	for(auto it = color_count.begin() ; it != color_count.end() ; it++)
		shrink.push(it);

	//Pop the least occured 5% colors and push them into another hash table.
	vec3b_hashmap<cv::Vec3b> less_occuring_color;
	for(size_t accumulated = 0 ; accumulated < output3b_bgr.rows * output3b_bgr.cols * ratio ; shrink.pop())
	{
		accumulated += shrink.top()->second;
		color_count.erase(shrink.top());
		less_occuring_color[shrink.top()->first] = {0, 0, 0};
	}

	//For each of the least occured colors, find the color that has the nearest Euclidean distance with it.
	for(auto it1 = less_occuring_color.begin() ; it1 != less_occuring_color.end() ; it1++)
	{
		auto smallest_it = color_count.end();
		uint64_t smallest_dist2 = 0xFFFFFFFFFFFFFFFF;
		for(auto it2 = color_count.begin() ; it2 != color_count.end() ; it2++)
		{
			uint32_t cur_dist2 = vec_dist2(it1->first, it2->first);
			if(cur_dist2 < smallest_dist2)
			{
				smallest_it = it2;
				smallest_dist2 = cur_dist2;
			}
		}
		it1->second = smallest_it->first;
	}

	//Replace the least occured colors.
	for(size_t i = 0 ; i < output3b_bgr.rows ; i++)
	{
		for(size_t j = 0 ; j < output3b_bgr.cols ; j++)
		{
			cv::Vec3b& pixel = output3b_bgr.at<cv::Vec3b>(i, j);
			auto it = less_occuring_color.find(pixel);
			if(it != less_occuring_color.end())
			{
				pixel = it->second;
			}
		}
	}

	return output3b_bgr;
}