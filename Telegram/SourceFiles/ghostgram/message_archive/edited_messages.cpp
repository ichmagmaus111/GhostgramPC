/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/message_archive/edited_messages.h"

#include "base/unixtime.h"
#include "data/data_peer.h"
#include "history/history.h"
#include "history/history_item.h"

#include <QtCore/QJsonObject>

namespace Ghostgram {

EditedMessages::EditedMessages() = default;

EditedMessages::~EditedMessages() = default;

void EditedMessages::hookEdit(
		not_null<HistoryItem*> item,
		const QString &previousText) {
	const auto peerId = item->history()->peer->id.value;
	const auto messageId = item->id.bare;
	const auto key = Key(peerId, messageId);
	auto &history = _byMessage[key];
	if (history.peerId == 0) {
		history.peerId = peerId;
		history.messageId = messageId;
	}
	history.revisions.push_back(MessageEdit{
		.text = previousText,
		.editedAt = base::unixtime::now(),
	});
}

MessageEditHistory EditedMessages::historyFor(
		uint64 peerId,
		int64 messageId) const {
	const auto it = _byMessage.find(Key(peerId, messageId));
	return (it != _byMessage.end())
		? it->second
		: MessageEditHistory{ .peerId = peerId, .messageId = messageId };
}

bool EditedMessages::hasEdits(uint64 peerId, int64 messageId) const {
	const auto it = _byMessage.find(Key(peerId, messageId));
	return (it != _byMessage.end()) && !it->second.revisions.empty();
}

QString EditedMessages::previousText(
		uint64 peerId,
		int64 messageId) const {
	const auto it = _byMessage.find(Key(peerId, messageId));
	if (it == _byMessage.end() || it->second.revisions.empty()) {
		return QString();
	}
	return it->second.revisions.back().text;
}

void EditedMessages::clear() {
	_byMessage.clear();
}

void EditedMessages::clearForMessage(uint64 peerId, int64 messageId) {
	_byMessage.remove(Key(peerId, messageId));
}

QJsonArray EditedMessages::serialize() const {
	auto arr = QJsonArray();
	for (const auto &[key, history] : _byMessage) {
		auto revisionsArr = QJsonArray();
		for (const auto &rev : history.revisions) {
			auto revObj = QJsonObject();
			revObj.insert(u"text"_q, rev.text);
			revObj.insert(u"at"_q, QString::number(rev.editedAt));
			revisionsArr.append(revObj);
		}
		auto obj = QJsonObject();
		obj.insert(u"peer"_q, QString::number(history.peerId));
		obj.insert(u"msg"_q, QString::number(history.messageId));
		obj.insert(u"revs"_q, revisionsArr);
		arr.append(obj);
	}
	return arr;
}

void EditedMessages::deserialize(const QJsonArray &arr) {
	_byMessage.clear();
	for (const auto &v : arr) {
		const auto obj = v.toObject();
		auto history = MessageEditHistory{
			.peerId = obj.value(u"peer"_q).toString().toULongLong(),
			.messageId = obj.value(u"msg"_q).toString().toLongLong(),
		};
		if (!history.peerId || !history.messageId) {
			continue;
		}
		const auto revs = obj.value(u"revs"_q).toArray();
		for (const auto &rv : revs) {
			const auto ro = rv.toObject();
			history.revisions.push_back(MessageEdit{
				.text = ro.value(u"text"_q).toString(),
				.editedAt = ro.value(u"at"_q).toString().toLongLong(),
			});
		}
		_byMessage[Key(history.peerId, history.messageId)] = std::move(history);
	}
}

} // namespace Ghostgram
