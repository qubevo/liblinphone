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

#include "account/account-params.h"
#include "linphone/api/c-account-params.h"

// =============================================================================

using namespace LinphonePrivate;

LinphoneAccountParams* linphone_account_params_new(LinphoneCore *lc) {
	return NULL;
}

LinphoneAccountParams* linphone_account_params_new_with_config(LinphoneCore *lc, int index) {
	return NULL;
}

LinphoneAccountParams* linphone_account_params_clone(const LinphoneAccountParams *params) {
	return NULL;
}

LinphoneAccountParams* linphone_account_params_ref(LinphoneAccountParams *params) {
	return NULL;
}

void linphone_account_params_unref(LinphoneAccountParams *params) {
}

void linphone_account_params_set_user_data(LinphoneAccountParams *params, void *user_data) {
}

void* linphone_account_params_get_user_data(LinphoneAccountParams *params) {
	return NULL;
}

LinphoneStatus linphone_account_params_set_server_addr(LinphoneAccountParams *params, const char *server_address) {
	return -1;
}

LinphoneStatus linphone_account_params_set_identity_address(LinphoneAccountParams *params, const LinphoneAddress *identity) {
	return -1;
}

LinphoneStatus linphone_account_params_set_routes(LinphoneAccountParams *params, const bctbx_list_t *routes) {
	return -1;
}

LinphoneStatus linphone_account_params_set_routes_addresses(LinphoneAccountParams *params, const bctbx_list_t *routes) {
	return -1;
}

void linphone_account_params_set_expires(LinphoneAccountParams *params, int expires) {
}

void linphone_account_params_set_register_enabled(LinphoneAccountParams *params, bool_t enable) {
}

void linphone_account_params_set_publish_enabled(LinphoneAccountParams *params, bool_t enable) {
}

void linphone_account_params_set_publish_expires(LinphoneAccountParams *params, int expires) {
}

int linphone_account_params_get_publish_expires(const LinphoneAccountParams *params) {
	return -1;
}

void linphone_account_params_set_dial_escape_plus_enabled(LinphoneAccountParams *params, bool_t enable) {
}

void linphone_account_params_set_dial_prefix(LinphoneAccountParams *params, const char *prefix) {
}

void linphone_account_params_set_quality_reporting_enabled(LinphoneAccountParams *params, bool_t enable) {
}

bool_t linphone_account_params_get_quality_reporting_enabled(LinphoneAccountParams *params) {
	return FALSE;
}

void linphone_account_params_set_quality_reporting_collector(LinphoneAccountParams *params, const char *collector) {
}

const char* linphone_account_params_get_quality_reporting_collector(const LinphoneAccountParams *params) {
	return NULL;
}

void linphone_account_params_set_quality_reporting_interval(LinphoneAccountParams *params, int interval) {
}

int linphone_account_params_get_quality_reporting_interval(LinphoneAccountParams *params) {
	return -1;
}

const char* linphone_account_params_get_domain(const LinphoneAccountParams *params) {
	return NULL;
}

const char* linphone_account_params_get_realm(const LinphoneAccountParams *params) {
	return NULL;
}

void linphone_account_params_set_realm(LinphoneAccountParams *params, const char * realm) {
}

const bctbx_list_t* linphone_account_params_get_routes(const LinphoneAccountParams *params) {
	return NULL;
}

const bctbx_list_t* linphone_account_params_get_routes_addresses(const LinphoneAccountParams *params) {
	return NULL;
}

const LinphoneAddress *linphone_account_params_get_identity_address(const LinphoneAccountParams *params) {
	return NULL;
}

bool_t linphone_account_params_get_publish_enabled(const LinphoneAccountParams *params) {
	return FALSE;
}

const char* linphone_account_params_get_server_addr(const LinphoneAccountParams *params) {
	return NULL;
}

int linphone_account_params_get_expires(const LinphoneAccountParams *params) {
	return -1;
}

bool_t linphone_account_params_get_register_enabled(const LinphoneAccountParams *params) {
	return FALSE;
}

const char* linphone_account_params_get_contact_parameters(const LinphoneAccountParams *params) {
	return NULL;
}

void linphone_account_params_set_contact_parameters(LinphoneAccountParams *params, const char *contact_params) {
}

void linphone_account_params_set_contact_uri_parameters(LinphoneAccountParams *params, const char *contact_uri_params) {
}

const char* linphone_account_params_get_contact_uri_parameters(const LinphoneAccountParams *params) {
	return NULL;
}

bool_t linphone_account_params_get_dial_escape_plus_enabled(const LinphoneAccountParams *params) {
	return FALSE;
}

const char* linphone_account_params_get_dial_prefix(const LinphoneAccountParams *params) {
	return NULL;
}

void linphone_account_params_set_privacy(LinphoneAccountParams *params, LinphonePrivacyMask privacy) {
}

LinphonePrivacyMask linphone_account_params_get_privacy(const LinphoneAccountParams *params) {
	return 0;
}

void linphone_account_params_set_file_transfer_server(LinphoneAccountParams *params, const char * server_url) {
}

const char* linphone_account_params_get_file_transfer_server(const LinphoneAccountParams *params) {
	return NULL;
}

void linphone_account_params_set_avpf_rr_interval(LinphoneAccountParams *params, uint8_t interval) {
}

uint8_t linphone_account_params_get_avpf_rr_interval(const LinphoneAccountParams *params) {
	return 0;
}

LinphoneAVPFMode linphone_account_params_get_avpf_mode(const LinphoneAccountParams *params) {
	return LinphoneAVPFDefault;
}

void linphone_account_params_set_avpf_mode(LinphoneAccountParams *params, LinphoneAVPFMode mode) {
}

const char* linphone_account_params_get_ref_key(const LinphoneAccountParams *params) {
	return NULL;
}

void linphone_account_params_set_ref_key(LinphoneAccountParams *params, const char *refkey) {
}

const char* linphone_account_params_get_idkey(LinphoneAccountParams *params) {
	return NULL;
}

void linphone_account_params_set_idkey(LinphoneAccountParams *params, const char *idkey) {
}

LinphoneNatPolicy* linphone_account_params_get_nat_policy(const LinphoneAccountParams *params) {
	return NULL;
}

void linphone_account_params_set_nat_policy(LinphoneAccountParams *params, LinphoneNatPolicy *policy) {
}

void linphone_account_params_set_conference_factory_uri(LinphoneAccountParams *params, const char *uri) {
}

const char* linphone_account_params_get_conference_factory_uri(const LinphoneAccountParams *params) {
	return NULL;
}

void linphone_account_params_set_push_notification_allowed(LinphoneAccountParams *params, bool_t allow) {
}

bool_t linphone_account_params_get_push_notification_allowed(const LinphoneAccountParams *params) {
	return FALSE;
}

void linphone_account_params_set_outbound_proxy_enabled(LinphoneAccountParams *params, bool_t enable) {
}

bool_t linphone_account_params_get_outbound_proxy_enabled(LinphoneAccountParams *params) {
	return FALSE;
}
