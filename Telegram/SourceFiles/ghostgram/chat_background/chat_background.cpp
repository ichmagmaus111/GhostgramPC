/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/chat_background/chat_background.h"

#include "ghostgram/ghostgram_config.h"

#include <rpl/then.h>

#include <QtCore/QFileInfo>
#include <QtGui/QPainter>

namespace Ghostgram {
namespace {

constexpr auto kInitialFrame = QSize(1280, 720);
constexpr auto kMaxDecodeSide = 1280;
constexpr auto kRepaintIntervalMs = crl::time(33);

} // namespace

ChatBackground::ChatBackground(not_null<Config*> config)
: _config(config)
, _repaintThrottle([=] {
	_repaintPending = false;
	_lastRepaintFired = crl::now();
	_repaintRequests.fire({});
}) {
	_config->chatBackgroundMediaPathValue(
	) | rpl::on_next([=](QString path) {
		reload(path);
	}, _lifetime);

	_config->chatBackgroundMediaOpacityValue(
	) | rpl::on_next([=](double) {
		_repaintRequests.fire({});
	}, _lifetime);
}

ChatBackground::~ChatBackground() {
	release();
}

void ChatBackground::release() {
	if (_reader) {
		_reader.reset();
	}
	_currentPath.clear();
	_startedSize = QSize();
	_scaledFrame = QPixmap();
	_scaledTargetSize = QSize();
	_scaledFrameIndex = -1;
}

void ChatBackground::reload(const QString &path) {
	if (path == _currentPath) {
		return;
	}
	release();
	_currentPath = path;
	if (path.isEmpty() || !QFileInfo(path).isFile()) {
		_repaintRequests.fire({});
		return;
	}
	const auto weak = base::make_weak(this);
	_reader = ::Media::Clip::MakeReader(
		path,
		[weak](::Media::Clip::Notification notification) {
			if (const auto strong = weak.get()) {
				strong->clipCallback(notification);
			}
		});
	_repaintRequests.fire({});
}

void ChatBackground::clipCallback(
		::Media::Clip::Notification notification) {
	using ::Media::Clip::Notification;
	using ::Media::Clip::State;
	switch (notification) {
	case Notification::Reinit: {
		if (_reader && _reader->state() == State::Error) {
			_reader.setBad();
		} else if (_reader && _reader->ready() && !_reader->started()) {
			auto w = _reader->width();
			auto h = _reader->height();
			if (w <= 0 || h <= 0) {
				w = kInitialFrame.width();
				h = kInitialFrame.height();
			}
			const auto maxSide = std::max(w, h);
			if (maxSide > kMaxDecodeSide) {
				const auto scale = double(kMaxDecodeSide) / double(maxSide);
				w = std::max(1, int(w * scale));
				h = std::max(1, int(h * scale));
			}
			_startedSize = QSize(w, h);
			_reader->start({ .frame = _startedSize, .keepAlpha = false });
		}
		scheduleRepaint();
	} break;
	case Notification::Repaint: {
		if (_reader && _reader->started() && !_reader->currentDisplayed()) {
			scheduleRepaint();
		}
	} break;
	}
}

void ChatBackground::scheduleRepaint() {
	const auto now = crl::now();
	const auto since = now - _lastRepaintFired;
	if (since >= kRepaintIntervalMs) {
		_lastRepaintFired = now;
		_repaintPending = false;
		_repaintThrottle.cancel();
		_repaintRequests.fire({});
	} else if (!_repaintPending) {
		_repaintPending = true;
		_repaintThrottle.callOnce(kRepaintIntervalMs - since);
	}
}

bool ChatBackground::active() const {
	return _reader
		&& !_reader.isBad()
		&& _reader->started()
		&& _config->chatBackgroundMediaOpacity() > 0.0;
}

void ChatBackground::paint(QPainter &p, QRect target, bool paused) {
	if (!active() || target.isEmpty() || _startedSize.isEmpty()) {
		return;
	}
	const auto info = _reader->frameInfo(
		{ .frame = _startedSize, .keepAlpha = false },
		paused ? 0 : crl::now());
	_reader->moveToNextFrame();
	if (info.image.isNull()) {
		return;
	}
	const auto opacity = std::clamp(
		_config->chatBackgroundMediaOpacity(),
		0.0,
		1.0);
	if (opacity <= 0.0) {
		return;
	}

	const auto fw = info.image.width();
	const auto fh = info.image.height();
	if (fw <= 0 || fh <= 0) {
		return;
	}
	const auto tw = target.width();
	const auto th = target.height();
	const auto scale = std::max(
		double(tw) / double(fw),
		double(th) / double(fh));
	const auto dstW = std::max(1, int(std::ceil(fw * scale)));
	const auto dstH = std::max(1, int(std::ceil(fh * scale)));
	const auto dstSize = QSize(dstW, dstH);

	if (_scaledFrame.isNull()
		|| _scaledFrameIndex != info.index
		|| _scaledTargetSize != target.size()) {
		auto scaled = info.image.scaled(
			dstSize,
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);
		_scaledFrame = QPixmap::fromImage(std::move(scaled));
		_scaledFrameIndex = info.index;
		_scaledTargetSize = target.size();
	}

	const auto dx = target.x() + (tw - dstW) / 2;
	const auto dy = target.y() + (th - dstH) / 2;
	const auto previous = p.opacity();
	p.setOpacity(previous * opacity);
	p.drawPixmap(dx, dy, _scaledFrame);
	p.setOpacity(previous);
}

rpl::producer<> ChatBackground::repaintRequests() const {
	return _repaintRequests.events();
}

} // namespace Ghostgram
