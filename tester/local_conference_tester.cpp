/*
 * Copyright (c) 2010-2019 Belledonne Communications SARL.
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

#include "linphone/core.h"
#include "linphone/api/c-types.h"
#include "linphone/api/c-chat-room-params.h"
#include "tester_utils.h"
#include "linphone/wrapper_utils.h"
#include "liblinphone_tester.h"
#include "bctoolbox/crypto.h"
#include <map>
#include "address/identity-address.h"
#include "address/address.h"
#include "c-wrapper/c-wrapper.h"
#include "chat/chat-room/chat-room.h"
#include "core/core.h"
#include "conference/participant.h"
#include "address/identity-address.h"

#if __clang__ || ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic push
#endif
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

using namespace LinphonePrivate;
using namespace std;
namespace LinphoneTest {

class BcAssert {
public:
	void addCustomIterate(const std::function<void ()> &iterate) {
		mIterateFuncs.push_back(iterate);
	}
	bool waitUntil( std::chrono::duration<double> timeout ,const std::function<bool ()> &condition) {
		auto start = std::chrono::steady_clock::now();
		
		bool_t result;
		while (!(result = condition()) && (std::chrono::steady_clock::now() - start < timeout)) {
			for (const std::function<void ()>& iterate:mIterateFuncs) {
				iterate();
			}
			ms_usleep(100);
		}
		return result;
	}
	bool wait(const std::function<bool ()> &condition) {
		return waitUntil(std::chrono::seconds(10),condition);
	}
private:
	std::list<std::function<void ()>> mIterateFuncs;
};

class CoreAssert : public BcAssert {
public:
	CoreAssert(std::initializer_list<std::shared_ptr<Core>> cores) {
		for (shared_ptr<Core> core: cores) {
			addCustomIterate([core] {
				linphone_core_iterate(L_GET_C_BACK_PTR(core));
			});
		}
	}
};

class ClientConference;

class CoreManager {
public:
	CoreManager(std::string rc) {
		mMgr = linphone_core_manager_new(rc.c_str());
		mMgr->user_info = this;
	}

	CoreManager(std::string rc,const std::function<void ()> &preStart):mPreStart(preStart) {
		mMgr = linphone_core_manager_create(rc.c_str());
		mMgr->user_info = this;
		mPreStart();
		linphone_core_manager_start(mMgr,TRUE);
	}

	~CoreManager() {
		IdentityAddress id = getIdentity();
		linphone_core_manager_destroy(mMgr);
		lInfo() << "Core manager for [" << id << "] destroyed";
	}
	LinphoneCore * getLc() const {
		return mMgr->lc;
	}
	Core & getCore() const {
		return *L_GET_CPP_PTR_FROM_C_OBJECT(mMgr->lc);
	}
	IdentityAddress getIdentity() {
		return *L_GET_CPP_PTR_FROM_C_OBJECT(mMgr->identity);
	}
	stats & getStats() {
		return mMgr->stat;
	}
	LinphoneCoreManager * getCMgr() {
		return mMgr;
	}
	void reStart() {
		linphone_core_manager_reinit(mMgr);
		mPreStart();
		linphone_core_manager_start(mMgr, TRUE);
	}
protected:
	LinphoneCoreManager * mMgr;
private:
	CoreManager(const CoreManager&) {};
	const std::function<void ()> mPreStart = []{return;};
};


class CoreManagerAssert : public BcAssert {
public:
	CoreManagerAssert(std::initializer_list<std::reference_wrapper<CoreManager>> coreMgrs): BcAssert(){
		for (CoreManager &coreMgr: coreMgrs) {
			addCustomIterate([&coreMgr] {
				linphone_core_iterate(coreMgr.getLc());
			});
		}
	}
};
class Focus;

/*Core manager acting as a client*/
class ClientConference :public CoreManager {
public:
	ClientConference(std::string rc,Address factoryUri):CoreManager(rc,[this,factoryUri] {
		_configure_core_for_conference(mMgr,L_GET_C_BACK_PTR(&factoryUri));
		LinphoneCoreCbs *cbs = linphone_factory_create_core_cbs(linphone_factory_get());
		linphone_core_cbs_set_chat_room_state_changed(cbs, core_chat_room_state_changed);
		linphone_core_cbs_set_chat_room_subject_changed(cbs, core_chat_room_subject_changed);
		linphone_core_add_callbacks(getLc(),cbs);
		linphone_core_cbs_unref(cbs);
		
	}),mFocus(nullptr) {
		
	}
	
	void deleteChatRoomSync(AbstractChatRoom& chatroom) {
		linphone_core_delete_chat_room(getLc(), L_GET_C_BACK_PTR(&chatroom));
		CoreManagerAssert({*mFocus,*this}).wait([&chatroom] {
			return chatroom.getState() == ChatRoom::State::Deleted;
		});
	}
	~ClientConference() {
		for (auto chatRoom :getCore().getChatRooms()) {
			deleteChatRoomSync(*chatRoom);
		}
	}
	friend Focus;
protected:
	void setFocus(CoreManager* myFocus) {
		mFocus = myFocus;
	}
private:
	CoreManager* mFocus;
};


/* Core manager acting as a focus*/
class Focus : public CoreManager {
public:
	Focus(std::string rc): CoreManager(rc) {
		linphone_config_set_int(linphone_core_get_config(getLc()), "misc", "hide_empty_chat_rooms", 0);
		linphone_core_enable_conference_server(getLc(),TRUE);
		linphone_core_cbs_set_chat_room_state_changed(linphone_core_get_first_callbacks(getLc()), server_core_chat_room_state_changed);
		linphone_proxy_config_set_conference_factory_uri(linphone_core_get_default_proxy_config(getLc()), getIdentity().asString().c_str());
	}
	~Focus() = default ;

	void registerAsParticipantDevice(ClientConference &otherMgr) {
		const LinphoneAddress *cAddr = linphone_proxy_config_get_contact(linphone_core_get_default_proxy_config(otherMgr.getLc()));
		IdentityAddress participantDevice(*L_GET_CPP_PTR_FROM_C_OBJECT(cAddr));
		IdentityAddress participant = participantDevice.getAddressWithoutGruu();
		mParticipantDevices.insert({participant,participantDevice});
		//to allow client conference to delete chatroom in its destructor
		otherMgr.setFocus(this);
		
	}
	
private:
	static void server_core_chat_room_state_changed (LinphoneCore *core, LinphoneChatRoom *cr, LinphoneChatRoomState state) {
		Focus *focus = (Focus*)(((LinphoneCoreManager *)linphone_core_get_user_data(core))->user_info);
		switch (state) {
			case LinphoneChatRoomStateInstantiated: {
			if (linphone_chat_room_get_conference_address(cr) == NULL) {
				char config_id[6];
				belle_sip_random_token(config_id,sizeof(config_id));
				const LinphoneAddress *cAddr = linphone_proxy_config_get_contact(linphone_core_get_default_proxy_config(focus->getLc()));
				Address conference_address = (*L_GET_CPP_PTR_FROM_C_OBJECT(cAddr));
				conference_address.setUriParam("conf-id",config_id);
				linphone_chat_room_set_conference_address(cr, L_GET_C_BACK_PTR(&conference_address));
				LinphoneChatRoomCbs *cbs = linphone_factory_create_chat_room_cbs(linphone_factory_get());
				linphone_chat_room_cbs_set_participant_registration_subscription_requested(cbs, participant_registration_subscription_requested);
				linphone_chat_room_add_callbacks(cr, cbs);
				linphone_chat_room_cbs_set_user_data(cbs, focus);
				linphone_chat_room_cbs_unref(cbs);
			}
				break;
		}
			default:
				break;
		}
	}
	static void participant_registration_subscription_requested(LinphoneChatRoom *cr, const LinphoneAddress *participantAddr) {
		Focus *focus = (Focus*)(linphone_chat_room_cbs_get_user_data(linphone_chat_room_get_current_callbacks(cr)));
		const IdentityAddress participant = *L_GET_CPP_PTR_FROM_C_OBJECT(participantAddr);
		bctbx_list_t * devices = NULL;
		auto participantRange = focus->mParticipantDevices.equal_range(participant);
		for (auto participantIt = participantRange.first; participantIt != participantRange.second; participantIt++ ) {
			LinphoneAddress *deviceAddr = linphone_address_new(participantIt->second.asString().c_str());
			devices = bctbx_list_append(devices, linphone_factory_create_participant_device_identity(linphone_factory_get(),deviceAddr,""));
			linphone_address_unref(deviceAddr);
		}
		Address participantAddress(participant);
		linphone_chat_room_set_participant_devices(cr,L_GET_C_BACK_PTR(&participantAddress),devices);
		bctbx_list_free_with_data(devices,(bctbx_list_free_func)belle_sip_object_unref);
	}

	std::multimap<IdentityAddress,IdentityAddress,std::less<IdentityAddress>> mParticipantDevices;
};



static void group_chat_room_creation_server (void) {
	Focus focus("chloe_rc");
	{//to make sure focus is destroyed after clients.
		ClientConference marie("marie_rc", focus.getIdentity());
		ClientConference pauline("pauline_rc", focus.getIdentity());
		
		focus.registerAsParticipantDevice(marie);
		focus.registerAsParticipantDevice(pauline);
		
		bctbx_list_t * coresList = bctbx_list_append(NULL, focus.getLc());
		coresList = bctbx_list_append(coresList, marie.getLc());
		coresList = bctbx_list_append(coresList, pauline.getLc());
		Address paulineAddr(pauline.getIdentity());
		bctbx_list_t *participantsAddresses = bctbx_list_append(NULL, linphone_address_ref(L_GET_C_BACK_PTR(&paulineAddr)));
		
		stats initialMarieStats = marie.getStats();
		stats initialPaulineStats = pauline.getStats();
		
		// Marie creates a new group chat room
		const char *initialSubject = "Colleagues";
		LinphoneChatRoom *marieCr = create_chat_room_client_side(coresList, marie.getCMgr(), &initialMarieStats, participantsAddresses, initialSubject, FALSE);
		const LinphoneAddress *confAddr = linphone_chat_room_get_conference_address(marieCr);
		
		// Check that the chat room is correctly created on Pauline's side and that the participants are added
		LinphoneChatRoom *paulineCr = check_creation_chat_room_client_side(coresList, pauline.getCMgr(), &initialPaulineStats, confAddr, initialSubject, 1, FALSE);
		
		
		// Marie now changes the subject
		const char *newSubject = "Let's go drink a beer";
		linphone_chat_room_set_subject(marieCr, newSubject);
		BC_ASSERT_TRUE(wait_for_list(coresList, &marie.getStats().number_of_subject_changed, initialMarieStats.number_of_subject_changed + 1, 5000));
		BC_ASSERT_TRUE(wait_for_list(coresList, &pauline.getStats().number_of_subject_changed, initialPaulineStats.number_of_subject_changed + 1, 5000));
		BC_ASSERT_STRING_EQUAL(linphone_chat_room_get_subject(marieCr), newSubject);
		BC_ASSERT_STRING_EQUAL(linphone_chat_room_get_subject(paulineCr), newSubject);
		
		linphone_chat_room_leave(paulineCr);
	}
}


static void group_chat_room_server_deletion (void) {
	Focus focus("chloe_rc");
	{//to make sure focus is destroyed after clients.
		ClientConference marie("marie_rc", focus.getIdentity());
		ClientConference pauline("pauline_rc", focus.getIdentity());
		
		focus.registerAsParticipantDevice(marie);
		focus.registerAsParticipantDevice(pauline);
		
		bctbx_list_t * coresList = bctbx_list_append(NULL, focus.getLc());
		coresList = bctbx_list_append(coresList, marie.getLc());
		coresList = bctbx_list_append(coresList, pauline.getLc());
		Address paulineAddr(pauline.getIdentity());
		bctbx_list_t *participantsAddresses = bctbx_list_append(NULL, linphone_address_ref(L_GET_C_BACK_PTR(&paulineAddr)));
		
		stats initialMarieStats = marie.getStats();
		stats initialPaulineStats = pauline.getStats();
		
		// Marie creates a new group chat room
		const char *initialSubject = "Colleagues";
		LinphoneChatRoom *marieCr = create_chat_room_client_side(coresList, marie.getCMgr(), &initialMarieStats, participantsAddresses, initialSubject, FALSE);
		const LinphoneAddress *confAddr = linphone_chat_room_get_conference_address(marieCr);
		
		// Check that the chat room is correctly created on Pauline's side and that the participants are added
		LinphoneChatRoom *paulineCr = check_creation_chat_room_client_side(coresList, pauline.getCMgr(), &initialPaulineStats, confAddr, initialSubject, 1, FALSE);
		
		/*BC_ASSERT_TRUE(*/CoreManagerAssert({focus,marie,pauline}).wait([&focus] {
			for (auto chatRoom :focus.getCore().getChatRooms()) {
				for (auto participant: chatRoom->getParticipants()) {
					for (auto device: participant->getDevices())
						if (device->getState() != ParticipantDevice::State::Present) {
							return false;
						}
				}
			}
			return true;
		})/*)*/;
		
		
		LinphoneChatMessage *msg = linphone_chat_room_create_message(marieCr, "message blabla");
		linphone_chat_message_send(msg);
		BC_ASSERT_TRUE(CoreManagerAssert({focus,marie,pauline}).wait([msg] {
			return (linphone_chat_message_get_state(msg) == LinphoneChatMessageStateDelivered);
		}));
		BC_ASSERT_TRUE(CoreManagerAssert({focus,marie,pauline}).wait([paulineCr] {
			return linphone_chat_room_get_unread_messages_count(paulineCr) == 1;
		}));
		
		
		for (auto chatRoom :focus.getCore().getChatRooms()) {
			for (auto participant: chatRoom->getParticipants()) {
				//  force deletion by removing devices
				bctbx_list_t *empty = bctbx_list_new(NULL);
				Address participantAddress(participant->getAddress());
				linphone_chat_room_set_participant_devices(  L_GET_C_BACK_PTR(chatRoom)
														   , L_GET_C_BACK_PTR(&participantAddress)
														   , NULL);
				bctbx_list_free(empty);
			}
		}
		
		//wait until chatroom is deleted server side
		BC_ASSERT_TRUE(CoreManagerAssert({focus,marie,pauline}).wait([&focus] {
			return focus.getCore().getChatRooms().size() == 0;
		}));
		
		//wait bit more to detect side effect if any
		CoreManagerAssert({focus,marie,pauline}).waitUntil(chrono::seconds(2),[] {
			return false;
		});
		
		// Clean db from chat room
		linphone_chat_message_unref(msg);
		
		bctbx_list_free(coresList);
	}
	
}
static void group_chat_room_server_deletion_with_rmt_lst_event_handler (void) {
	Focus focus("chloe_rc");
	{//to make sure focus is destroyed after clients.
		ClientConference marie("marie_rc", focus.getIdentity());
		ClientConference pauline("pauline_rc", focus.getIdentity());
		
		focus.registerAsParticipantDevice(marie);
		focus.registerAsParticipantDevice(pauline);
		
		bctbx_list_t * coresList = bctbx_list_append(NULL, focus.getLc());
		coresList = bctbx_list_append(coresList, marie.getLc());
		coresList = bctbx_list_append(coresList, pauline.getLc());
		Address paulineAddr(pauline.getIdentity());
		bctbx_list_t *participantsAddresses = bctbx_list_append(NULL, linphone_address_ref(L_GET_C_BACK_PTR(&paulineAddr)));
		
		stats initialMarieStats = marie.getStats();
		stats initialPaulineStats = pauline.getStats();
		
		// Marie creates a new group chat room
		const char *initialSubject = "Colleagues";
		LinphoneChatRoom *marieCr = create_chat_room_client_side(coresList, marie.getCMgr(), &initialMarieStats, participantsAddresses, initialSubject, FALSE);
		const LinphoneAddress *confAddr = linphone_chat_room_get_conference_address(marieCr);
		
		// Check that the chat room is correctly created on Pauline's side and that the participants are added
		LinphoneChatRoom *paulineCr = check_creation_chat_room_client_side(coresList, pauline.getCMgr(), &initialPaulineStats, confAddr, initialSubject, 1, FALSE);
		
		BC_ASSERT_TRUE(CoreManagerAssert({focus,marie,pauline}).wait([&focus] {
			for (auto chatRoom :focus.getCore().getChatRooms()) {
				for (auto participant: chatRoom->getParticipants()) {
					for (auto device: participant->getDevices())
						if (device->getState() != ParticipantDevice::State::Present) {
							return false;
						}
				}
			}
			return true;
		}));
		
		
		LinphoneChatMessage *msg = linphone_chat_room_create_message(marieCr, "message blabla");
		linphone_chat_message_send(msg);
		BC_ASSERT_TRUE(CoreManagerAssert({focus,marie,pauline}).wait([msg] {
			return (linphone_chat_message_get_state(msg) == LinphoneChatMessageStateDelivered);
		}));
		BC_ASSERT_TRUE(CoreManagerAssert({focus,marie,pauline}).wait([paulineCr] {
			return linphone_chat_room_get_unread_messages_count(paulineCr) == 1;
		}));
		
		
		//now with simulate foregound/backgroud switch to get a remote event handler list instead of a simple remote event handler
		linphone_core_enter_background(pauline.getLc());
		linphone_config_set_bool(linphone_core_get_config(pauline.getLc()), "misc", "conference_event_package_force_full_state",TRUE);
		CoreManagerAssert({focus, marie, pauline}).waitUntil(std::chrono::seconds(1),[ ]{return false;});
		
		unsigned int paulineCrNo = (unsigned int)bctbx_list_size(linphone_core_get_chat_rooms (pauline.getLc()));
		coresList = bctbx_list_remove(coresList, pauline.getLc());
		pauline.reStart();
		coresList = bctbx_list_append(coresList, pauline.getLc());
		
		// Wait for chat rooms to be recovered from the main DB
		BC_ASSERT_TRUE(wait_for_list(coresList, &pauline.getStats().number_of_LinphoneConferenceStateCreated, paulineCrNo, 5000));
		char *paulineDeviceIdentity = linphone_core_get_device_identity(pauline.getLc());
		LinphoneAddress *paulineLocalAddr = linphone_address_new(paulineDeviceIdentity);
		bctbx_free(paulineDeviceIdentity);
		paulineCr = linphone_core_search_chat_room(pauline.getLc(), NULL, paulineLocalAddr, confAddr, NULL);
		linphone_address_unref(paulineLocalAddr);
		BC_ASSERT_PTR_NOT_NULL(paulineCr);
		
		CoreManagerAssert({focus, marie, pauline}).waitUntil(std::chrono::seconds(1),[ ]{return false;});
		
		CoreManagerAssert({focus,marie}).waitUntil(std::chrono::seconds(2),[ ]{return false;});
		
		for (auto chatRoom :focus.getCore().getChatRooms()) {
			for (auto participant: chatRoom->getParticipants()) {
				//  force deletion by removing devices
				bctbx_list_t *empty = bctbx_list_new(NULL);
				Address participantAddress(participant->getAddress());
				linphone_chat_room_set_participant_devices(  L_GET_C_BACK_PTR(chatRoom)
														   , L_GET_C_BACK_PTR(&participantAddress)
														   , NULL);
				bctbx_list_free(empty);
			}
		}
		
		//wait until chatroom is deleted server side
		BC_ASSERT_TRUE(CoreManagerAssert({focus,marie,pauline}).wait([&focus] {
			return focus.getCore().getChatRooms().size() == 0;
		}));
		
		//wait bit more to detect side effect if any
		CoreManagerAssert({focus,marie,pauline}).waitUntil(chrono::seconds(2),[] {
			return false;
		});
		
		//to avoid creation attempt of a new chatroom
		linphone_proxy_config_set_conference_factory_uri(linphone_core_get_default_proxy_config(focus.getLc()), NULL);
		
		linphone_chat_message_unref(msg);
		
		
		bctbx_list_free(coresList);
	}
}

static void multidomain_group_chat_room (void) {
	Focus focusExampleDotOrg("chloe_rc");
	Focus focusAuth1DotExampleDotOrg("arthur_rc");
	{ //to make sure focus is destroyed after clients.
		ClientConference marie("marie_rc", focusExampleDotOrg.getIdentity());
		ClientConference pauline("pauline_rc", focusExampleDotOrg.getIdentity());
		ClientConference michelle("michelle_rc_udp", focusExampleDotOrg.getIdentity());
		
		focusExampleDotOrg.registerAsParticipantDevice(marie);
		focusExampleDotOrg.registerAsParticipantDevice(pauline);
		focusExampleDotOrg.registerAsParticipantDevice(michelle);
		
		bctbx_list_t * coresList = bctbx_list_append(NULL, focusExampleDotOrg.getLc());
		coresList = bctbx_list_append(coresList, marie.getLc());
		coresList = bctbx_list_append(coresList, pauline.getLc());
		coresList = bctbx_list_append(coresList, michelle.getLc());
		Address paulineAddr(pauline.getIdentity());
		bctbx_list_t *participantsAddresses = bctbx_list_append(NULL, linphone_address_ref(L_GET_C_BACK_PTR(&paulineAddr)));
		Address michelleAddr(michelle.getIdentity());
		participantsAddresses = bctbx_list_append(participantsAddresses, linphone_address_ref(L_GET_C_BACK_PTR(&michelleAddr)));
		
		stats initialMarieStats = marie.getStats();
		stats initialPaulineStats = pauline.getStats();
		stats initialMichelleStats = michelle.getStats();
		
		// Marie creates a new group chat room
		const char *initialSubject = "Colleagues";
		LinphoneChatRoom *marieCr = create_chat_room_client_side(coresList, marie.getCMgr(), &initialMarieStats, participantsAddresses, initialSubject, FALSE);
		LinphoneAddress *confAddr = linphone_address_clone(linphone_chat_room_get_conference_address(marieCr));
		
		// Check that the chat room is correctly created on Pauline's side and that the participants are added
		LinphoneChatRoom *paulineCr = check_creation_chat_room_client_side(coresList, pauline.getCMgr(), &initialPaulineStats, confAddr, initialSubject, 2, FALSE);
		
		LinphoneChatRoom *michelleCr = check_creation_chat_room_client_side(coresList, michelle.getCMgr(), &initialMichelleStats, confAddr, initialSubject, 2, FALSE);
		
		if (paulineCr || michelleCr) {
			//throw BCTBX_EXCEPTION << "Cannot create chatroom giving up";
			//goto end;
		}
		BC_ASSERT_TRUE(CoreManagerAssert({focusExampleDotOrg,marie,pauline,michelle}).wait([&focusExampleDotOrg] {
			for (auto chatRoom :focusExampleDotOrg.getCore().getChatRooms()) {
				for (auto participant: chatRoom->getParticipants()) {
					for (auto device: participant->getDevices())
						if (device->getState() != ParticipantDevice::State::Present) {
							return false;
						}
				}
			}
			return true;
		}));
		
		
		LinphoneChatMessage *msg = linphone_chat_room_create_message(marieCr, "message blabla");
		linphone_chat_message_send(msg);
		BC_ASSERT_TRUE(CoreManagerAssert({focusExampleDotOrg,marie,pauline,michelle}).wait([msg] {
			return (linphone_chat_message_get_state(msg) == LinphoneChatMessageStateDelivered);
		}));
		BC_ASSERT_TRUE(CoreManagerAssert({focusExampleDotOrg,marie,pauline,michelle}).wait([paulineCr] {
			return linphone_chat_room_get_unread_messages_count(paulineCr) == 1;
		}));
		BC_ASSERT_TRUE(CoreManagerAssert({focusExampleDotOrg,marie,pauline,michelle}).wait([michelleCr] {
			return linphone_chat_room_get_unread_messages_count(michelleCr) == 1;
		}));
		linphone_chat_message_unref(msg);
		
		
		// now change focus in order to get conference with multiple domain.

		
		focusAuth1DotExampleDotOrg.registerAsParticipantDevice(marie);
		focusAuth1DotExampleDotOrg.registerAsParticipantDevice(pauline);
		focusAuth1DotExampleDotOrg.registerAsParticipantDevice(michelle);
		
		//change conference factory uri
		Address focusAuth1DotExampleDotOrgFactoryAddress = focusAuth1DotExampleDotOrg.getIdentity();
		_configure_core_for_conference(marie.getCMgr(),L_GET_C_BACK_PTR(&focusAuth1DotExampleDotOrgFactoryAddress));
		setup_mgr_for_conference(marie.getCMgr());
		_configure_core_for_conference(pauline.getCMgr(),L_GET_C_BACK_PTR(&focusAuth1DotExampleDotOrgFactoryAddress));
		setup_mgr_for_conference(pauline.getCMgr());
		_configure_core_for_conference(michelle.getCMgr(),L_GET_C_BACK_PTR(&focusAuth1DotExampleDotOrgFactoryAddress));
		setup_mgr_for_conference(michelle.getCMgr());
		
		coresList = bctbx_list_append(coresList, focusAuth1DotExampleDotOrg.getLc());
		initialMarieStats = marie.getStats();
		initialPaulineStats = pauline.getStats();
		initialMichelleStats = michelle.getStats();
		participantsAddresses = bctbx_list_append(NULL, linphone_address_ref(L_GET_C_BACK_PTR(&paulineAddr)));
		participantsAddresses = bctbx_list_append(participantsAddresses, linphone_address_ref(L_GET_C_BACK_PTR(&michelleAddr)));
		
		LinphoneChatRoom *marieCrfocusAuth1DotExampleDotOrg = create_chat_room_client_side(coresList, marie.getCMgr(), &initialMarieStats, participantsAddresses, initialSubject, FALSE);
		LinphoneAddress *confAddrfocusAuth1DotExampleDotOrg = linphone_address_clone(linphone_chat_room_get_conference_address(marieCrfocusAuth1DotExampleDotOrg));
		
		// Check that the chat room is correctly created on Pauline's side and that the participants are added
		LinphoneChatRoom *paulineCrfocusAuth1DotExampleDotOrg = check_creation_chat_room_client_side(coresList, pauline.getCMgr(), &initialPaulineStats, confAddrfocusAuth1DotExampleDotOrg, initialSubject, 2, FALSE);
		BC_ASSERT_PTR_NOT_NULL(paulineCrfocusAuth1DotExampleDotOrg);
		LinphoneChatRoom *michelleCrfocusAuth1DotExampleDotOrg = check_creation_chat_room_client_side(coresList, michelle.getCMgr(), &initialMichelleStats, confAddrfocusAuth1DotExampleDotOrg, initialSubject, 2, FALSE);
		BC_ASSERT_PTR_NOT_NULL(michelleCrfocusAuth1DotExampleDotOrg);
		
		
		BC_ASSERT_TRUE(CoreManagerAssert({focusAuth1DotExampleDotOrg,marie,pauline,michelle}).wait([&focusAuth1DotExampleDotOrg] {
			for (auto chatRoom :focusAuth1DotExampleDotOrg.getCore().getChatRooms()) {
				for (auto participant: chatRoom->getParticipants()) {
					for (auto device: participant->getDevices())
						if (device->getState() != ParticipantDevice::State::Present) {
							return false;
						}
				}
			}
			return true;
		}));
		
		msg = linphone_chat_room_create_message(marieCrfocusAuth1DotExampleDotOrg, "message blabla");
		linphone_chat_message_send(msg);
		BC_ASSERT_TRUE(CoreManagerAssert({focusAuth1DotExampleDotOrg,marie,pauline,michelle}).wait([msg] {
			return (linphone_chat_message_get_state(msg) == LinphoneChatMessageStateDelivered);
		}));
		
		
		//great, now I want to see what happened if marie restart.
		
		coresList = bctbx_list_remove(coresList, marie.getLc());
		marie.reStart();
		setup_mgr_for_conference(marie.getCMgr());
		coresList = bctbx_list_append(coresList, marie.getLc());
		
		// Retrieve chat room
		LinphoneAddress *marieDeviceAddr =  linphone_address_clone(linphone_proxy_config_get_contact(linphone_core_get_default_proxy_config(marie.getLc())));
		marieCr = linphone_core_search_chat_room(marie.getLc(), NULL, marieDeviceAddr, confAddr, NULL);
		BC_ASSERT_PTR_NOT_NULL(marieCr);
		marieCrfocusAuth1DotExampleDotOrg = linphone_core_search_chat_room(marie.getLc(), NULL, marieDeviceAddr, confAddrfocusAuth1DotExampleDotOrg, NULL);
		BC_ASSERT_PTR_NOT_NULL(marieCrfocusAuth1DotExampleDotOrg);
		
		CoreManagerAssert({focusExampleDotOrg,marie,pauline,michelle}).waitUntil(chrono::seconds(2),[] {
			return false;
		});
		
		BC_ASSERT_TRUE(wait_for_list(coresList,&marie.getCMgr()->stat.number_of_LinphoneSubscriptionActive,2,10000));
		
		ClientConference laure("laure_tcp_rc", focusExampleDotOrg.getIdentity());
		coresList = bctbx_list_append(coresList, laure.getLc());
		Address laureAddr(laure.getIdentity());
		focusExampleDotOrg.registerAsParticipantDevice(laure);
		
		initialMarieStats = marie.getStats();
		initialPaulineStats = pauline.getStats();
		initialMichelleStats = michelle.getStats();
		stats initialLaureStats = laure.getStats();
		
		participantsAddresses = bctbx_list_append(NULL, linphone_address_ref(L_GET_C_BACK_PTR(&laureAddr)));
		linphone_chat_room_add_participants(marieCr, participantsAddresses);
		
		BC_ASSERT_TRUE(wait_for_list(coresList, &laure.getCMgr()->stat.number_of_LinphoneConferenceStateCreationPending, initialLaureStats.number_of_LinphoneConferenceStateCreationPending + 1, 5000));
		BC_ASSERT_TRUE(wait_for_list(coresList, &laure.getCMgr()->stat.number_of_LinphoneConferenceStateCreated, initialLaureStats.number_of_LinphoneConferenceStateCreated + 1, 5000));
		BC_ASSERT_TRUE(wait_for_list(coresList, &laure.getCMgr()->stat.number_of_LinphoneChatRoomConferenceJoined, initialLaureStats.number_of_LinphoneChatRoomConferenceJoined + 1, 5000));
		
		LinphoneChatRoom *laureCr = check_creation_chat_room_client_side(coresList, laure.getCMgr(), &initialLaureStats, confAddr, initialSubject, 3, FALSE);
		BC_ASSERT_PTR_NOT_NULL(laureCr);
		
		BC_ASSERT_TRUE(wait_for_list(coresList, &marie.getCMgr()->stat.number_of_participants_added, initialMarieStats.number_of_participants_added + 1, 5000));
		BC_ASSERT_TRUE(wait_for_list(coresList, &pauline.getCMgr()->stat.number_of_participants_added, initialPaulineStats.number_of_participants_added + 1, 5000));
		BC_ASSERT_TRUE(wait_for_list(coresList, &michelle.getCMgr()->stat.number_of_participants_added, initialMichelleStats.number_of_participants_added + 1, 5000));
		
		BC_ASSERT_EQUAL(linphone_chat_room_get_nb_participants(marieCr), 3, int, "%d");
		BC_ASSERT_EQUAL(linphone_chat_room_get_nb_participants(paulineCr), 3, int, "%d");
		BC_ASSERT_EQUAL(linphone_chat_room_get_nb_participants(michelleCr), 3, int, "%d");
		
		BC_ASSERT_TRUE(wait_for_list(coresList,&pauline.getCMgr()->stat.number_of_NotifyReceived,initialPaulineStats.number_of_NotifyReceived + 1,5000));
		BC_ASSERT_TRUE(wait_for_list(coresList,&michelle.getCMgr()->stat.number_of_NotifyReceived,initialMichelleStats.number_of_NotifyReceived + 1,5000));
		
		focusAuth1DotExampleDotOrg.registerAsParticipantDevice(laure);
		_configure_core_for_conference(laure.getCMgr(),L_GET_C_BACK_PTR(&focusAuth1DotExampleDotOrgFactoryAddress));
		
		participantsAddresses = bctbx_list_append(NULL, linphone_address_ref(L_GET_C_BACK_PTR(&laureAddr)));
		linphone_chat_room_add_participants(marieCrfocusAuth1DotExampleDotOrg, participantsAddresses);
		BC_ASSERT_TRUE(wait_for_list(coresList, &laure.getCMgr()->stat.number_of_LinphoneConferenceStateCreationPending, initialLaureStats.number_of_LinphoneConferenceStateCreationPending + 2, 5000));
		BC_ASSERT_TRUE(wait_for_list(coresList, &laure.getCMgr()->stat.number_of_LinphoneConferenceStateCreated, initialLaureStats.number_of_LinphoneConferenceStateCreated + 2, 5000));
		BC_ASSERT_TRUE(wait_for_list(coresList, &laure.getCMgr()->stat.number_of_LinphoneChatRoomConferenceJoined, initialLaureStats.number_of_LinphoneChatRoomConferenceJoined + 2, 5000));
		
		LinphoneChatRoom *laureCrfocusAuth1DotExampleDotOrg = check_creation_chat_room_client_side(coresList, laure.getCMgr(), &initialLaureStats, confAddrfocusAuth1DotExampleDotOrg, initialSubject, 3, FALSE);
		BC_ASSERT_PTR_NOT_NULL(laureCrfocusAuth1DotExampleDotOrg);
		
		BC_ASSERT_TRUE(wait_for_list(coresList,&pauline.getCMgr()->stat.number_of_NotifyReceived,initialPaulineStats.number_of_NotifyReceived + 2,5000));
		BC_ASSERT_TRUE(wait_for_list(coresList,&michelle.getCMgr()->stat.number_of_NotifyReceived,initialMichelleStats.number_of_NotifyReceived + 2,5000));
		
		BC_ASSERT_TRUE(wait_for_list(coresList, &marie.getCMgr()->stat.number_of_participants_added, initialMarieStats.number_of_participants_added + 2, 5000));
		BC_ASSERT_TRUE(wait_for_list(coresList, &pauline.getCMgr()->stat.number_of_participants_added, initialPaulineStats.number_of_participants_added + 2, 5000));
		BC_ASSERT_TRUE(wait_for_list(coresList, &michelle.getCMgr()->stat.number_of_participants_added, initialMichelleStats.number_of_participants_added + 2, 5000));
		
		BC_ASSERT_EQUAL(linphone_chat_room_get_nb_participants(marieCrfocusAuth1DotExampleDotOrg), 3, int, "%d");
		BC_ASSERT_EQUAL(linphone_chat_room_get_nb_participants(paulineCrfocusAuth1DotExampleDotOrg), 3, int, "%d");
		BC_ASSERT_EQUAL(linphone_chat_room_get_nb_participants(michelleCrfocusAuth1DotExampleDotOrg), 3, int, "%d");
		
		linphone_chat_message_unref(msg);
		
		bctbx_list_free(coresList);
	}
	
}


}

static test_t local_conference_tests[] = {
	TEST_NO_TAG("Group chat room creation local server", LinphoneTest::group_chat_room_creation_server),
	TEST_NO_TAG("Group chat Server chat room deletion", LinphoneTest::group_chat_room_server_deletion),
	TEST_ONE_TAG("Group chat Server chat room deletion with remote list event handler", LinphoneTest::group_chat_room_server_deletion_with_rmt_lst_event_handler,"LeaksMemory"), /* beacause of coreMgr restart*/
	TEST_ONE_TAG("Multi domain chatroom", LinphoneTest::multidomain_group_chat_room,"LeaksMemory"), /* beacause of coreMgr restart*/
};

test_suite_t local_conference_test_suite = {
	"Local conference tester",
	NULL,
	NULL,
	liblinphone_tester_before_each,
	liblinphone_tester_after_each,
	sizeof(local_conference_tests) / sizeof(local_conference_tests[0]), local_conference_tests
};
#if __clang__ || ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic pop
#endif
