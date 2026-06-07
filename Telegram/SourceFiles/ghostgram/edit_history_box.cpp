/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/edit_history_box.h"

#include "base/unixtime.h"
#include "data/data_peer.h"
#include "ghostgram/ghostgram.h"
#include "ghostgram/i18n.h"
#include "ghostgram/message_archive/edited_messages.h"
#include "ghostgram/message_archive/message_archive.h"
#include "history/history.h"
#include "history/history_item.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "styles/style_layers.h"
#include "styles/style_settings.h"

#include <QtCore/QDateTime>
#include <QtCore/QLocale>

namespace Ghostgram {
namespace {

[[nodiscard]] QString FormatTimestamp(int64 unixTime) {
	if (!unixTime) {
		return QString();
	}
	const auto when = base::unixtime::parse(static_cast<TimeId>(unixTime));
	return QLocale::system().toString(when, QLocale::ShortFormat);
}

void AddVersion(
		not_null<Ui::VerticalLayout*> container,
		const QString &heading,
		const QString &timestamp,
		const QString &text) {
	const auto fullTitle = timestamp.isEmpty()
		? heading
		: (heading + u"  ·  "_q + timestamp);

	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, rpl::single(fullTitle));

	const auto label = container->add(
		object_ptr<Ui::FlatLabel>(
			container,
			rpl::single(text),
			st::boxLabel),
		st::boxRowPadding);
	label->setSelectable(true);

	Ui::AddSkip(container);
	Ui::AddDivider(container);
}

} // namespace

void EditHistoryBox(
		not_null<Ui::GenericBox*> box,
		not_null<HistoryItem*> item) {
	box->setTitle(I18n::TrValue(
		u"Edit history"_q,
		u"История изменений"_q));
	box->setWidth(st::boxWideWidth);

	const auto peerId = item->history()->peer->id.value;
	const auto messageId = item->id.bare;
	const auto &edited = Manager::Instance().archive().edited();
	const auto history = edited.historyFor(peerId, messageId);
	const auto currentText = item->originalText().text;

	const auto content = box->verticalLayout();

	const auto current = I18n::Tr(u"Current"_q, u"Текущая"_q);
	const auto original = I18n::Tr(u"Original"_q, u"Оригинал"_q);
	const auto empty = I18n::Tr(u"(empty)"_q, u"(пусто)"_q);
	const auto versionWord = I18n::Tr(u"Version"_q, u"Версия"_q);

	const auto revisionCount = int(history.revisions.size());
	if (revisionCount == 0) {
		AddVersion(
			content,
			current,
			QString(),
			currentText.isEmpty() ? empty : currentText);
	} else {
		AddVersion(
			content,
			original,
			QString(),
			history.revisions[0].text.isEmpty()
				? empty
				: history.revisions[0].text);

		for (auto i = 1; i < revisionCount; ++i) {
			const auto stamp = FormatTimestamp(history.revisions[i - 1].editedAt);
			const auto txt = history.revisions[i].text;
			AddVersion(
				content,
				versionWord + u" %1"_q.arg(i),
				stamp,
				txt.isEmpty() ? empty : txt);
		}

		const auto lastStamp = FormatTimestamp(
			history.revisions[revisionCount - 1].editedAt);
		AddVersion(
			content,
			current,
			lastStamp,
			currentText.isEmpty() ? empty : currentText);
	}

	box->addButton(
		I18n::TrValue(u"Close"_q, u"Закрыть"_q),
		[=] { box->closeBox(); });
}

} // namespace Ghostgram
