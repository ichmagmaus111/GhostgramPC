/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/ghost_mode/ghost_mode.h"

#include "ghostgram/ghostgram_config.h"

namespace Ghostgram {
namespace {

thread_local int g_actionDepth = 0;

} // namespace

GhostMode::ActionScope::ActionScope() {
	++g_actionDepth;
}

GhostMode::ActionScope::~ActionScope() {
	--g_actionDepth;
}

GhostMode::GhostMode(not_null<Config*> config)
: _config(config) {
}

GhostMode::~GhostMode() = default;

bool GhostMode::isActive() const {
	return _config->ghostMode();
}

bool GhostMode::inUserAction() const {
	return (g_actionDepth > 0) && _config->readOnActions();
}

bool GhostMode::shouldHideOnline() const {
	return isActive() && _config->hideOnline();
}

bool GhostMode::shouldForceOffline() const {
	return isActive() && _config->autoOffline();
}

bool GhostMode::shouldSuppressReadHistory() const {
	return isActive()
		&& _config->suppressReadMessages()
		&& !inUserAction();
}

bool GhostMode::shouldSuppressReadStories() const {
	return isActive() && _config->suppressReadStories();
}

bool GhostMode::shouldSuppressTyping() const {
	return isActive() && _config->suppressTyping();
}

bool GhostMode::shouldSuppressReadReceipts() const {
	return shouldSuppressReadHistory();
}

} // namespace Ghostgram
