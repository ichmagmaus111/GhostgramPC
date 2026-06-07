/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/basic_types.h"
#include "base/flat_map.h"
#include "base/flat_set.h"
#include "base/timer.h"

#include <QString>
#include <QtCore/QJsonArray>
#include <vector>

namespace Ghostgram {

class Config;

struct EphemeralMediaRecord {
	uint64 peerId = 0;
	int64 messageId = 0;
	uint64 fromUserId = 0;
	QString mediaType;
	QString fileName;
	crl::time savedAt = 0;
};

class MediaArchive final {
public:
	explicit MediaArchive(not_null<Config*> config);
	~MediaArchive();

	MediaArchive(const MediaArchive &) = delete;
	MediaArchive &operator=(const MediaArchive &) = delete;

	void noteEphemeralMessage(
		uint64 peerId,
		int64 messageId,
		uint64 fromUserId,
		const QString &mediaType);

	void recordFileSaved(
		uint64 peerId,
		int64 messageId,
		const QString &sourcePath);

	[[nodiscard]] bool contains(uint64 peerId, int64 messageId) const;
	[[nodiscard]] bool hasFile(uint64 peerId, int64 messageId) const;

	[[nodiscard]] std::vector<EphemeralMediaRecord> all() const;
	[[nodiscard]] QString filePathFor(
		uint64 peerId,
		int64 messageId) const;

	void scheduleSave();
	void save();
	void load();

private:
	using Key = std::pair<uint64, int64>;

	[[nodiscard]] static QString MetaFilePath();
	[[nodiscard]] static QString MediaDirPath();

	const not_null<Config*> _config;
	std::vector<EphemeralMediaRecord> _records;
	base::flat_set<Key> _idIndex;
	base::flat_map<Key, std::size_t> _byKey;
	base::Timer _saveTimer;

};

} // namespace Ghostgram
