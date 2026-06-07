/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/deleted_reply.h"

#include "data/data_document.h"
#include "data/data_media_types.h"
#include "data/data_peer.h"
#include "ghostgram/ghostgram.h"
#include "ghostgram/i18n.h"
#include "ghostgram/message_archive/deleted_messages.h"
#include "ghostgram/message_archive/message_archive.h"
#include "history/history.h"
#include "history/history_item.h"
#include "ui/text/text_entity.h"
#include "ui/widgets/fields/input_field.h"

namespace Ghostgram {

bool TryQuoteDeletedInto(
		not_null<Ui::InputField*> field,
		not_null<HistoryItem*> item) {
	const auto peerId = item->history()->peer->id.value;
	const auto messageId = item->id.bare;
	auto &deleted = Manager::Instance().archive().deleted();
	const auto record = deleted.find(peerId, messageId);
	if (!record) {
		return false;
	}

	const auto authorName = item->from()->name();
	auto deletedText = record->text.isEmpty()
		? item->originalText().text
		: record->text;
	if (deletedText.isEmpty()) {
		if (const auto media = item->media()) {
			auto label = media->notificationText().text;
			if (const auto document = media->document()) {
				if (!document->isVoiceMessage()
					&& !document->isVideoMessage()
					&& !document->isAnimation()
					&& !document->isVideoFile()
					&& !document->isSong()
					&& !document->sticker()
					&& !document->isImage()) {
					const auto name = document->filename();
					if (!name.isEmpty()) {
						label = I18n::Tr(u"File: "_q, u"Файл: "_q) + name;
					}
				}
			}
			if (!label.isEmpty()) {
				deletedText = u"["_q + label + u"]"_q;
			}
		}
	}
	if (authorName.isEmpty() && deletedText.isEmpty()) {
		return false;
	}

	auto current = field->getTextWithTags();
	const auto authorLen = int(authorName.size());
	const auto textLen = int(deletedText.size());

	auto prefix = QString();
	prefix.reserve(authorLen + textLen + 4);
	if (!authorName.isEmpty()) {
		prefix.append(authorName).append(QChar('\n'));
	}
	prefix.append(deletedText).append(QChar('\n'));
	const auto prefixLen = int(prefix.size());

	auto tags = TextWithTags::Tags();
	const auto quoteLen = authorName.isEmpty()
		? textLen
		: (authorLen + 1 + textLen);
	tags.push_back({
		0,
		quoteLen,
		Ui::InputField::kTagBlockquote,
	});

	for (const auto &t : current.tags) {
		tags.push_back({ t.offset + prefixLen, t.length, t.id });
	}

	auto combined = TextWithTags{
		.text = prefix + current.text,
		.tags = std::move(tags),
	};
	field->setTextWithTags(combined, Ui::InputField::HistoryAction::Clear);

	auto cursor = field->textCursor();
	cursor.setPosition(combined.text.size());
	field->setTextCursor(cursor);
	field->setFocus();

	return true;
}

} // namespace Ghostgram
