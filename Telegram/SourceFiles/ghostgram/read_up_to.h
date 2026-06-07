/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/basic_types.h"

class HistoryItem;

namespace Main {
class Session;
} // namespace Main

namespace Ghostgram {

void ReadUpTo(not_null<HistoryItem*> item);

// After an outgoing action (sending a message, reacting, reading) the
// Telegram server marks us online again. When ghost mode hides the online
// status or forces offline, re-assert offline right away so we don't linger
// online for the server-side timeout.
void GoOfflineAfterAction(not_null<Main::Session*> session);

} // namespace Ghostgram
