/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/basic_types.h"

#include <memory>

namespace Ghostgram {

class Config;
class GhostMode;
class MessageArchive;
class MediaArchive;
class ChatBackground;

class Manager final {
public:
	Manager();
	~Manager();

	Manager(const Manager &) = delete;
	Manager &operator=(const Manager &) = delete;

	[[nodiscard]] static Manager &Instance();

	[[nodiscard]] Config &config();
	[[nodiscard]] GhostMode &ghostMode();
	[[nodiscard]] MessageArchive &archive();
	[[nodiscard]] MediaArchive &mediaArchive();
	[[nodiscard]] ChatBackground &chatBackground();

private:
	std::unique_ptr<Config> _config;
	std::unique_ptr<GhostMode> _ghostMode;
	std::unique_ptr<MessageArchive> _archive;
	std::unique_ptr<MediaArchive> _mediaArchive;
	std::unique_ptr<ChatBackground> _chatBackground;

};

} // namespace Ghostgram
