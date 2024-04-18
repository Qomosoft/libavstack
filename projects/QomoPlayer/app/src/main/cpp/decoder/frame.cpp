//
// Created by Robin on 2024/4/13.
//

#include "frame.h"

#include <sstream>

Frame::Frame() : data_(nullptr) {}
Frame::~Frame() {
  if (data_) {
    delete data_;
    data_ = nullptr;
  }
}
int Frame::GetMediaType() const {
  return media_type_;
}
void Frame::SetMediaType(int media_type) {
  media_type_ = media_type;
}
int Frame::GetWidth() const {
  return width_;
}
void Frame::SetWidth(int width) {
  width_ = width;
}
int Frame::GetHeight() const {
  return height_;
}
void Frame::SetHeight(int height) {
  height_ = height;
}
float Frame::GetPts() const {
  return pts_;
}
void Frame::SetPts(float pts) {
  pts_ = pts;
}
float Frame::GetDuration() const {
  return duration_;
}
void Frame::SetDuration(float duration) {
  duration_ = duration;
}
std::string Frame::ToString() const {
  std::stringstream ss;
  ss << "[pts=" << pts_ << ", duration=" << duration_ << "]";
  return ss.str();
}
uint8_t *Frame::GetData() const {
  return data_;
}
void Frame::SetData(uint8_t *data) {
  data_ = data;
}
long Frame::GetSize() const {
  return size_;
}
void Frame::SetSize(long size) {
  size_ = size;
}
int Frame::GetPixFmt() const {
  return pix_fmt_;
}
void Frame::SetPixFmt(int pix_fmt) {
  pix_fmt_ = pix_fmt;
}
