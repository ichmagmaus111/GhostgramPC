/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/basic_types.h"
#include "base/flat_map.h"

#include <QString>
#include <QtCore/QJsonArray>
#include <vector>

class HistoryItem;

namespace Ghostgram {

struct MessageEdit {
	QString text;
	crl::time editedAt = 0;
};

struct MessageEditHistory {
	uint64 peerId = 0;
	int64 messageId = 0;
	std::vector<MessageEdit> revisions;
};

class EditedMessages final {
public:
	EditedMessages();
	~EditedMessages();

	EditedMessages(const EditedMessages &) = delete;
	EditedMessages &operator=(const EditedMessages &) = delete;

	void hookEdit(
		not_null<HistoryItem*> item,
		const QString &previousText);

	[[nodiscard]] MessageEditHistory historyFor(
		uint64 peerId,
		int64 messageId) const;

	[[nodiscard]] bool hasEdits(uint64 peerId, int64 messageId) const;
	[[nodiscard]] QString previousText(
		uint64 peerId,
		int64 messageId) const;

	void clear();
	void clearForMessage(uint64 peerId, int64 messageId);

	[[nodiscard]] QJsonArray serialize() const;
	void deserialize(const QJsonArray &arr);

private:
	using Key = std::pair<uint64, int64>;
	base::flat_map<Key, MessageEditHistory> _byMessage;

};

} // namespace Ghostgram
