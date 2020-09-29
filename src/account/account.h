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

#ifndef _L_ACCOUNT_H_
#define _L_ACCOUNT_H_

#include <memory>

#include <belle-sip/object++.hh>

#include "account-params.h"
#include "c-wrapper/internal/c-sal.h"
#include "linphone/api/c-types.h"
#include "linphone/sipsetup.h"
#include "sal/register-op.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

class LINPHONE_PUBLIC Account : public bellesip::HybridObject<LinphoneAccount, Account> {
public:
	Account (LinphoneCore *lc, std::shared_ptr<AccountParams> params);
	Account (const Account &other);
	~Account ();

	Account* clone () const override;
	Account& operator= (const Account &other);

	// Account params configuration
	void setAccountParams (std::shared_ptr<AccountParams> params);
	std::shared_ptr<AccountParams> getAccountParams ();

	// Setters
	void setAuthFailure (int authFailure);
	void setRegisterChanged (bool registerChanged);
	void setDeletionDate (time_t deletionDate);
	void setUserData (void *userData);
	void setErrorInfo (LinphoneErrorInfo *errorInfo);
	void setContactAddress (LinphoneAddress *contact);
	void setContactAddressWithoutParams (LinphoneAddress *contact);
	void setPendingContactAddress (LinphoneAddress *contact);
	void setServiceRouteAddress (LinphoneAddress *serviceRoute);
	void setState (LinphoneRegistrationState state);
	void setOp (SalRegisterOp *op);
	void setCustomheader (const std::string& headerName, const std::string& headerValue);
	void setPresencePublishEvent (LinphoneEvent *presencePublishEvent);
	void setDependency (std::shared_ptr<Account> dependency);

	// Getters
	int getAuthFailure () const;
	bool hasRegisterChanged () const;
	time_t getDeletionDate () const;
	void* getUserData () const;
	LinphoneCore* getCore () const;
	LinphoneErrorInfo* getErrorInfo () const;
	LinphoneAddress* getContactAddress () const;
	LinphoneAddress* getContactAddressWithoutParams () const;
	LinphoneAddress* getPendingContactAddress () const;
	LinphoneAddress* getServiceRouteAddress () const;
	LinphoneRegistrationState getState () const;
	SalRegisterOp* getOp() const;
	const std::string getCustomHeader (const std::string& headerName) const;
	LinphoneEvent* getPresencePublishEvent () const;
	std::shared_ptr<Account> getDependency () const;

	// Other
	void refreshRegister ();
	void pauseRegister ();
	LinphoneReason getError () const;
	const std::string getTransport () const;
	bool avpfEnabled () const;
	const LinphoneAuthInfo* findAuthInfo () const;
	int getUnreadChatMessageCount () const;

private:
	std::shared_ptr<AccountParams> mParams;

	int mAuthFailure;

	bool mRegisterChanged;

	time_t mDeletionDate;

	void *mUserData;

	LinphoneCore *mCore;

	LinphoneErrorInfo *mErrorInfo;

	LinphoneAddress *mContactAddress;
	LinphoneAddress *mContactAddressWithoutParams;
	LinphoneAddress *mPendingContactAddress;
	LinphoneAddress *mServiceRouteAddress;

	LinphoneRegistrationState mState;

	SalRegisterOp *mOp;
	SalCustomHeader *mSentHeaders;

	LinphoneEvent *mPresencePublishEvent;

	std::shared_ptr<Account> mDependency;
};

LINPHONE_END_NAMESPACE

#endif // ifndef _L_ACCOUNT_H_
