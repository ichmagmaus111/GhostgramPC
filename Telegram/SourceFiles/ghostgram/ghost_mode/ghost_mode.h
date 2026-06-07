/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/basic_types.h"

namespace Ghostgram {

class Config;

class GhostMode final {
public:
	explicit GhostMode(not_null<Config*> config);
	~GhostMode();

	GhostMode(const GhostMode &) = delete;
	GhostMode &operator=(const GhostMode &) = delete;

	[[nodiscard]] bool isActive() const;

	[[nodiscard]] bool shouldHideOnline() const;
	[[nodiscard]] bool shouldForceOffline() const;
	[[nodiscard]] bool shouldSuppressReadHistory() const;
	[[nodiscard]] bool shouldSuppressReadStories() const;
	[[nodiscard]] bool shouldSuppressTyping() const;

	[[nodiscard]] bool shouldSuppressReadReceipts() const;

	// RAII scope that marks the current thread as performing an explicit
	// user action (send, reply, react). While any scope is alive AND the
	// "read on actions" toggle is on, `shouldSuppressReadHistory()` returns
	// false to allow the read marker to flow through.
	class ActionScope final {
	public:
		ActionScope();
		~ActionScope();

		ActionScope(const ActionScope &) = delete;
		ActionScope &operator=(const ActionScope &) = delete;
	};

	[[nodiscard]] bool inUserAction() const;

private:
	const not_null<Config*> _config;

};

} // namespace Ghostgram
