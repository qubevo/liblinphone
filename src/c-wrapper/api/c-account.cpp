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
#include "linphone/api/c-account.h"

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
