/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/basic_types.h"
#include "base/flat_map.h"
#include "base/flat_set.h"

#include <QString>
#include <QtCore/QJsonArray>
#include <vector>

class HistoryItem;
class PeerData;

namespace Ghostgram {

struct DeletedMessage {
	uint64 peerId = 0;
	int64 messageId = 0;
	QString text;
	crl::time deletedAt = 0;
	uint64 fromUserId = 0;
};

class DeletedMessages final {
public:
	DeletedMessages();
	~DeletedMessages();

	DeletedMessages(const DeletedMessages &) = delete;
	DeletedMessages &operator=(const DeletedMessages &) = delete;

	void hookDelete(not_null<HistoryItem*> item);

	[[nodiscard]] bool contains(uint64 peerId, int64 messageId) const;
	[[nodiscard]] const DeletedMessage *find(
		uint64 peerId,
		int64 messageId) const;
	[[nodiscard]] std::vector<DeletedMessage> forPeer(uint64 peerId) const;
	[[nodiscard]] std::vector<DeletedMessage> all() const;

	void clear();
	void clearForPeer(uint64 peerId);

	[[nodiscard]] QJsonArray serialize() const;
	void deserialize(const QJsonArray &arr);

private:
	base::flat_map<uint64, std::vector<DeletedMessage>> _byPeer;
	base::flat_set<std::pair<uint64, int64>> _idIndex;

};

} // namespace Ghostgram
