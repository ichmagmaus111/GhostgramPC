/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/basic_types.h"

class HistoryItem;

namespace Ui {
class GenericBox;
} // namespace Ui

namespace Ghostgram {

void EditHistoryBox(
	not_null<Ui::GenericBox*> box,
	not_null<HistoryItem*> item);

} // namespace Ghostgram
