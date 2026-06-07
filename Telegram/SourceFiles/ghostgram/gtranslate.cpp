/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/gtranslate.h"

#include <QtCore/QEventLoop>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonValue>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Ghostgram {
namespace {

[[nodiscard]] QString ParseGoogleResponse(const QByteArray &bytes) {
	const auto doc = QJsonDocument::fromJson(bytes);
	if (!doc.isArray()) {
		return QString();
	}
	const auto root = doc.array();
	if (root.isEmpty() || !root.at(0).isArray()) {
		return QString();
	}
	auto result = QString();
	for (const auto &segmentValue : root.at(0).toArray()) {
		if (!segmentValue.isArray()) {
			continue;
		}
		const auto segment = segmentValue.toArray();
		if (segment.isEmpty() || !segment.at(0).isString()) {
			continue;
		}
		result.append(segment.at(0).toString());
	}
	return result;
}

} // namespace

QString TranslateSync(
		const QString &text,
		const QString &targetLang,
		int timeoutMs) {
	if (text.trimmed().isEmpty() || targetLang.isEmpty()) {
		return text;
	}

	auto url = QUrl(u"https://translate.googleapis.com/translate_a/single"_q);
	auto query = QUrlQuery();
	query.addQueryItem(u"client"_q, u"gtx"_q);
	query.addQueryItem(u"sl"_q, u"auto"_q);
	query.addQueryItem(u"tl"_q, targetLang);
	query.addQueryItem(u"dt"_q, u"t"_q);
	query.addQueryItem(u"q"_q, text);
	url.setQuery(query);

	auto request = QNetworkRequest(url);
	request.setHeader(
		QNetworkRequest::UserAgentHeader,
		u"Mozilla/5.0"_q);

	auto manager = QNetworkAccessManager();
	auto waiter = QEventLoop();
	auto reply = manager.get(request);

	auto timer = QTimer();
	timer.setSingleShot(true);
	QObject::connect(&timer, &QTimer::timeout, &waiter, [&] {
		reply->abort();
		waiter.quit();
	});
	QObject::connect(
		reply,
		&QNetworkReply::finished,
		&waiter,
		&QEventLoop::quit);

	timer.start(timeoutMs);
	waiter.exec();

	auto translated = QString();
	if (reply->error() == QNetworkReply::NoError) {
		translated = ParseGoogleResponse(reply->readAll());
	}
	reply->deleteLater();
	return translated.isEmpty() ? text : translated;
}

} // namespace Ghostgram
