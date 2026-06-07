/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/message_archive/message_archive.h"

#include "core/launcher.h"
#include "ghostgram/ghostgram_config.h"
#include "ghostgram/message_archive/deleted_messages.h"
#include "ghostgram/message_archive/edited_messages.h"
#include "history/history_item.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Ghostgram {
namespace {

constexpr auto kSchemaVersion = 1;
constexpr auto kSaveDebounceMs = crl::time(500);

} // namespace

MessageArchive::MessageArchive(not_null<Config*> config)
: _config(config)
, _deleted(std::make_unique<DeletedMessages>())
, _edited(std::make_unique<EditedMessages>())
, _saveTimer([=] { save(); }) {
	load();
}

MessageArchive::~MessageArchive() {
	save();
}

QString MessageArchive::FilePath() {
	return cWorkingDir() + u"tdata/ghostgram_archive.json"_q;
}

DeletedMessages &MessageArchive::deleted() {
	return *_deleted;
}

EditedMessages &MessageArchive::edited() {
	return *_edited;
}

bool MessageArchive::preserveOnServerDelete(
		not_null<HistoryItem*> item) const {
	return _config->saveDeletedMessages() && item->isRegular();
}

bool MessageArchive::shouldRecordEdit(
		not_null<HistoryItem*> item) const {
	return _config->saveEditedMessages() && item->isRegular();
}

void MessageArchive::notifyDelete(not_null<HistoryItem*> item) {
	if (!preserveOnServerDelete(item)) {
		return;
	}
	_deleted->hookDelete(item);
	if (_config->saveToDisk()) {
		scheduleSave();
	}
}

void MessageArchive::notifyEdit(
		not_null<HistoryItem*> item,
		const QString &previousText) {
	if (!shouldRecordEdit(item)) {
		return;
	}
	_edited->hookEdit(item, previousText);
	if (_config->saveToDisk()) {
		scheduleSave();
	}
}

void MessageArchive::scheduleSave() {
	_saveTimer.callOnce(kSaveDebounceMs);
}

void MessageArchive::save() {
	_saveTimer.cancel();

	if (!_config->saveToDisk()) {
		QFile::remove(FilePath());
		return;
	}

	auto obj = QJsonObject();
	obj.insert(u"v"_q, kSchemaVersion);
	obj.insert(u"deleted"_q, _deleted->serialize());
	obj.insert(u"edited"_q, _edited->serialize());

	const auto path = FilePath();
	QDir().mkpath(QFileInfo(path).absolutePath());

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return;
	}
	file.write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
	file.close();
}

void MessageArchive::load() {
	QFile file(FilePath());
	if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
		return;
	}
	const auto bytes = file.readAll();
	file.close();

	const auto doc = QJsonDocument::fromJson(bytes);
	if (!doc.isObject()) {
		return;
	}
	const auto obj = doc.object();
	_deleted->deserialize(obj.value(u"deleted"_q).toArray());
	_edited->deserialize(obj.value(u"edited"_q).toArray());
}

} // namespace Ghostgram
