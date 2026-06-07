/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/timer.h"

#include <rpl/event_stream.h>
#include <rpl/producer.h>

#include <QtCore/QString>

namespace Ghostgram {

enum class AutoFormat : int {
	None = 0,
	Bold = 1,
	Italic = 2,
	Underline = 3,
	Strike = 4,
	Mono = 5,
	Quote = 6,
	Spoiler = 7,
};

class Config final {
public:
	Config();
	~Config();

	Config(const Config &) = delete;
	Config &operator=(const Config &) = delete;

	[[nodiscard]] bool ghostMode() const;
	void setGhostMode(bool value);
	[[nodiscard]] rpl::producer<bool> ghostModeValue() const;

	[[nodiscard]] bool hideOnline() const;
	void setHideOnline(bool value);
	[[nodiscard]] rpl::producer<bool> hideOnlineValue() const;

	[[nodiscard]] bool autoOffline() const;
	void setAutoOffline(bool value);
	[[nodiscard]] rpl::producer<bool> autoOfflineValue() const;

	[[nodiscard]] bool suppressReadMessages() const;
	void setSuppressReadMessages(bool value);
	[[nodiscard]] rpl::producer<bool> suppressReadMessagesValue() const;

	[[nodiscard]] bool suppressReadStories() const;
	void setSuppressReadStories(bool value);
	[[nodiscard]] rpl::producer<bool> suppressReadStoriesValue() const;

	[[nodiscard]] bool suppressTyping() const;
	void setSuppressTyping(bool value);
	[[nodiscard]] rpl::producer<bool> suppressTypingValue() const;

	[[nodiscard]] bool readOnActions() const;
	void setReadOnActions(bool value);
	[[nodiscard]] rpl::producer<bool> readOnActionsValue() const;

	[[nodiscard]] bool ghostSendDelay() const;
	void setGhostSendDelay(bool value);
	[[nodiscard]] rpl::producer<bool> ghostSendDelayValue() const;

	[[nodiscard]] int ghostTextDelaySec() const;
	void setGhostTextDelaySec(int value);
	[[nodiscard]] rpl::producer<int> ghostTextDelaySecValue() const;

	[[nodiscard]] int ghostMediaDelaySec() const;
	void setGhostMediaDelaySec(int value);
	[[nodiscard]] rpl::producer<int> ghostMediaDelaySecValue() const;

	[[nodiscard]] bool eternalOnline() const;
	void setEternalOnline(bool value);
	[[nodiscard]] rpl::producer<bool> eternalOnlineValue() const;

	[[nodiscard]] bool saveDeletedMessages() const;
	void setSaveDeletedMessages(bool value);
	[[nodiscard]] rpl::producer<bool> saveDeletedMessagesValue() const;

	[[nodiscard]] bool saveEditedMessages() const;
	void setSaveEditedMessages(bool value);
	[[nodiscard]] rpl::producer<bool> saveEditedMessagesValue() const;

	[[nodiscard]] bool saveToDisk() const;
	void setSaveToDisk(bool value);
	[[nodiscard]] rpl::producer<bool> saveToDiskValue() const;

	[[nodiscard]] double archiveOpacity() const;
	void setArchiveOpacity(double value);
	[[nodiscard]] rpl::producer<double> archiveOpacityValue() const;

	[[nodiscard]] bool saveEphemeralMedia() const;
	void setSaveEphemeralMedia(bool value);
	[[nodiscard]] rpl::producer<bool> saveEphemeralMediaValue() const;

	[[nodiscard]] bool bypassProtectedContent() const;
	void setBypassProtectedContent(bool value);
	[[nodiscard]] rpl::producer<bool> bypassProtectedContentValue() const;

	[[nodiscard]] bool blockAds() const;
	void setBlockAds(bool value);
	[[nodiscard]] rpl::producer<bool> blockAdsValue() const;

	[[nodiscard]] bool hideReactions() const;
	void setHideReactions(bool value);
	[[nodiscard]] rpl::producer<bool> hideReactionsValue() const;

	[[nodiscard]] bool hideComments() const;
	void setHideComments(bool value);
	[[nodiscard]] rpl::producer<bool> hideCommentsValue() const;

	[[nodiscard]] bool squareUserpics() const;
	void setSquareUserpics(bool value);
	[[nodiscard]] rpl::producer<bool> squareUserpicsValue() const;

	[[nodiscard]] bool antiCaps() const;
	void setAntiCaps(bool value);
	[[nodiscard]] rpl::producer<bool> antiCapsValue() const;

	[[nodiscard]] bool fakePremium() const;
	void setFakePremium(bool value);
	[[nodiscard]] rpl::producer<bool> fakePremiumValue() const;

	[[nodiscard]] bool fakeStars() const;
	void setFakeStars(bool value);
	[[nodiscard]] rpl::producer<bool> fakeStarsValue() const;

	[[nodiscard]] int64 fakeStarsAmount() const;
	void setFakeStarsAmount(int64 value);
	[[nodiscard]] rpl::producer<int64> fakeStarsAmountValue() const;

	[[nodiscard]] bool translateOutgoing() const;
	void setTranslateOutgoing(bool value);
	[[nodiscard]] rpl::producer<bool> translateOutgoingValue() const;

	[[nodiscard]] QString translateOutgoingLang() const;
	void setTranslateOutgoingLang(const QString &lang);
	[[nodiscard]] rpl::producer<QString> translateOutgoingLangValue() const;

	[[nodiscard]] AutoFormat autoFormat() const;
	void setAutoFormat(AutoFormat value);
	[[nodiscard]] rpl::producer<AutoFormat> autoFormatValue() const;

	[[nodiscard]] QString customFontPath() const;
	[[nodiscard]] QString customFontFamily() const;
	void setCustomFont(const QString &path, const QString &family);
	void clearCustomFont();
	[[nodiscard]] rpl::producer<QString> customFontFamilyValue() const;

	[[nodiscard]] QString chatBackgroundMediaPath() const;
	void setChatBackgroundMediaPath(const QString &path);
	[[nodiscard]] rpl::producer<QString> chatBackgroundMediaPathValue() const;

	[[nodiscard]] double chatBackgroundMediaOpacity() const;
	void setChatBackgroundMediaOpacity(double value);
	[[nodiscard]] rpl::producer<double> chatBackgroundMediaOpacityValue() const;

	void load();
	void save();

private:
	void scheduleSave();
	[[nodiscard]] static QString FilePath();

	bool _ghostMode = false;
	bool _hideOnline = false;
	bool _autoOffline = false;
	bool _suppressReadMessages = false;
	bool _suppressReadStories = false;
	bool _suppressTyping = false;
	bool _readOnActions = false;
	bool _ghostSendDelay = false;
	int _ghostTextDelaySec = 12;
	int _ghostMediaDelaySec = 20;
	bool _eternalOnline = false;
	bool _saveDeletedMessages = true;
	bool _saveEditedMessages = true;
	bool _saveToDisk = true;
	double _archiveOpacity = 0.5;
	bool _saveEphemeralMedia = true;
	bool _bypassProtectedContent = false;
	bool _blockAds = true;
	bool _hideReactions = false;
	bool _hideComments = false;
	bool _squareUserpics = false;
	bool _antiCaps = false;
	bool _fakePremium = false;
	bool _fakeStars = false;
	int64 _fakeStarsAmount = 1000;
	bool _translateOutgoing = false;
	QString _translateOutgoingLang = u"en"_q;
	AutoFormat _autoFormat = AutoFormat::None;
	QString _customFontPath;
	QString _customFontFamily;
	QString _chatBackgroundMediaPath;
	double _chatBackgroundMediaOpacity = 1.0;

	rpl::event_stream<bool> _ghostModeChanges;
	rpl::event_stream<bool> _hideOnlineChanges;
	rpl::event_stream<bool> _autoOfflineChanges;
	rpl::event_stream<bool> _suppressReadMessagesChanges;
	rpl::event_stream<bool> _suppressReadStoriesChanges;
	rpl::event_stream<bool> _suppressTypingChanges;
	rpl::event_stream<bool> _readOnActionsChanges;
	rpl::event_stream<bool> _ghostSendDelayChanges;
	rpl::event_stream<int> _ghostTextDelaySecChanges;
	rpl::event_stream<int> _ghostMediaDelaySecChanges;
	rpl::event_stream<bool> _eternalOnlineChanges;
	rpl::event_stream<bool> _saveDeletedMessagesChanges;
	rpl::event_stream<bool> _saveEditedMessagesChanges;
	rpl::event_stream<bool> _saveToDiskChanges;
	rpl::event_stream<double> _archiveOpacityChanges;
	rpl::event_stream<bool> _saveEphemeralMediaChanges;
	rpl::event_stream<bool> _bypassProtectedContentChanges;
	rpl::event_stream<bool> _blockAdsChanges;
	rpl::event_stream<bool> _hideReactionsChanges;
	rpl::event_stream<bool> _hideCommentsChanges;
	rpl::event_stream<bool> _squareUserpicsChanges;
	rpl::event_stream<bool> _antiCapsChanges;
	rpl::event_stream<bool> _fakePremiumChanges;
	rpl::event_stream<bool> _fakeStarsChanges;
	rpl::event_stream<int64> _fakeStarsAmountChanges;
	rpl::event_stream<bool> _translateOutgoingChanges;
	rpl::event_stream<QString> _translateOutgoingLangChanges;
	rpl::event_stream<AutoFormat> _autoFormatChanges;
	rpl::event_stream<QString> _customFontFamilyChanges;
	rpl::event_stream<QString> _chatBackgroundMediaPathChanges;
	rpl::event_stream<double> _chatBackgroundMediaOpacityChanges;

	base::Timer _saveTimer;
	bool _loading = false;

};

} // namespace Ghostgram
