/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/ghostgram_config.h"

#include "core/launcher.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <rpl/then.h>

namespace Ghostgram {
namespace {

constexpr auto kSchemaVersion = 1;
constexpr auto kSaveDebounceMs = crl::time(500);

} // namespace

Config::Config()
: _saveTimer([=] { save(); }) {
}

Config::~Config() = default;

QString Config::FilePath() {
	return cWorkingDir() + u"tdata/ghostgram_config.json"_q;
}

void Config::scheduleSave() {
	if (_loading) {
		return;
	}
	_saveTimer.callOnce(kSaveDebounceMs);
}

bool Config::ghostMode() const {
	return _ghostMode;
}

void Config::setGhostMode(bool value) {
	if (_ghostMode == value) {
		return;
	}
	_ghostMode = value;
	_ghostModeChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::ghostModeValue() const {
	return rpl::single(_ghostMode) | rpl::then(_ghostModeChanges.events());
}

bool Config::hideOnline() const {
	return _hideOnline;
}

void Config::setHideOnline(bool value) {
	if (_hideOnline == value) {
		return;
	}
	_hideOnline = value;
	_hideOnlineChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::hideOnlineValue() const {
	return rpl::single(_hideOnline) | rpl::then(_hideOnlineChanges.events());
}

bool Config::autoOffline() const {
	return _autoOffline;
}

void Config::setAutoOffline(bool value) {
	if (_autoOffline == value) {
		return;
	}
	_autoOffline = value;
	_autoOfflineChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::autoOfflineValue() const {
	return rpl::single(_autoOffline) | rpl::then(_autoOfflineChanges.events());
}

bool Config::suppressReadMessages() const {
	return _suppressReadMessages;
}

void Config::setSuppressReadMessages(bool value) {
	if (_suppressReadMessages == value) {
		return;
	}
	_suppressReadMessages = value;
	_suppressReadMessagesChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::suppressReadMessagesValue() const {
	return rpl::single(_suppressReadMessages)
		| rpl::then(_suppressReadMessagesChanges.events());
}

bool Config::suppressReadStories() const {
	return _suppressReadStories;
}

void Config::setSuppressReadStories(bool value) {
	if (_suppressReadStories == value) {
		return;
	}
	_suppressReadStories = value;
	_suppressReadStoriesChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::suppressReadStoriesValue() const {
	return rpl::single(_suppressReadStories)
		| rpl::then(_suppressReadStoriesChanges.events());
}

bool Config::suppressTyping() const {
	return _suppressTyping;
}

void Config::setSuppressTyping(bool value) {
	if (_suppressTyping == value) {
		return;
	}
	_suppressTyping = value;
	_suppressTypingChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::suppressTypingValue() const {
	return rpl::single(_suppressTyping)
		| rpl::then(_suppressTypingChanges.events());
}

bool Config::readOnActions() const {
	return _readOnActions;
}

void Config::setReadOnActions(bool value) {
	if (_readOnActions == value) {
		return;
	}
	_readOnActions = value;
	_readOnActionsChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::readOnActionsValue() const {
	return rpl::single(_readOnActions)
		| rpl::then(_readOnActionsChanges.events());
}

bool Config::ghostSendDelay() const {
	return _ghostSendDelay;
}

void Config::setGhostSendDelay(bool value) {
	if (_ghostSendDelay == value) {
		return;
	}
	_ghostSendDelay = value;
	_ghostSendDelayChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::ghostSendDelayValue() const {
	return rpl::single(_ghostSendDelay)
		| rpl::then(_ghostSendDelayChanges.events());
}

int Config::ghostTextDelaySec() const {
	return _ghostTextDelaySec;
}

void Config::setGhostTextDelaySec(int value) {
	value = std::clamp(value, 12, 60);
	if (_ghostTextDelaySec == value) {
		return;
	}
	_ghostTextDelaySec = value;
	_ghostTextDelaySecChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<int> Config::ghostTextDelaySecValue() const {
	return rpl::single(_ghostTextDelaySec)
		| rpl::then(_ghostTextDelaySecChanges.events());
}

int Config::ghostMediaDelaySec() const {
	return _ghostMediaDelaySec;
}

void Config::setGhostMediaDelaySec(int value) {
	value = std::clamp(value, 20, 60);
	if (_ghostMediaDelaySec == value) {
		return;
	}
	_ghostMediaDelaySec = value;
	_ghostMediaDelaySecChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<int> Config::ghostMediaDelaySecValue() const {
	return rpl::single(_ghostMediaDelaySec)
		| rpl::then(_ghostMediaDelaySecChanges.events());
}

bool Config::eternalOnline() const {
	return _eternalOnline;
}

void Config::setEternalOnline(bool value) {
	if (_eternalOnline == value) {
		return;
	}
	_eternalOnline = value;
	_eternalOnlineChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::eternalOnlineValue() const {
	return rpl::single(_eternalOnline)
		| rpl::then(_eternalOnlineChanges.events());
}

bool Config::saveDeletedMessages() const {
	return _saveDeletedMessages;
}

void Config::setSaveDeletedMessages(bool value) {
	if (_saveDeletedMessages == value) {
		return;
	}
	_saveDeletedMessages = value;
	_saveDeletedMessagesChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::saveDeletedMessagesValue() const {
	return rpl::single(_saveDeletedMessages)
		| rpl::then(_saveDeletedMessagesChanges.events());
}

bool Config::saveEditedMessages() const {
	return _saveEditedMessages;
}

void Config::setSaveEditedMessages(bool value) {
	if (_saveEditedMessages == value) {
		return;
	}
	_saveEditedMessages = value;
	_saveEditedMessagesChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::saveEditedMessagesValue() const {
	return rpl::single(_saveEditedMessages)
		| rpl::then(_saveEditedMessagesChanges.events());
}

bool Config::saveToDisk() const {
	return _saveToDisk;
}

void Config::setSaveToDisk(bool value) {
	if (_saveToDisk == value) {
		return;
	}
	_saveToDisk = value;
	_saveToDiskChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::saveToDiskValue() const {
	return rpl::single(_saveToDisk) | rpl::then(_saveToDiskChanges.events());
}

double Config::archiveOpacity() const {
	return _archiveOpacity;
}

void Config::setArchiveOpacity(double value) {
	value = std::clamp(value, 0.0, 1.0);
	if (qFuzzyCompare(_archiveOpacity, value)) {
		return;
	}
	_archiveOpacity = value;
	_archiveOpacityChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<double> Config::archiveOpacityValue() const {
	return rpl::single(_archiveOpacity)
		| rpl::then(_archiveOpacityChanges.events());
}

bool Config::saveEphemeralMedia() const {
	return _saveEphemeralMedia;
}

void Config::setSaveEphemeralMedia(bool value) {
	if (_saveEphemeralMedia == value) {
		return;
	}
	_saveEphemeralMedia = value;
	_saveEphemeralMediaChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::saveEphemeralMediaValue() const {
	return rpl::single(_saveEphemeralMedia)
		| rpl::then(_saveEphemeralMediaChanges.events());
}

bool Config::bypassProtectedContent() const {
	return _bypassProtectedContent;
}

void Config::setBypassProtectedContent(bool value) {
	if (_bypassProtectedContent == value) {
		return;
	}
	_bypassProtectedContent = value;
	_bypassProtectedContentChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::bypassProtectedContentValue() const {
	return rpl::single(_bypassProtectedContent)
		| rpl::then(_bypassProtectedContentChanges.events());
}

bool Config::blockAds() const {
	return _blockAds;
}

void Config::setBlockAds(bool value) {
	if (_blockAds == value) {
		return;
	}
	_blockAds = value;
	_blockAdsChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::blockAdsValue() const {
	return rpl::single(_blockAds) | rpl::then(_blockAdsChanges.events());
}

bool Config::hideReactions() const {
	return _hideReactions;
}

void Config::setHideReactions(bool value) {
	if (_hideReactions == value) {
		return;
	}
	_hideReactions = value;
	_hideReactionsChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::hideReactionsValue() const {
	return rpl::single(_hideReactions)
		| rpl::then(_hideReactionsChanges.events());
}

bool Config::hideComments() const {
	return _hideComments;
}

void Config::setHideComments(bool value) {
	if (_hideComments == value) {
		return;
	}
	_hideComments = value;
	_hideCommentsChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::hideCommentsValue() const {
	return rpl::single(_hideComments)
		| rpl::then(_hideCommentsChanges.events());
}

bool Config::squareUserpics() const {
	return _squareUserpics;
}

void Config::setSquareUserpics(bool value) {
	if (_squareUserpics == value) {
		return;
	}
	_squareUserpics = value;
	_squareUserpicsChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::squareUserpicsValue() const {
	return rpl::single(_squareUserpics)
		| rpl::then(_squareUserpicsChanges.events());
}

bool Config::antiCaps() const {
	return _antiCaps;
}

void Config::setAntiCaps(bool value) {
	if (_antiCaps == value) {
		return;
	}
	_antiCaps = value;
	_antiCapsChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::antiCapsValue() const {
	return rpl::single(_antiCaps) | rpl::then(_antiCapsChanges.events());
}

bool Config::fakePremium() const {
	return _fakePremium;
}

void Config::setFakePremium(bool value) {
	if (_fakePremium == value) {
		return;
	}
	_fakePremium = value;
	_fakePremiumChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::fakePremiumValue() const {
	return rpl::single(_fakePremium)
		| rpl::then(_fakePremiumChanges.events());
}

bool Config::fakeStars() const {
	return _fakeStars;
}

void Config::setFakeStars(bool value) {
	if (_fakeStars == value) {
		return;
	}
	_fakeStars = value;
	_fakeStarsChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::fakeStarsValue() const {
	return rpl::single(_fakeStars)
		| rpl::then(_fakeStarsChanges.events());
}

int64 Config::fakeStarsAmount() const {
	return _fakeStarsAmount;
}

void Config::setFakeStarsAmount(int64 value) {
	value = std::clamp<int64>(value, 0, 100000000);
	if (_fakeStarsAmount == value) {
		return;
	}
	_fakeStarsAmount = value;
	_fakeStarsAmountChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<int64> Config::fakeStarsAmountValue() const {
	return rpl::single(_fakeStarsAmount)
		| rpl::then(_fakeStarsAmountChanges.events());
}

bool Config::translateOutgoing() const {
	return _translateOutgoing;
}

void Config::setTranslateOutgoing(bool value) {
	if (_translateOutgoing == value) {
		return;
	}
	_translateOutgoing = value;
	_translateOutgoingChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<bool> Config::translateOutgoingValue() const {
	return rpl::single(_translateOutgoing)
		| rpl::then(_translateOutgoingChanges.events());
}

QString Config::translateOutgoingLang() const {
	return _translateOutgoingLang;
}

void Config::setTranslateOutgoingLang(const QString &lang) {
	if (_translateOutgoingLang == lang) {
		return;
	}
	_translateOutgoingLang = lang;
	_translateOutgoingLangChanges.fire_copy(lang);
	scheduleSave();
}

rpl::producer<QString> Config::translateOutgoingLangValue() const {
	return rpl::single(_translateOutgoingLang)
		| rpl::then(_translateOutgoingLangChanges.events());
}

AutoFormat Config::autoFormat() const {
	return _autoFormat;
}

void Config::setAutoFormat(AutoFormat value) {
	if (_autoFormat == value) {
		return;
	}
	_autoFormat = value;
	_autoFormatChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<AutoFormat> Config::autoFormatValue() const {
	return rpl::single(_autoFormat)
		| rpl::then(_autoFormatChanges.events());
}

QString Config::customFontPath() const {
	return _customFontPath;
}

QString Config::customFontFamily() const {
	return _customFontFamily;
}

void Config::setCustomFont(const QString &path, const QString &family) {
	if (_customFontPath == path && _customFontFamily == family) {
		return;
	}
	_customFontPath = path;
	_customFontFamily = family;
	_customFontFamilyChanges.fire_copy(family);
	scheduleSave();
}

void Config::clearCustomFont() {
	if (_customFontPath.isEmpty() && _customFontFamily.isEmpty()) {
		return;
	}
	_customFontPath.clear();
	_customFontFamily.clear();
	_customFontFamilyChanges.fire_copy(QString());
	scheduleSave();
}

rpl::producer<QString> Config::customFontFamilyValue() const {
	return rpl::single(_customFontFamily)
		| rpl::then(_customFontFamilyChanges.events());
}

QString Config::chatBackgroundMediaPath() const {
	return _chatBackgroundMediaPath;
}

void Config::setChatBackgroundMediaPath(const QString &path) {
	if (_chatBackgroundMediaPath == path) {
		return;
	}
	_chatBackgroundMediaPath = path;
	_chatBackgroundMediaPathChanges.fire_copy(path);
	scheduleSave();
}

rpl::producer<QString> Config::chatBackgroundMediaPathValue() const {
	return rpl::single(_chatBackgroundMediaPath)
		| rpl::then(_chatBackgroundMediaPathChanges.events());
}

double Config::chatBackgroundMediaOpacity() const {
	return _chatBackgroundMediaOpacity;
}

void Config::setChatBackgroundMediaOpacity(double value) {
	value = std::clamp(value, 0.0, 1.0);
	if (qFuzzyCompare(_chatBackgroundMediaOpacity, value)) {
		return;
	}
	_chatBackgroundMediaOpacity = value;
	_chatBackgroundMediaOpacityChanges.fire_copy(value);
	scheduleSave();
}

rpl::producer<double> Config::chatBackgroundMediaOpacityValue() const {
	return rpl::single(_chatBackgroundMediaOpacity)
		| rpl::then(_chatBackgroundMediaOpacityChanges.events());
}

void Config::load() {
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

	_loading = true;
	const auto take = [&](const char *key, bool &out) {
		const auto v = obj.value(QLatin1String(key));
		if (v.isBool()) {
			out = v.toBool();
		}
	};
	take("ghostMode", _ghostMode);
	take("hideOnline", _hideOnline);
	take("autoOffline", _autoOffline);
	take("suppressReadMessages", _suppressReadMessages);
	take("suppressReadStories", _suppressReadStories);
	take("suppressTyping", _suppressTyping);
	take("readOnActions", _readOnActions);
	take("ghostSendDelay", _ghostSendDelay);
	take("eternalOnline", _eternalOnline);
	{
		const auto v = obj.value(u"ghostTextDelaySec"_q);
		if (v.isDouble()) {
			_ghostTextDelaySec = std::clamp(v.toInt(12), 1, 120);
		}
	}
	{
		const auto v = obj.value(u"ghostMediaDelaySec"_q);
		if (v.isDouble()) {
			_ghostMediaDelaySec = std::clamp(v.toInt(20), 1, 180);
		}
	}
	take("saveDeletedMessages", _saveDeletedMessages);
	take("saveEditedMessages", _saveEditedMessages);
	take("saveToDisk", _saveToDisk);
	{
		const auto v = obj.value(u"archiveOpacity"_q);
		if (v.isDouble()) {
			_archiveOpacity = std::clamp(v.toDouble(), 0.0, 1.0);
		}
	}
	take("saveEphemeralMedia", _saveEphemeralMedia);
	take("bypassProtectedContent", _bypassProtectedContent);
	take("blockAds", _blockAds);
	take("hideReactions", _hideReactions);
	take("hideComments", _hideComments);
	take("squareUserpics", _squareUserpics);
	take("antiCaps", _antiCaps);
	take("fakePremium", _fakePremium);
	take("fakeStars", _fakeStars);
	{
		const auto v = obj.value(u"fakeStarsAmount"_q);
		if (v.isDouble()) {
			_fakeStarsAmount = std::clamp<int64>(
				int64(v.toDouble()), 0, 100000000);
		}
	}
	take("translateOutgoing", _translateOutgoing);
	{
		const auto v = obj.value(u"translateOutgoingLang"_q);
		if (v.isString() && !v.toString().isEmpty()) {
			_translateOutgoingLang = v.toString();
		}
	}
	{
		const auto v = obj.value(u"autoFormat"_q);
		if (v.isDouble()) {
			const auto i = v.toInt();
			if (i >= int(AutoFormat::None)
				&& i <= int(AutoFormat::Spoiler)) {
				_autoFormat = AutoFormat(i);
			}
		}
	}
	{
		const auto v = obj.value(u"customFontPath"_q);
		if (v.isString()) {
			_customFontPath = v.toString();
		}
		const auto f = obj.value(u"customFontFamily"_q);
		if (f.isString()) {
			_customFontFamily = f.toString();
		}
	}
	{
		const auto v = obj.value(u"chatBackgroundMediaPath"_q);
		if (v.isString()) {
			_chatBackgroundMediaPath = v.toString();
		}
		const auto o = obj.value(u"chatBackgroundMediaOpacity"_q);
		if (o.isDouble()) {
			_chatBackgroundMediaOpacity = std::clamp(o.toDouble(), 0.0, 1.0);
		}
	}
	_loading = false;
}

void Config::save() {
	_saveTimer.cancel();

	auto obj = QJsonObject();
	obj.insert(u"v"_q, kSchemaVersion);
	obj.insert(u"ghostMode"_q, _ghostMode);
	obj.insert(u"hideOnline"_q, _hideOnline);
	obj.insert(u"autoOffline"_q, _autoOffline);
	obj.insert(u"suppressReadMessages"_q, _suppressReadMessages);
	obj.insert(u"suppressReadStories"_q, _suppressReadStories);
	obj.insert(u"suppressTyping"_q, _suppressTyping);
	obj.insert(u"readOnActions"_q, _readOnActions);
	obj.insert(u"ghostSendDelay"_q, _ghostSendDelay);
	obj.insert(u"ghostTextDelaySec"_q, _ghostTextDelaySec);
	obj.insert(u"ghostMediaDelaySec"_q, _ghostMediaDelaySec);
	obj.insert(u"eternalOnline"_q, _eternalOnline);
	obj.insert(u"saveDeletedMessages"_q, _saveDeletedMessages);
	obj.insert(u"saveEditedMessages"_q, _saveEditedMessages);
	obj.insert(u"saveToDisk"_q, _saveToDisk);
	obj.insert(u"archiveOpacity"_q, _archiveOpacity);
	obj.insert(u"saveEphemeralMedia"_q, _saveEphemeralMedia);
	obj.insert(u"bypassProtectedContent"_q, _bypassProtectedContent);
	obj.insert(u"blockAds"_q, _blockAds);
	obj.insert(u"hideReactions"_q, _hideReactions);
	obj.insert(u"hideComments"_q, _hideComments);
	obj.insert(u"squareUserpics"_q, _squareUserpics);
	obj.insert(u"antiCaps"_q, _antiCaps);
	obj.insert(u"fakePremium"_q, _fakePremium);
	obj.insert(u"fakeStars"_q, _fakeStars);
	obj.insert(u"fakeStarsAmount"_q, double(_fakeStarsAmount));
	obj.insert(u"translateOutgoing"_q, _translateOutgoing);
	obj.insert(u"translateOutgoingLang"_q, _translateOutgoingLang);
	obj.insert(u"autoFormat"_q, int(_autoFormat));
	obj.insert(u"customFontPath"_q, _customFontPath);
	obj.insert(u"customFontFamily"_q, _customFontFamily);
	obj.insert(u"chatBackgroundMediaPath"_q, _chatBackgroundMediaPath);
	obj.insert(u"chatBackgroundMediaOpacity"_q, _chatBackgroundMediaOpacity);

	const auto path = FilePath();
	QDir().mkpath(QFileInfo(path).absolutePath());

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return;
	}
	file.write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
	file.close();
}

} // namespace Ghostgram
