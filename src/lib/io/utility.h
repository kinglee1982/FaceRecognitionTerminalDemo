#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <sys/time.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace suanzi {

std::string basename(const std::string &fname);

std::string join(const std::vector<std::string> &paths);

bool makedirs(const std::string &path);

/*std::string md5(const uchar *data, int length);

std::string md5(const std::string &content);

std::string md5(const cv::Mat &image);
*/
bool makedirs(const std::string &path);

void compress_string(const std::string &str, std::string &outstring);

void rotate(const cv::Mat &src, cv::Mat &dst, int degree);

bool is_local();

float getElapse(struct timeval *tv1, struct timeval *tv2);

std::vector<std::string> split(const std::string &s, const char &c);

}  // namespace suanzi

#endif  // _UTILITY_H_
