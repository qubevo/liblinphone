/*
 * Copyright (c) 2010-2020 Belledonne Communications SARL.
 *
 * This file is part of Liblinphone.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "account/account.h"
#include "account/account-params.h"
#include "c-wrapper/internal/c-tools.h"
#include "linphone/api/c-account.h"
#include "linphone/api/c-account-cbs.h"
#include "linphone/wrapper_utils.h"

// =============================================================================

using namespace LinphonePrivate;

LinphoneAccount* linphone_account_new(LinphoneCore *lc, LinphoneAccountParams *params) {
	return NULL;
}

LinphoneAccount* linphone_account_clone(const LinphoneAccount *account) {
	return NULL;
}

LinphoneAccount* linphone_account_ref(LinphoneAccount *account) {
	return NULL;
}

void linphone_account_unref(LinphoneAccount *account) {
}

void linphone_account_set_params(LinphoneAccount *account, LinphoneAccountParams* params) {
}

LinphoneAccountParams* linphone_account_get_params(LinphoneAccount *account) {
	return NULL;
}

void linphone_account_set_user_data(LinphoneAccount *account, void *user_data) {
}

void* linphone_account_get_user_data(LinphoneAccount *account) {
	return NULL;
}

void linphone_account_set_custom_header(LinphoneAccount *account, const char *header_name, const char *header_value) {
}

const char* linphone_account_get_custom_header(LinphoneAccount *account, const char *header_name) {
	return NULL;
}

void linphone_account_set_dependency(LinphoneAccount *account, LinphoneAccount *depends_on) {
}

LinphoneAccount* linphone_account_get_dependency(LinphoneAccount *account) {
	return NULL;
}

LinphoneCore* linphone_account_get_core(LinphoneAccount *account) {
	return NULL;
}

LinphoneErrorInfo* linphone_account_get_error_info(LinphoneAccount *account) {
	return NULL;
}

LinphoneAddress* linphone_account_get_contact_address(LinphoneAccount *account) {
	return NULL;
}

LinphoneRegistrationState linphone_account_get_state(LinphoneAccount *account) {
	return LinphoneRegistrationNone;
}

void linphone_account_refresh_register(LinphoneAccount *account) {
}

void linphone_account_pause_register(LinphoneAccount *account) {
}

LinphoneReason linphone_account_get_error(LinphoneAccount *account) {
	return LinphoneReasonNone;
}

LinphoneTransportType linphone_account_get_transport(LinphoneAccount *account) {
	return LinphoneTransportUdp;
}

bool_t linphone_account_is_avpf_enabled(LinphoneAccount *account) {
	return FALSE;
}

const LinphoneAuthInfo* linphone_account_find_auth_info(LinphoneAccount *account) {
	return NULL;
}

int linphone_account_get_unread_chat_message_count(LinphoneAccount *account) {
	return -1;
}

bool_t linphone_account_is_phone_number(LinphoneAccount *account, const char *username) {
	return FALSE;
}

char* linphone_account_normalize_phone_number(LinphoneAccount *account, const char *username) {
	return NULL;
}

LinphoneAddress* linphone_account_normalize_sip_uri(LinphoneAccount *account, const char* username) {
	return NULL;
}

void linphone_account_add_callbacks(LinphoneAccount *account, LinphoneAccountCbs *cbs) {
	Account::toCpp(account)->addCallbacks(linphone_account_cbs_ref(cbs));
}

void linphone_account_remove_callbacks(LinphoneAccount *account, LinphoneAccountCbs *cbs) {
	Account::toCpp(account)->removeCallbacks(cbs);
	linphone_account_cbs_unref(cbs);
}

LinphoneAccountCbs *linphone_account_get_current_callbacks(const LinphoneAccount *account) {
	return Account::toCpp(account)->getCurrentCallbacks();
}

void linphone_account_set_current_callbacks(LinphoneAccount *account, LinphoneAccountCbs *cbs) {
	Account::toCpp(account)->setCurrentCallbacks(cbs);
}

const bctbx_list_t *linphone_account_get_callbacks_list(const LinphoneAccount *account) {
	return L_GET_C_LIST_FROM_CPP_LIST(Account::toCpp(account)->getCallbacksList());
}

#define NOTIFY_IF_EXIST(cbName, functionName, ...) \
	auto& list = Account::toCpp(account)->getCallbacksList(); \
	for (const auto &cbs : list) { \
		Account::toCpp(account)->setCurrentCallbacks(cbs); \
		LinphoneAccountCbs ## cbName ## Cb cb = linphone_account_cbs_get_ ## functionName (cbs); \
		if (cb) \
			cb(__VA_ARGS__); \
	} \
	Account::toCpp(account)->setCurrentCallbacks(nullptr);

void _linphone_account_notify_registration_state_changed(LinphoneAccount *account, LinphoneRegistrationState state, const char *message) {
	NOTIFY_IF_EXIST(RegistrationStateChanged, registration_state_changed, account, state, message)
}
