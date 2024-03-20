//
// Created by Robin on 2024/3/18.
//

#include "media_player.h"
#include "logging.h"

void MediaPlayer::Init(int decoder_type) {
  LOGI("enter");
  decoder_type_ = decoder_type;
}
void MediaPlayer::Finalize() {
  LOGI("enter");
}
void MediaPlayer::SetDataSource(const std::string &path) {
  LOGI("path=%s", path.c_str());
  path_ = path;
}
void MediaPlayer::Prepare() {
  LOGI("enter");
  synchronizer_ = std::make_shared<AVSynchronizer>();
  synchronizer_->Init(path_, decoder_type_);
}
void MediaPlayer::PrepareAsync() {

}
void MediaPlayer::Start() {

}
void MediaPlayer::Stop() {

}
void MediaPlayer::Pause() {

}
void MediaPlayer::Seek(float position) {

}
