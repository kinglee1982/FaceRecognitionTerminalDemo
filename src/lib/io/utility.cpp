#include "utility.h"

#include <openssl/md5.h>
#include <zlib.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

namespace suanzi {
/*
std::string md5(const uchar *data, int length) {
  uchar *digest = MD5(data, length, NULL);
  char hash_code_printable[33];
  for (int i = 0; i < 16; i++)
    sprintf(hash_code_printable + i * 2, "%02x", (unsigned int)digest[i]);
  return std::string(hash_code_printable);
}

std::string md5(const std::string &content) {
  return md5((const uchar *)content.c_str(), content.size());
}

std::string md5(const cv::Mat &image) {
  return md5((const uchar *)image.data, (int)image.step[0] * image.rows);
}*/

std::string basename(const std::string &fname) {
  std::string::size_type pos;
  if ((pos = fname.find_last_of("\\/")) != std::string::npos)
    return fname.substr(pos + 1);
  else
    return fname;
}

std::string join(const std::vector<std::string> &paths) {
  std::ostringstream res;

  bool flag = false;
  for (auto &path : paths) {
    if (flag) res << "/";
    res << path;
    flag = path.back() != '/';
  }

  return res.str();
}

bool makedirs(const std::string &path) {
  std::string cmd = "mkdir -p " + path + " && chmod 777 -R " + path;
  if (system(cmd.c_str()) < 0) {
    std::cerr << "cannot create " << path << std::endl;
    return false;
  } else
    return true;
}

void compress_string(const std::string &str, std::string &outstring) {
  z_stream zs;  // z_stream is zlib's control structure

  memset(&zs, 0, sizeof(zs));

  if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
    throw(std::runtime_error("deflateInit failed while compressing."));
  }

  zs.next_in = (Bytef *)str.data();
  zs.avail_in = str.size();  // set the z_stream's input

  int ret;
  char outbuffer[50000];

  // retrieve the compressed bytes blockwise
  do {
    zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
    zs.avail_out = sizeof(outbuffer);

    ret = deflate(&zs, Z_FINISH);

    if (outstring.size() < zs.total_out) {
      // append the block to the output string
      outstring.append(outbuffer, zs.total_out - outstring.size());
    }
  } while (ret == Z_OK);

  deflateEnd(&zs);

  if (ret != Z_STREAM_END)  // an error occurred that was not EOF
  {
    std::ostringstream oss;
    oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
    throw(std::runtime_error(oss.str()));
  }
}

void rotate(const cv::Mat &src, cv::Mat &dst, int degree) {
  switch ((degree + 360) % 360) {
    case 90:
      cv::transpose(src, dst);
      cv::flip(dst, dst, 1);
      break;
    case 180:
      cv::flip(src, dst, 0);
      break;
    case 270:
      cv::transpose(src, dst);
      cv::flip(dst, dst, 0);
      break;
    default:
      src.copyTo(dst);
      break;
  }
}

bool is_local() {
#if LOCAL_MODE
  return true;
#else
  return false;
#endif
}

float getElapse(struct timeval *tv1, struct timeval *tv2) {
  float t = 0.0f;

  if (tv1->tv_sec == tv2->tv_sec)
    t = (tv2->tv_usec - tv1->tv_usec) / 1000.0f;
  else
    t = ((tv2->tv_sec - tv1->tv_sec) * 1000 * 1000 + tv2->tv_usec -
         tv1->tv_usec) /
        1000.0f;
  return t;
}

vector<std::string> split(const std::string &s, const char &c) {
  string buff{""};
  vector<string> v;
  for (auto n : s) {
    if (n != c)
      buff += n;
    else if (n == c && buff != "") {
      v.push_back(buff);
      buff = "";
    }
  }

  if (buff != "") v.push_back(buff);
  return v;
}

}  // namespace suanzi
