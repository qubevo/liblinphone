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

#include "account-params.h"

// =============================================================================

using namespace std;

LINPHONE_BEGIN_NAMESPACE

AccountParams::AccountParams (LinphoneCore *lc) {
}

AccountParams::AccountParams (LinphoneCore *lc, int index) {
}

AccountParams::AccountParams (const AccountParams &other) : HybridObject<LinphoneAccountParams, AccountParams>() {
}

AccountParams::~AccountParams () {
}

AccountParams* AccountParams::clone () const {
	return nullptr;
}

AccountParams& AccountParams::operator= (const AccountParams &other) {
	return *this;
}

void AccountParams::setExpires (int expires) {
}

void AccountParams::setQualityReportingInterval (int qualityReportingInterval) {
}

void AccountParams::setPublishExpires (int publishExpires) {
}

void AccountParams::setAvpfRrInterval (uint8_t avpfRrInterval) {
}

void AccountParams::setRegisterEnabled (bool enable) {
}

void AccountParams::setDialEscapePlusEnabled (bool enable) {
}

void AccountParams::setQualityReportingEnabled (bool enable) {
}

void AccountParams::setPublishEnabled (bool enable) {
}

void AccountParams::setOutboundProxyEnabled (bool enable) {
}

void AccountParams::setPushNotificationAllowed (bool allow) {
}

void AccountParams::setUserData (void *userData) {
}

void AccountParams::setDialPrefix (const std::string &dialPrefix) {
}

void AccountParams::setProxy (const std::string &proxy) {
}

void AccountParams::setRealm (const std::string &realm) {
}

void AccountParams::setQualityReportingCollector (const std::string &qualityReportingCollector) {
}

void AccountParams::setContactParameters (const std::string &contactParameters) {
}

void AccountParams::setContactUriParameters (const std::string &contactUriParameters) {
}

void AccountParams::setRefKey (const std::string &refKey) {
}

void AccountParams::setDependsOn (const std::string &dependsOn) {
}

void AccountParams::setIdKey (const std::string &idKey) {
}

void AccountParams::setConferenceFactoryUri (const std::string &conferenceFactoryUri) {
}

void AccountParams::setFileTranferServer (const std::string &fileTransferServer) {
}

void AccountParams::setRoutes (const std::list<LinphoneAddress *> &routes) {
}

void AccountParams::setPrivacy (LinphonePrivacyMask privacy) {
}

void AccountParams::setIdentityAddress (LinphoneAddress* identityAddress) {
}

void AccountParams::setAvpfMode (LinphoneAVPFMode avpfMode) {
}

void AccountParams::setNatPolicy (LinphoneNatPolicy *natPolicy) {
}

int AccountParams::getExpires () const {
	return mExpires;
}

int AccountParams::getQualityReportingInterval () const {
	return mQualityReportingInterval;
}

int AccountParams::getPublishExpires () const {
	return mPublishExpires;
}

uint8_t AccountParams::getAvpfRrInterval () const {
	return mAvpfRrInterval;
}

bool AccountParams::getRegisterEnabled () const {
	return mRegisterEnabled;
}

bool AccountParams::getDialEscapePlusEnabled () const {
	return mDialEscapePlusEnabled;
}

bool AccountParams::getQualityReportingEnabled () const {
	return mQualityReportingEnabled;
}

bool AccountParams::getPublishEnabled () const {
	return mPublishEnabled;
}

bool AccountParams::getOutboundProxyEnabled () const {
	return mOutboundProxyEnabled;
}

bool AccountParams::getPushNotificationAllowed () const {
	return mPushNotificationAllowed;
}

void* AccountParams::getUserData () const {
	return mUserData;
}

const std::string& AccountParams::getDialPrefix () const {
	return mDialPrefix;
}

const std::string& AccountParams::getDomain () const {
	return mProxy; // TODO: get the domain
}

const std::string& AccountParams::getProxy () const {
	return mProxy;
}

const std::string& AccountParams::getRealm () const {
	return mRealm;
}

const std::string& AccountParams::getQualityReportingCollector () const {
	return mQualityReportingCollector;
}

const std::string& AccountParams::getContactParameters () const {
	return mContactParameters;
}

const std::string& AccountParams::getContactUriParameters () const {
	return mContactUriParameters;
}

const std::string& AccountParams::getRefKey () const {
	return mRefKey;
}

const std::string& AccountParams::getDependsOn () const {
	return mDependsOn;
}

const std::string& AccountParams::getIdKey () const {
	return mIdKey;
}

const std::string& AccountParams::getConferenceFactoryUri () const {
	return mConferenceFactoryUri;
}

const std::string& AccountParams::getFileTransferServer () const {
	return mFileTransferServer;
}

const std::list<LinphoneAddress *>& AccountParams::getRoutes () const {
	return mRoutes;
}

LinphonePrivacyMask AccountParams::getPrivacy () const {
	return mPrivacy;
}

LinphoneAddress* AccountParams::getIdentityAddress () const {
	return mIdentityAddress;
}

LinphoneAVPFMode AccountParams::getAvpfMode () const {
	return mAvpfMode;
}

LinphoneNatPolicy* AccountParams::getNatPolicy () const {
	return mNatPolicy;
}

LinphoneStatus AccountParams::setServerAddr (const std::string &addr) {
	return LinphoneStatusOffline;
}

LINPHONE_END_NAMESPACE
