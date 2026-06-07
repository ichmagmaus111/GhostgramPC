/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/ghostgram.h"

#include "ghostgram/chat_background/chat_background.h"
#include "ghostgram/ghost_mode/ghost_mode.h"
#include "ghostgram/ghostgram_config.h"
#include "ghostgram/media_archive/media_archive.h"
#include "ghostgram/message_archive/message_archive.h"

namespace Ghostgram {

Manager::Manager()
: _config(std::make_unique<Config>()) {
	_config->load();
	_ghostMode = std::make_unique<GhostMode>(_config.get());
	_archive = std::make_unique<MessageArchive>(_config.get());
	_mediaArchive = std::make_unique<MediaArchive>(_config.get());
	_chatBackground = std::make_unique<ChatBackground>(_config.get());
}

Manager::~Manager() {
	_config->save();
}

Manager &Manager::Instance() {
	// Intentionally leaked: members hold Qt objects that must not be
	// destroyed during exit() after QGuiApplication is gone. Config changes
	// are persisted via the debounced save timer during the app's lifetime.
	static Manager *instance = new Manager();
	return *instance;
}

Config &Manager::config() {
	return *_config;
}

GhostMode &Manager::ghostMode() {
	return *_ghostMode;
}

MessageArchive &Manager::archive() {
	return *_archive;
}

MediaArchive &Manager::mediaArchive() {
	return *_mediaArchive;
}

ChatBackground &Manager::chatBackground() {
	return *_chatBackground;
}

} // namespace Ghostgram
