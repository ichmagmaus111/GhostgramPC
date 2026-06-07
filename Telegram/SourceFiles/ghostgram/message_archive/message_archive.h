/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/basic_types.h"
#include "base/timer.h"

#include <memory>

class HistoryItem;
class QString;

namespace Ghostgram {

class Config;
class DeletedMessages;
class EditedMessages;

class MessageArchive final {
public:
	explicit MessageArchive(not_null<Config*> config);
	~MessageArchive();

	MessageArchive(const MessageArchive &) = delete;
	MessageArchive &operator=(const MessageArchive &) = delete;

	[[nodiscard]] DeletedMessages &deleted();
	[[nodiscard]] EditedMessages &edited();

	[[nodiscard]] bool preserveOnServerDelete(
		not_null<HistoryItem*> item) const;
	[[nodiscard]] bool shouldRecordEdit(
		not_null<HistoryItem*> item) const;

	void notifyDelete(not_null<HistoryItem*> item);
	void notifyEdit(
		not_null<HistoryItem*> item,
		const QString &previousText);

	void scheduleSave();
	void save();
	void load();

private:
	[[nodiscard]] static QString FilePath();

	const not_null<Config*> _config;
	std::unique_ptr<DeletedMessages> _deleted;
	std::unique_ptr<EditedMessages> _edited;
	base::Timer _saveTimer;

};

} // namespace Ghostgram
