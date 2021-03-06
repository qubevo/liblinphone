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

#ifndef _L_C_TYPES_H_
#define _L_C_TYPES_H_

// TODO: Remove me in the future.
#include "linphone/types.h"

#include "linphone/enums/call-enums.h"
#include "linphone/enums/chat-message-enums.h"
#include "linphone/enums/chat-room-enums.h"
#include "linphone/enums/conference-enums.h"
#include "linphone/enums/encryption-engine-enums.h"
#include "linphone/enums/event-log-enums.h"
#include "linphone/enums/security-event-enums.h"
#include "linphone/utils/enum-generator.h"

// =============================================================================

#ifdef __cplusplus
	extern "C" {
#endif // ifdef __cplusplus

// =============================================================================
// Misc.
// =============================================================================

#ifdef TRUE
	#undef TRUE
#endif

#ifdef FALSE
	#undef FALSE
#endif

#define TRUE 1
#define FALSE 0

// =============================================================================
// C Structures.
// =============================================================================

// -----------------------------------------------------------------------------
// Address.
// -----------------------------------------------------------------------------

/**
 * Object that represents a SIP address.
 *
 * The #LinphoneAddress is an opaque object to represents SIP addresses, ie
 * the content of SIP's 'from' and 'to' headers.
 * A SIP address is made of display name, username, domain name, port, and various
 * uri headers (such as tags). It looks like 'Alice <sip:alice@example.net>'.
 * The #LinphoneAddress has methods to extract and manipulate all parts of the address.
 * When some part of the address (for example the username) is empty, the accessor methods
 * return NULL.
 * @ingroup linphone_address
 */
typedef struct _LinphoneAddress LinphoneAddress;

// -----------------------------------------------------------------------------
// Call.
// -----------------------------------------------------------------------------

/**
 * The #LinphoneCall object represents a call issued or received by the #LinphoneCore
 * @ingroup call_control
 */
typedef struct _LinphoneCall LinphoneCall;

/** Callback prototype */
typedef void (*LinphoneCallCbFunc) (LinphoneCall *call, void *ud);

/**
 * That class holds all the callbacks which are called by #LinphoneCall objects.
 *
 * Use linphone_factory_create_call_cbs() to create an instance. Then, call the
 * callback setters on the events you need to monitor and pass the object to
 * a #LinphoneCall instance through linphone_call_add_callbacks().
 * @ingroup call_control
 */
typedef struct _LinphoneCallCbs LinphoneCallCbs;

// -----------------------------------------------------------------------------
// ChatRoom.
// -----------------------------------------------------------------------------

/**
 * An object to handle the callbacks for the handling a #LinphoneConference objects.
 *
 * Use linphone_factory_create_conference_cbs() to create an instance. Then, call the
 * callback setters on the events you need to monitor and pass the object to
 * a #LinphoneConferece instance through linphone_conference_add_callbacks().
 * @ingroup conferencing
 */
typedef struct _LinphoneConferenceCbs LinphoneConferenceCbs;

// -----------------------------------------------------------------------------
// ChatRoom.
// -----------------------------------------------------------------------------

/**
 * An chat message is the object that is sent and received through LinphoneChatRooms.
 * @ingroup chatroom
 */
typedef struct _LinphoneChatMessage LinphoneChatMessage;

/**
 * An object to handle the callbacks for the handling a #LinphoneChatMessage objects.
 * @ingroup chatroom
 */
typedef struct _LinphoneChatMessageCbs LinphoneChatMessageCbs;

/**
 * A chat room is the place where text messages are exchanged.
 * Can be created by linphone_core_create_chat_room().
 * @ingroup chatroom
 */
typedef struct _LinphoneChatRoom LinphoneChatRoom;

/**
 * An object to handle a chat room parameters.
 * Can be created with linphone_core_get_default_chat_room_params() or linphone_chat_room_params_new().
 * @ingroup chatroom
 */
typedef struct _LinphoneChatRoomParams LinphoneChatRoomParams;

/**
 * A mask of #LinphoneChatRoomCapabilities
 * @ingroup chatroom
 */
typedef int LinphoneChatRoomCapabilitiesMask;

/**
 * An object to handle the callbacks for the handling a #LinphoneChatRoom objects.
 * @ingroup chatroom
 */
typedef struct _LinphoneChatRoomCbs LinphoneChatRoomCbs;

// -----------------------------------------------------------------------------
// EventLog.
// -----------------------------------------------------------------------------

/**
 * Base object of events.
 * @ingroup events
 */
typedef struct _LinphoneEventLog LinphoneEventLog;

// -----------------------------------------------------------------------------
// Misc.
// -----------------------------------------------------------------------------

/**
 * The LinphoneContent object holds data that can be embedded in a signaling message.
 * @ingroup misc
 */
typedef struct _LinphoneContent LinphoneContent;

/**
 * Represents a dial plan
 * @ingroup misc
 */
typedef struct _LinphoneDialPlan LinphoneDialPlan;

/**
 * A #LinphoneMagicSearch is used to do specifics searchs
 * @ingroup misc
 */
typedef struct _LinphoneMagicSearch LinphoneMagicSearch;

/**
 * @ingroup misc
 */
typedef struct _LinphoneParticipant LinphoneParticipant;

/**
 * The LinphoneParticipantImdnState object represents the state of chat message for a participant of a conference chat room.
 * @ingroup misc
 */
typedef struct _LinphoneParticipantImdnState LinphoneParticipantImdnState;

/**
 * @ingroup misc
 */
typedef struct _LinphoneParticipantDeviceIdentity LinphoneParticipantDeviceIdentity;

/**
 * The LinphoneSearchResult object represents a result of a search
 * @ingroup misc
 */
typedef struct _LinphoneSearchResult LinphoneSearchResult;

// =============================================================================
// C Enums.
// =============================================================================

// -----------------------------------------------------------------------------
// How-to: Declare one enum
//
// 1. Declare a macro like this example in include/linphone/enums/chat-message-enums.h:
//
// #define L_ENUM_VALUES_CHAT_MESSAGE_DIRECTION(F) \ //
//   F(Incoming /**< Incoming message */) \ //
//   F(Outgoing /**< Outgoing message */)
//
// 2. And in this file, call L_DECLARE_C_ENUM with the enum name and values as params:
//
// L_DECLARE_C_ENUM(ChatMessageDirection, L_ENUM_VALUES_CHAT_MESSAGE_DIRECTION);
//
// 3. Do not forget to replace each single quote (with &apos;) or other special char like
// to an escaped sequence. Otherwise you get this error at compilation:
//
// [ 99%] Building CXX object wrappers/cpp/CMakeFiles/linphone++.dir/src/linphone++.cc.o
// c++: error: WORK/desktop/Build/linphone/wrappers/cpp/src/linphone++.cc: No such file or directory
// c++: fatal error: no input files
// compilation terminated.
// -----------------------------------------------------------------------------

#ifdef __cplusplus
	}
#endif // ifdef __cplusplus

#endif // ifndef _L_C_TYPES_H_
