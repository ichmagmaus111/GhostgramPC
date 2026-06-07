/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/timer.h"
#include "base/weak_ptr.h"
#include "media/clip/media_clip_reader.h"

#include <rpl/event_stream.h>
#include <rpl/lifetime.h>
#include <rpl/producer.h>

#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtGui/QPixmap>

class QPainter;
class QRect;

namespace Ghostgram {

class Config;

class ChatBackground final : public base::has_weak_ptr {
public:
	explicit ChatBackground(not_null<Config*> config);
	~ChatBackground();

	ChatBackground(const ChatBackground &) = delete;
	ChatBackground &operator=(const ChatBackground &) = delete;

	void paint(QPainter &p, QRect target, bool paused);
	[[nodiscard]] bool active() const;
	[[nodiscard]] rpl::producer<> repaintRequests() const;

private:
	void reload(const QString &path);
	void release();
	void clipCallback(::Media::Clip::Notification notification);
	void scheduleRepaint();

	not_null<Config*> _config;
	::Media::Clip::ReaderPointer _reader;
	QString _currentPath;
	QSize _startedSize;
	QPixmap _scaledFrame;
	QSize _scaledTargetSize;
	int _scaledFrameIndex = -1;
	rpl::event_stream<> _repaintRequests;
	base::Timer _repaintThrottle;
	crl::time _lastRepaintFired = 0;
	bool _repaintPending = false;
	rpl::lifetime _lifetime;

};

} // namespace Ghostgram
