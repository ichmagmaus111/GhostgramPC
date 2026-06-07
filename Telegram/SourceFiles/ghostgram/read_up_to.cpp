/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/read_up_to.h"

#include "apiwrap.h"
#include "data/data_channel.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "ghostgram/ghostgram.h"
#include "ghostgram/ghost_mode/ghost_mode.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"

namespace Ghostgram {

void ReadUpTo(not_null<HistoryItem*> item) {
	const auto history = item->history();
	const auto peer = history->peer;
	const auto session = &peer->session();
	const auto maxId = item->id;

	if (const auto channel = peer->asChannel()) {
		session->api().request(MTPchannels_ReadHistory(
			channel->inputChannel(),
			MTP_int(maxId)
		)).send();
	} else {
		session->api().request(MTPmessages_ReadHistory(
			peer->input(),
			MTP_int(maxId)
		)).done([=](const MTPmessages_AffectedMessages &result) {
			session->api().applyAffectedMessages(peer, result);
		}).send();
	}

	history->inboxRead(maxId);

	GoOfflineAfterAction(session);
}

void GoOfflineAfterAction(not_null<Main::Session*> session) {
	const auto &ghost = Manager::Instance().ghostMode();
	if (!ghost.shouldHideOnline() && !ghost.shouldForceOffline()) {
		return;
	}
	const auto raw = session.get();
	crl::on_main(raw, [raw] {
		raw->api().request(MTPaccount_UpdateStatus(
			MTP_bool(true) // offline
		)).send();
	});
}

} // namespace Ghostgram
