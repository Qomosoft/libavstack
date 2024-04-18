//
// Created by Robin on 2024/4/13.
//
#pragma once
#include <string>

const static int kMediaTypeAudio = 0;
const static int kMediaTypeVideo = 1;

class Frame {
 public:
  Frame();
  virtual ~Frame();
  int GetMediaType() const;
  void SetMediaType(int media_type);
  float GetPts() const;
  void SetPts(float pts);
  float GetDuration() const;
  void SetDuration(float duration);
  long GetSize() const;
  void SetSize(long size);
  uint8_t *GetData() const;
  void SetData(uint8_t *data);
  int GetWidth() const;
  void SetWidth(int width);
  int GetHeight() const;
  void SetHeight(int height);
  int GetPixFmt() const;
  void SetPixFmt(int pix_fmt);
  std::string ToString() const;
 private:
  int media_type_;
  float pts_;
  float duration_;
  uint8_t *data_;
  long size_;
  int width_;
  int height_;
  int pix_fmt_;
};