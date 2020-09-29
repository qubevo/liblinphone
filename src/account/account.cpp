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
 * but WITHOUT ANY WARRANTY{
}
 without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "account.h"

// =============================================================================

using namespace std;

LINPHONE_BEGIN_NAMESPACE

Account::Account (LinphoneCore *lc, std::shared_ptr<AccountParams> params) {
	mCore = lc;
}


Account::Account (const Account &other) : HybridObject<LinphoneAccount, Account>() {
}

Account::~Account () {
}

Account* Account::clone () const {
	return nullptr;
}

Account& Account::operator= (const Account &other) {
	return *this;
}

void Account::setAccountParams (std::shared_ptr<AccountParams> params) {
}

std::shared_ptr<AccountParams> Account::getAccountParams () {
	return mParams;
}

void Account::setAuthFailure (int authFailure) {
}

void Account::setRegisterChanged (bool registerChanged) {
}

void Account::setDeletionDate (time_t deletionDate) {
}

void Account::setUserData (void *userData) {
}

void Account::setErrorInfo (LinphoneErrorInfo *errorInfo) {
}

void Account::setContactAddress (LinphoneAddress *contact) {
}

void Account::setContactAddressWithoutParams (LinphoneAddress *contact) {
}

void Account::setPendingContactAddress (LinphoneAddress *contact) {
}

void Account::setServiceRouteAddress (LinphoneAddress *serviceRoute) {
}

void Account::setState (LinphoneRegistrationState state) {
}

void Account::setOp (SalRegisterOp *op) {
}

void Account::setCustomheader (const std::string& headerName, const std::string& headerValue) {
}

void Account::setPresencePublishEvent (LinphoneEvent *presencePublishEvent) {
}

void Account::setDependency (std::shared_ptr<Account> dependency) {
}

int Account::getAuthFailure () const {
	return mAuthFailure;
}

bool Account::hasRegisterChanged () const {
	return mRegisterChanged;
}

time_t Account::getDeletionDate () const {
	return mDeletionDate;
}

void *Account::getUserData () const {
	return mUserData;
}

LinphoneCore *Account::getCore () const {
	return mCore;
}

LinphoneErrorInfo *Account::getErrorInfo () const {
	return mErrorInfo;
}

LinphoneAddress *Account::getContactAddress () const {
	return mContactAddress;
}

LinphoneAddress* Account::getContactAddressWithoutParams () const {
	return mContactAddressWithoutParams;
}

LinphoneAddress* Account::getPendingContactAddress () const {
	return mPendingContactAddress;
}

LinphoneAddress* Account::getServiceRouteAddress () const {
	return mServiceRouteAddress;
}

LinphoneRegistrationState Account::getState () const {
	return mState;
}

SalRegisterOp* Account::getOp() const {
	return mOp;
}

const std::string Account::getCustomHeader (const std::string& headerName) const {
	if (mSentHeaders != NULL) return string();
	return string();
}

LinphoneEvent* Account::getPresencePublishEvent () const {
	return mPresencePublishEvent;
}

std::shared_ptr<Account> Account::getDependency () const {
	return mDependency;
}

void Account::refreshRegister () {
}

void Account::pauseRegister () {
}

LinphoneReason Account::getError () const {
	return LinphoneReasonNone;
}

const std::string Account::getTransport () const {
	return string();
}

bool Account::avpfEnabled () const {
	return false;
}

const LinphoneAuthInfo* Account::findAuthInfo () const {
	return NULL;
}

int Account::getUnreadChatMessageCount () const {
	return -1;
}

LINPHONE_END_NAMESPACE
