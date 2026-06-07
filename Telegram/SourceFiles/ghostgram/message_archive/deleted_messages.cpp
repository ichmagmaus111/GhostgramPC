/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/message_archive/deleted_messages.h"

#include "base/unixtime.h"
#include "data/data_peer.h"
#include "history/history.h"
#include "history/history_item.h"

#include <QtCore/QJsonObject>

namespace Ghostgram {

DeletedMessages::DeletedMessages() = default;

DeletedMessages::~DeletedMessages() = default;

void DeletedMessages::hookDelete(not_null<HistoryItem*> item) {
	const auto peerId = item->history()->peer->id.value;
	const auto messageId = item->id.bare;
	const auto key = std::pair<uint64, int64>(peerId, messageId);
	if (_idIndex.contains(key)) {
		return;
	}
	auto record = DeletedMessage{
		.peerId = peerId,
		.messageId = messageId,
		.text = item->originalText().text,
		.deletedAt = base::unixtime::now(),
		.fromUserId = item->from()->id.value,
	};
	_byPeer[peerId].push_back(std::move(record));
	_idIndex.emplace(key);
}

bool DeletedMessages::contains(uint64 peerId, int64 messageId) const {
	return _idIndex.contains(std::pair<uint64, int64>(peerId, messageId));
}

const DeletedMessage *DeletedMessages::find(
		uint64 peerId,
		int64 messageId) const {
	const auto it = _byPeer.find(peerId);
	if (it == _byPeer.end()) {
		return nullptr;
	}
	for (const auto &msg : it->second) {
		if (msg.messageId == messageId) {
			return &msg;
		}
	}
	return nullptr;
}

std::vector<DeletedMessage> DeletedMessages::forPeer(uint64 peerId) const {
	const auto it = _byPeer.find(peerId);
	return (it != _byPeer.end()) ? it->second : std::vector<DeletedMessage>();
}

std::vector<DeletedMessage> DeletedMessages::all() const {
	auto result = std::vector<DeletedMessage>();
	for (const auto &[peerId, list] : _byPeer) {
		result.insert(result.end(), list.begin(), list.end());
	}
	return result;
}

void DeletedMessages::clear() {
	_byPeer.clear();
	_idIndex.clear();
}

void DeletedMessages::clearForPeer(uint64 peerId) {
	const auto it = _byPeer.find(peerId);
	if (it == _byPeer.end()) {
		return;
	}
	for (const auto &msg : it->second) {
		_idIndex.remove(std::pair<uint64, int64>(msg.peerId, msg.messageId));
	}
	_byPeer.remove(peerId);
}

QJsonArray DeletedMessages::serialize() const {
	auto arr = QJsonArray();
	for (const auto &[peerId, list] : _byPeer) {
		for (const auto &msg : list) {
			auto obj = QJsonObject();
			obj.insert(u"peer"_q, QString::number(msg.peerId));
			obj.insert(u"msg"_q, QString::number(msg.messageId));
			obj.insert(u"from"_q, QString::number(msg.fromUserId));
			obj.insert(u"at"_q, QString::number(msg.deletedAt));
			obj.insert(u"text"_q, msg.text);
			arr.append(obj);
		}
	}
	return arr;
}

void DeletedMessages::deserialize(const QJsonArray &arr) {
	_byPeer.clear();
	_idIndex.clear();
	for (const auto &v : arr) {
		const auto obj = v.toObject();
		auto record = DeletedMessage{
			.peerId = obj.value(u"peer"_q).toString().toULongLong(),
			.messageId = obj.value(u"msg"_q).toString().toLongLong(),
			.text = obj.value(u"text"_q).toString(),
			.deletedAt = obj.value(u"at"_q).toString().toLongLong(),
			.fromUserId = obj.value(u"from"_q).toString().toULongLong(),
		};
		if (!record.peerId || !record.messageId) {
			continue;
		}
		const auto key = std::pair<uint64, int64>(record.peerId, record.messageId);
		_byPeer[record.peerId].push_back(std::move(record));
		_idIndex.emplace(key);
	}
}

} // namespace Ghostgram
