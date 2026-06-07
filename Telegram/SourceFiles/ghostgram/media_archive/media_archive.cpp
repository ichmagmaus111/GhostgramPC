/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/media_archive/media_archive.h"

#include "base/unixtime.h"
#include "core/launcher.h"
#include "ghostgram/ghostgram_config.h"

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

MediaArchive::MediaArchive(not_null<Config*> config)
: _config(config)
, _saveTimer([=] { save(); }) {
	load();
}

MediaArchive::~MediaArchive() {
	save();
}

QString MediaArchive::MetaFilePath() {
	return cWorkingDir() + u"tdata/ghostgram_media_archive.json"_q;
}

QString MediaArchive::MediaDirPath() {
	return cWorkingDir() + u"tdata/ghostgram_media"_q;
}

void MediaArchive::noteEphemeralMessage(
		uint64 peerId,
		int64 messageId,
		uint64 fromUserId,
		const QString &mediaType) {
	const auto key = Key(peerId, messageId);
	if (_idIndex.contains(key)) {
		return;
	}
	_records.push_back(EphemeralMediaRecord{
		.peerId = peerId,
		.messageId = messageId,
		.fromUserId = fromUserId,
		.mediaType = mediaType,
		.savedAt = base::unixtime::now(),
	});
	_idIndex.emplace(key);
	_byKey[key] = _records.size() - 1;
	if (_config->saveToDisk()) {
		scheduleSave();
	}
}

void MediaArchive::recordFileSaved(
		uint64 peerId,
		int64 messageId,
		const QString &sourcePath) {
	const auto key = Key(peerId, messageId);
	const auto it = _byKey.find(key);
	if (it == _byKey.end() || sourcePath.isEmpty()) {
		return;
	}
	auto &record = _records[it->second];
	if (!record.fileName.isEmpty()) {
		return;
	}
	const auto suffix = QFileInfo(sourcePath).suffix();
	auto fileName = QString::number(peerId)
		+ u"_"_q
		+ QString::number(messageId);
	if (!suffix.isEmpty()) {
		fileName += '.' + suffix;
	}
	const auto dir = MediaDirPath();
	QDir().mkpath(dir);
	const auto dst = dir + '/' + fileName;
	if (QFile::exists(dst)) {
		QFile::remove(dst);
	}
	if (!QFile::copy(sourcePath, dst)) {
		return;
	}
	record.fileName = fileName;
	if (_config->saveToDisk()) {
		scheduleSave();
	}
}

bool MediaArchive::contains(uint64 peerId, int64 messageId) const {
	return _idIndex.contains(Key(peerId, messageId));
}

bool MediaArchive::hasFile(uint64 peerId, int64 messageId) const {
	const auto it = _byKey.find(Key(peerId, messageId));
	return (it != _byKey.end())
		&& !_records[it->second].fileName.isEmpty();
}

std::vector<EphemeralMediaRecord> MediaArchive::all() const {
	return _records;
}

QString MediaArchive::filePathFor(uint64 peerId, int64 messageId) const {
	const auto it = _byKey.find(Key(peerId, messageId));
	if (it == _byKey.end()) {
		return QString();
	}
	const auto &name = _records[it->second].fileName;
	return name.isEmpty() ? QString() : (MediaDirPath() + '/' + name);
}

void MediaArchive::scheduleSave() {
	_saveTimer.callOnce(kSaveDebounceMs);
}

void MediaArchive::save() {
	_saveTimer.cancel();

	if (!_config->saveToDisk()) {
		QFile::remove(MetaFilePath());
		return;
	}

	auto arr = QJsonArray();
	for (const auto &record : _records) {
		auto obj = QJsonObject();
		obj.insert(u"peer"_q, QString::number(record.peerId));
		obj.insert(u"msg"_q, QString::number(record.messageId));
		obj.insert(u"from"_q, QString::number(record.fromUserId));
		obj.insert(u"type"_q, record.mediaType);
		obj.insert(u"file"_q, record.fileName);
		obj.insert(u"at"_q, QString::number(record.savedAt));
		arr.append(obj);
	}

	auto root = QJsonObject();
	root.insert(u"v"_q, kSchemaVersion);
	root.insert(u"media"_q, arr);

	const auto path = MetaFilePath();
	QDir().mkpath(QFileInfo(path).absolutePath());

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return;
	}
	file.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
	file.close();
}

void MediaArchive::load() {
	QFile file(MetaFilePath());
	if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
		return;
	}
	const auto bytes = file.readAll();
	file.close();

	const auto doc = QJsonDocument::fromJson(bytes);
	if (!doc.isObject()) {
		return;
	}
	const auto arr = doc.object().value(u"media"_q).toArray();
	for (const auto &v : arr) {
		const auto obj = v.toObject();
		auto record = EphemeralMediaRecord{
			.peerId = obj.value(u"peer"_q).toString().toULongLong(),
			.messageId = obj.value(u"msg"_q).toString().toLongLong(),
			.fromUserId = obj.value(u"from"_q).toString().toULongLong(),
			.mediaType = obj.value(u"type"_q).toString(),
			.fileName = obj.value(u"file"_q).toString(),
			.savedAt = obj.value(u"at"_q).toString().toLongLong(),
		};
		if (!record.peerId || !record.messageId) {
			continue;
		}
		const auto key = Key(record.peerId, record.messageId);
		_records.push_back(std::move(record));
		_idIndex.emplace(key);
		_byKey[key] = _records.size() - 1;
	}
}

} // namespace Ghostgram
