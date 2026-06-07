/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/basic_types.h"

class HistoryItem;

namespace Ui {
class InputField;
} // namespace Ui

namespace Ghostgram {

bool TryQuoteDeletedInto(
	not_null<Ui::InputField*> field,
	not_null<HistoryItem*> item);

} // namespace Ghostgram
