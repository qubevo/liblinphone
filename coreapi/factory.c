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

#include "linphone/factory.h"

#include "c-wrapper/c-wrapper.h"

#include "address/address.h"
#include "core/paths/paths.h"
#include "bctoolbox/vfs_encrypted.hh"
#include "bctoolbox/crypto.h"
#include "sqlite3_bctbx_vfs.h"

// TODO: From coreapi. Remove me later.
#include "private.h"

#ifndef PACKAGE_SOUND_DIR
	#define PACKAGE_SOUND_DIR "."
#endif
#ifndef PACKAGE_RING_DIR
	#define PACKAGE_RING_DIR "."
#endif

#ifndef PACKAGE_DATA_DIR
	#define PACKAGE_DATA_DIR "."
#endif

extern LinphoneAddress *_linphone_address_new(const char *addr);

typedef belle_sip_object_t_vptr_t LinphoneFactory_vptr_t;

struct _LinphoneFactory {
	belle_sip_object_t base;

	bctbx_list_t *supported_video_definitions;

	/*these are the directories set by the application*/
	char *top_resources_dir;
	char *data_resources_dir;
	char *sound_resources_dir;
	char *ring_resources_dir;
	char *image_resources_dir;
	char *msplugins_dir;

	/*these are the cached result computed from directories set by the application*/
	char *cached_data_resources_dir;
	char *cached_sound_resources_dir;
	char *cached_ring_resources_dir;
	char *cached_image_resources_dir;
	char *cached_msplugins_dir;
	char *cached_config_dir;
	char *cached_data_dir;
	char *cached_download_dir;
	LinphoneErrorInfo* ei;

	/* the EVFS encryption key */
	std::shared_ptr<std::vector<uint8_t>> evfs_master_key; // use a shared_ptr as _LinphoneFactory is not really an object and vector destructor end up never being called otherwise
	void *user_data;
};

static void linphone_factory_uninit(LinphoneFactory *obj){
	bctbx_list_free_with_data(obj->supported_video_definitions, (bctbx_list_free_func)linphone_video_definition_unref);

	STRING_RESET(obj->top_resources_dir);
	STRING_RESET(obj->data_resources_dir);
	STRING_RESET(obj->sound_resources_dir);
	STRING_RESET(obj->ring_resources_dir);
	STRING_RESET(obj->image_resources_dir);
	STRING_RESET(obj->msplugins_dir);

	STRING_RESET(obj->cached_data_resources_dir);
	STRING_RESET(obj->cached_sound_resources_dir);
	STRING_RESET(obj->cached_ring_resources_dir);
	STRING_RESET(obj->cached_image_resources_dir);
	STRING_RESET(obj->cached_msplugins_dir);

	STRING_RESET(obj->cached_config_dir);
	STRING_RESET(obj->cached_data_dir);
	STRING_RESET(obj->cached_download_dir);

	// sqlite3 vfs is registered at factory creation, so unregister it when destroying it
	sqlite3_bctbx_vfs_unregister();

	// proper cleaning of EVFS master key if any is set
	if (obj->evfs_master_key != nullptr) {
		bctbx_clean(obj->evfs_master_key->data(), obj->evfs_master_key->size());
		obj->evfs_master_key = nullptr;
	}
}

BELLE_SIP_DECLARE_NO_IMPLEMENTED_INTERFACES(LinphoneFactory);
BELLE_SIP_INSTANCIATE_VPTR(LinphoneFactory, belle_sip_object_t,
	linphone_factory_uninit, // destroy
	NULL, // clone
	NULL, // Marshall
	FALSE
);

static LinphoneFactory *_factory = NULL;

static void _linphone_factory_destroying_cb(void) {
	if (_factory != NULL) {
		belle_sip_object_unref(_factory);
		_factory = NULL;
	}
}

#define ADD_SUPPORTED_VIDEO_DEFINITION(factory, width, height, name) \
	(factory)->supported_video_definitions = bctbx_list_append((factory)->supported_video_definitions, \
		linphone_video_definition_new(width, height, name))

static void initialize_supported_video_definitions(LinphoneFactory *factory) {
#if !defined(__ANDROID__) && !TARGET_OS_IPHONE
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_1080P_W, MS_VIDEO_SIZE_1080P_H, "1080p");
#endif
#if !defined(__ANDROID__) && !TARGET_OS_MAC /*limit to most common sizes because mac video API cannot list supported resolutions*/
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_UXGA_W, MS_VIDEO_SIZE_UXGA_H, "uxga");
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_SXGA_MINUS_W, MS_VIDEO_SIZE_SXGA_MINUS_H, "sxga-");
#endif
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_720P_W, MS_VIDEO_SIZE_720P_H, "720p");
#if !defined(__ANDROID__) && !TARGET_OS_MAC
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_XGA_W, MS_VIDEO_SIZE_XGA_H, "xga");
#endif
#if !defined(__ANDROID__) && !TARGET_OS_IPHONE
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_SVGA_W, MS_VIDEO_SIZE_SVGA_H, "svga");
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_4CIF_W, MS_VIDEO_SIZE_4CIF_H, "4cif");
#endif
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_VGA_W, MS_VIDEO_SIZE_VGA_H, "vga");
#if TARGET_OS_IPHONE
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_IOS_MEDIUM_H, MS_VIDEO_SIZE_IOS_MEDIUM_W, "ios-medium");
#endif
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_CIF_W, MS_VIDEO_SIZE_CIF_H, "cif");
#if !TARGET_OS_MAC || TARGET_OS_IPHONE /* OS_MAC is 1 for iPhone, but we need QVGA */
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_QVGA_W, MS_VIDEO_SIZE_QVGA_H, "qvga");
#endif
	ADD_SUPPORTED_VIDEO_DEFINITION(factory, MS_VIDEO_SIZE_QCIF_W, MS_VIDEO_SIZE_QCIF_H, "qcif");
}

static LinphoneFactory *linphone_factory_new(void){
	LinphoneFactory *factory = belle_sip_object_new(LinphoneFactory);
	factory->top_resources_dir = bctbx_strdup(PACKAGE_DATA_DIR);
	initialize_supported_video_definitions(factory);
	/* register the bctbx sqlite vfs. It is not used by default */
	/* sqlite3_bctbx_vfs use the default bctbx_vfs, so if encryption is turned on by default, it will apply to sqlte3 db */
	sqlite3_bctbx_vfs_register(0);
	factory->evfs_master_key = nullptr;
	return factory;
}


LinphoneFactory *linphone_factory_get(void) {
	if (_factory == NULL) {
		_factory = linphone_factory_new();
		atexit(_linphone_factory_destroying_cb);
	}
	return _factory;
}

void linphone_factory_clean(void){
	LinphonePrivate::Address::clearSipAddressesCache();
	if (_factory){
		belle_sip_object_unref(_factory);
		_factory = NULL;
	}
}

static LinphoneCore *_linphone_factory_create_core (
	const LinphoneFactory *factory,
	LinphoneCoreCbs *cbs,
	const char *config_path,
	const char *factory_config_path,
	void *user_data,
	void *system_context,
	bool_t automatically_start
) {
	bctbx_init_logger(FALSE);
	LpConfig *config = linphone_config_new_with_factory(config_path, factory_config_path);
	LinphoneCore *lc = _linphone_core_new_with_config(cbs, config, user_data, system_context, automatically_start);
	linphone_config_unref(config);
	bctbx_uninit_logger();
	return lc;
}

static LinphoneCore *_linphone_factory_create_shared_core (
	const LinphoneFactory *factory,
	LinphoneCoreCbs *cbs,
	const char *config_filename,
	const char *factory_config_path,
	void *user_data,
	void *system_context,
	bool_t automatically_start,
	const char *app_group_id,
	bool_t main_core
) {
	bctbx_init_logger(FALSE);
	LpConfig *config = linphone_config_new_for_shared_core(app_group_id, config_filename, factory_config_path);
	LinphoneCore *lc = _linphone_core_new_shared_with_config(cbs, config, user_data, system_context, automatically_start, app_group_id, main_core);
	linphone_config_unref(config);
	bctbx_uninit_logger();
	return lc;
}

LinphoneCore *linphone_factory_create_core (
	const LinphoneFactory *factory,
	LinphoneCoreCbs *cbs,
	const char *config_path,
	const char *factory_config_path
) {
	return _linphone_factory_create_core(factory, cbs, config_path, factory_config_path, NULL, NULL, TRUE);
}

LinphoneCore *linphone_factory_create_core_2 (
	const LinphoneFactory *factory,
	LinphoneCoreCbs *cbs,
	const char *config_path,
	const char *factory_config_path,
	void *user_data,
	void *system_context
) {
	return _linphone_factory_create_core(factory, cbs, config_path, factory_config_path, user_data, system_context, TRUE);
}

LinphoneCore *linphone_factory_create_core_3 (
	const LinphoneFactory *factory,
	const char *config_path,
	const char *factory_config_path,
	void *system_context
) {
	return _linphone_factory_create_core(factory, NULL, config_path, factory_config_path, NULL, system_context, FALSE);
}

LinphoneCore *linphone_factory_create_shared_core (
	const LinphoneFactory *factory,
	const char *config_filename,
	const char *factory_config_path,
	void *system_context,
	const char *app_group_id,
	bool_t main_core
) {
	return _linphone_factory_create_shared_core(factory, NULL, factory_config_path, factory_config_path, NULL, system_context, FALSE, app_group_id, main_core);
}

LinphoneCore *linphone_factory_create_core_with_config (
	const LinphoneFactory *factory,
	LinphoneCoreCbs *cbs,
	LinphoneConfig *config
) {
	return _linphone_core_new_with_config(cbs, config, NULL, NULL, TRUE);
}

LinphoneCore *linphone_factory_create_core_with_config_2 (
	const LinphoneFactory *factory,
	LinphoneCoreCbs *cbs,
	LinphoneConfig *config,
	void *user_data,
	void *system_context
) {
	return _linphone_core_new_with_config(cbs, config, user_data, system_context, TRUE);
}

LinphoneCore *linphone_factory_create_core_with_config_3 (
	const LinphoneFactory *factory,
	LinphoneConfig *config,
	void *system_context
) {
	return _linphone_core_new_with_config(NULL, config, NULL, system_context, FALSE);
}

LinphoneCore *linphone_factory_create_shared_core_with_config (
	const LinphoneFactory *factory,
	LinphoneConfig *config,
	void *system_context,
	const char *app_group_id,
	bool_t main_core
) {
	return _linphone_core_new_shared_with_config(NULL, config, NULL, system_context, FALSE, app_group_id, main_core);
}

LinphoneCoreCbs *linphone_factory_create_core_cbs(const LinphoneFactory *factory) {
	return _linphone_core_cbs_new();
}

LinphoneAddress *linphone_factory_create_address(const LinphoneFactory *factory, const char *addr) {
	return linphone_address_new(addr);
}

LinphoneParticipantDeviceIdentity *linphone_factory_create_participant_device_identity(
	const LinphoneFactory *factory,
	const LinphoneAddress *address,
	const char *name
) {
#ifdef HAVE_ADVANCED_IM
	return linphone_participant_device_identity_new(address, name);
#else
	ms_warning("Advanced IM such as group chat is disabled");
	return NULL;
#endif
}

LinphoneAuthInfo *linphone_factory_create_auth_info(const LinphoneFactory *factory, const char *username, const char *userid, const char *passwd, const char *ha1, const char *realm, const char *domain) {
	return linphone_auth_info_new(username, userid, passwd, ha1, realm, domain);
}

LinphoneAuthInfo *linphone_factory_create_auth_info_2(const LinphoneFactory *factory, const char *username, const char *userid, const char *passwd, const char *ha1, const char *realm, const char *domain, const char *algorithm) {
	return linphone_auth_info_new_for_algorithm(username, userid, passwd, ha1, realm, domain, algorithm);
}

LinphoneCallCbs * linphone_factory_create_call_cbs(const LinphoneFactory *factory) {
	return _linphone_call_cbs_new();
}

LinphoneConferenceCbs * linphone_factory_create_conference_cbs(const LinphoneFactory *factory) {
	return _linphone_conference_cbs_new();
}

LinphoneChatRoomCbs * linphone_factory_create_chat_room_cbs(const LinphoneFactory *factory) {
	return _linphone_chat_room_cbs_new();
}

LinphoneChatMessageCbs * linphone_factory_create_chat_message_cbs(const LinphoneFactory *factory) {
	return linphone_chat_message_cbs_new();
}

LinphoneVcard *linphone_factory_create_vcard(LinphoneFactory *factory) {
	return _linphone_vcard_new();
}

LinphoneVideoDefinition * linphone_factory_create_video_definition(const LinphoneFactory *factory, unsigned int width, unsigned int height) {
	LinphoneVideoDefinition *supported = linphone_factory_find_supported_video_definition(factory, width, height);
	return supported ? linphone_video_definition_clone(supported) : linphone_video_definition_new(width, height, NULL);
}

LinphoneVideoDefinition * linphone_factory_create_video_definition_from_name(const LinphoneFactory *factory, const char *name) {
	unsigned int width = 0;
	unsigned int height = 0;
	LinphoneVideoDefinition *vdef = linphone_factory_find_supported_video_definition_by_name(factory, name);
	if (vdef != NULL) return linphone_video_definition_ref(vdef);
	if (sscanf(name, "%ux%u", &width, &height) == 2) {
		return linphone_video_definition_new(width, height, NULL);
	}
	return linphone_video_definition_new(0, 0, NULL);
}

const bctbx_list_t * linphone_factory_get_supported_video_definitions(const LinphoneFactory *factory) {
	return factory->supported_video_definitions;
}

LinphoneVideoDefinition * linphone_factory_find_supported_video_definition(const LinphoneFactory *factory, unsigned int width, unsigned int height) {
	const bctbx_list_t *item;
	const bctbx_list_t *supported = linphone_factory_get_supported_video_definitions(factory);
	LinphoneVideoDefinition *searched_vdef = linphone_video_definition_new(width, height, NULL);
	LinphoneVideoDefinition *found = NULL;

	for (item = supported; item != NULL; item = bctbx_list_next(item)) {
		LinphoneVideoDefinition *svdef = (LinphoneVideoDefinition *)bctbx_list_get_data(item);
		if (linphone_video_definition_equals(svdef, searched_vdef)) {
			found = svdef;
			break;
		}
	}
	linphone_video_definition_unref(searched_vdef);
	
	if (!found) {
		ms_warning("Couldn't find supported video definition for %ux%u", width, height);
	}
	return found;
}

LinphoneVideoDefinition * linphone_factory_find_supported_video_definition_by_name(const LinphoneFactory *factory, const char *name) {
	const bctbx_list_t *item;
	const bctbx_list_t *supported = linphone_factory_get_supported_video_definitions(factory);

	for (item = supported; item != NULL; item = bctbx_list_next(item)) {
		LinphoneVideoDefinition *svdef = (LinphoneVideoDefinition *)bctbx_list_get_data(item);
		if (strcmp(linphone_video_definition_get_name(svdef), name) == 0) {
			return svdef;
		}
	}

	ms_error("Couldn't find a supported video definition for name [%s]", name);
	return NULL;
}

const char * linphone_factory_get_top_resources_dir(const LinphoneFactory *factory) {
	return factory->top_resources_dir;
}

void linphone_factory_set_top_resources_dir(LinphoneFactory *factory, const char *path) {
	STRING_SET(factory->top_resources_dir, path);
}

const char * linphone_factory_get_data_resources_dir(LinphoneFactory *factory) {
	if (factory->data_resources_dir) return factory->data_resources_dir;
	if (factory->top_resources_dir){
		STRING_TRANSFER(factory->cached_data_resources_dir, bctbx_strdup_printf("%s/linphone", factory->top_resources_dir));
	}else{
		STRING_TRANSFER(factory->cached_data_resources_dir, bctbx_strdup_printf("%s/linphone", PACKAGE_DATA_DIR));
	}
	return factory->cached_data_resources_dir;
}

void linphone_factory_set_data_resources_dir(LinphoneFactory *factory, const char *path) {
	STRING_SET(factory->data_resources_dir, path);
}

const char * linphone_factory_get_sound_resources_dir(LinphoneFactory *factory) {
	if (factory->sound_resources_dir) return factory->sound_resources_dir;
	if (factory->top_resources_dir){
		STRING_TRANSFER(factory->cached_sound_resources_dir, bctbx_strdup_printf("%s/sounds/linphone", factory->top_resources_dir));
		return factory->cached_sound_resources_dir;
	}
	return PACKAGE_SOUND_DIR;
}

void linphone_factory_set_sound_resources_dir(LinphoneFactory *factory, const char *path) {
	STRING_SET(factory->sound_resources_dir, path);
}

const char * linphone_factory_get_ring_resources_dir(LinphoneFactory *factory) {
	if (factory->ring_resources_dir) return factory->ring_resources_dir;
	if (factory->sound_resources_dir){
		STRING_TRANSFER(factory->cached_ring_resources_dir, bctbx_strdup_printf("%s/rings", factory->sound_resources_dir));
		return factory->cached_ring_resources_dir;
	}
	if (factory->top_resources_dir) {
		STRING_TRANSFER(factory->cached_ring_resources_dir, bctbx_strdup_printf("%s/sounds/linphone/rings", factory->top_resources_dir));
		return factory->cached_ring_resources_dir;
	}
	return PACKAGE_RING_DIR;
}

void linphone_factory_set_ring_resources_dir(LinphoneFactory *factory, const char *path) {
	STRING_SET(factory->ring_resources_dir, path);
}

const char * linphone_factory_get_image_resources_dir(LinphoneFactory *factory) {
	if (factory->image_resources_dir) return factory->image_resources_dir;
	if (factory->top_resources_dir) {
		STRING_TRANSFER(factory->cached_image_resources_dir, bctbx_strdup_printf("%s/images", factory->top_resources_dir));
	}else{
		STRING_TRANSFER(factory->cached_image_resources_dir, bctbx_strdup_printf("%s/images", PACKAGE_DATA_DIR));
	}
	return factory->cached_image_resources_dir;
}

void linphone_factory_set_image_resources_dir(LinphoneFactory *factory, const char *path) {
	STRING_SET(factory->image_resources_dir, path);
}

const char * linphone_factory_get_msplugins_dir(LinphoneFactory *factory) {
	return factory->msplugins_dir;
}

void linphone_factory_set_msplugins_dir(LinphoneFactory *factory, const char *path) {
	STRING_SET(factory->msplugins_dir, path);
}

LinphoneErrorInfo *linphone_factory_create_error_info(LinphoneFactory *factory){

	return linphone_error_info_new();

}

LinphoneRange *linphone_factory_create_range(LinphoneFactory *factory) {
	return linphone_range_new();
}

LinphoneTransports *linphone_factory_create_transports(LinphoneFactory *factory) {
	return linphone_transports_new();
}

LinphoneVideoActivationPolicy *linphone_factory_create_video_activation_policy(LinphoneFactory *factory) {
	return linphone_video_activation_policy_new();
}

LinphoneContent *linphone_factory_create_content(LinphoneFactory *factory) {
	return linphone_content_new();
}

LinphoneBuffer *linphone_factory_create_buffer(LinphoneFactory *factory) {
	return linphone_buffer_new();
}

LinphoneBuffer *linphone_factory_create_buffer_from_data(LinphoneFactory *factory, const uint8_t *data, size_t size) {
	return linphone_buffer_new_from_data(data, size);
}

LinphoneBuffer *linphone_factory_create_buffer_from_string(LinphoneFactory *factory, const char *data) {
	return linphone_buffer_new_from_string(data);
}

LinphoneConfig *linphone_factory_create_config(LinphoneFactory *factory, const char *path) {
	return linphone_config_new(path);
}

LinphoneConfig *linphone_factory_create_config_with_factory(LinphoneFactory *factory, const char *path, const char *factory_path) {
	return linphone_config_new_with_factory(path, factory_path);
}

LinphoneConfig *linphone_factory_create_config_from_string(LinphoneFactory *factory, const char *data) {
	return linphone_config_new_from_buffer(data);
}

const bctbx_list_t * linphone_factory_get_dial_plans(const LinphoneFactory *factory) {
	return linphone_dial_plan_get_all_list();
}

void *linphone_factory_get_user_data(const LinphoneFactory *factory) {
	return factory->user_data;
}

void linphone_factory_set_user_data(LinphoneFactory *factory, void *data) {
	factory->user_data = data;
}

void linphone_factory_set_log_collection_path(LinphoneFactory *factory, const char *path) {
	linphone_core_set_log_collection_path(path);
}

void linphone_factory_enable_log_collection(LinphoneFactory *factory, LinphoneLogCollectionState state) {
	linphone_core_enable_log_collection(state);
}

LinphoneTunnelConfig *linphone_factory_create_tunnel_config(LinphoneFactory *factory) {
	return linphone_tunnel_config_new();
}

LinphoneLoggingServiceCbs *linphone_factory_create_logging_service_cbs(LinphoneFactory *factory) {
	return linphone_logging_service_cbs_new();
}

LinphonePlayerCbs *linphone_factory_create_player_cbs(LinphoneFactory *factory) {
	return linphone_player_cbs_new();
}

LinphoneEventCbs *linphone_factory_create_event_cbs(LinphoneFactory *factory) {
	return linphone_event_cbs_new();
}

LinphoneFriendListCbs *linphone_factory_create_friend_list_cbs(LinphoneFactory *factory) {
	return linphone_friend_list_cbs_new();
}

LinphoneAccountCreatorCbs *linphone_factory_create_account_creator_cbs(LinphoneFactory *factory) {
	return linphone_account_creator_cbs_new();
}

LinphoneXmlRpcRequestCbs *linphone_factory_create_xml_rpc_request_cbs(LinphoneFactory *factory) {
	return linphone_xml_rpc_request_cbs_new();
}

bool_t linphone_factory_is_chatroom_backend_available(LinphoneFactory *factory, LinphoneChatRoomBackend chatroom_backend) {
#ifdef HAVE_ADVANCED_IM
	return TRUE;
#else
	return (chatroom_backend != LinphoneChatRoomBackendFlexisipChat);
#endif
}

bool_t linphone_factory_is_database_storage_available(LinphoneFactory *factory) {
#ifdef HAVE_DB_STORAGE
	return TRUE;
#else
	return FALSE;
#endif
}

bool_t linphone_factory_is_imdn_available(LinphoneFactory *factory) {
#ifdef HAVE_ADVANCED_IM
	return TRUE;
#else
	return FALSE;
#endif
}

const char *linphone_factory_get_config_dir(LinphoneFactory *factory, void *context) {
	STRING_SET(factory->cached_config_dir, LinphonePrivate::Paths::getPath(LinphonePrivate::Paths::Config, context).c_str());
	return factory->cached_config_dir;
}

const char *linphone_factory_get_data_dir(LinphoneFactory *factory, void *context) {
	STRING_SET(factory->cached_data_dir, LinphonePrivate::Paths::getPath(LinphonePrivate::Paths::Data, context).c_str());
	return factory->cached_data_dir;
}

const char *linphone_factory_get_download_dir(LinphoneFactory *factory, void *context) {
	STRING_SET(factory->cached_download_dir, LinphonePrivate::Paths::getPath(LinphonePrivate::Paths::Download, context).c_str());
	return factory->cached_download_dir;
}

void linphone_factory_set_vfs_encryption(LinphoneFactory *factory, const uint16_t encryptionModule, const uint8_t *secret, const size_t secretSize) {

	/* Check encryptionMpdule is valid */
	auto module = bctoolbox::EncryptionSuite::unset;
	switch (encryptionModule) {
		case LINPHONE_VFS_ENCRYPTION_UNSET: // do not use the encrypted VFS
			bctbx_vfs_set_default(bctbx_vfs_get_standard());
			bctoolbox::VfsEncryption::openCallbackSet(nullptr);
			return;
		case LINPHONE_VFS_ENCRYPTION_PLAIN: // use the encrypted VFS but write plain files
			bctbx_warning("linphone_factory_set_vfs_encryption : encryptionModule set to plain text");
			module = bctoolbox::EncryptionSuite::plain;
			break;
		case LINPHONE_VFS_ENCRYPTION_DUMMY:
			module = bctoolbox::EncryptionSuite::dummy;
			break;
		case LINPHONE_VFS_ENCRYPTION_AES256GCM128_SHA256:
			module = bctoolbox::EncryptionSuite::aes256gcm128_sha256;
			break;
		default:
			bctbx_error("linphone_factory_set_vfs_encryption : encryptionModule %04x unknown", encryptionModule);
			return;
	}

	/* save the key */
	if (factory->evfs_master_key != nullptr) {
		bctbx_clean(factory->evfs_master_key->data(), factory->evfs_master_key->size());
	}
	factory->evfs_master_key = std::make_shared<std::vector<uint8_t>>(secret, secret+secretSize);

	// Set the default bctbx vfs to the encrypted one
	bctbx_vfs_set_default(&bctoolbox::bcEncryptedVfs);

	// Associate the VfsEncryption class callback
	bctoolbox::VfsEncryption::openCallbackSet([module](bctoolbox::VfsEncryption &settings) {
		bctbx_message("Encrypted VFS: Open file %s, encryption is set to %s file. Current file's encryption module is %s", settings.filenameGet().data(), encryptionSuiteString(module).data(), encryptionSuiteString(settings.encryptionSuiteGet()).data());

		settings.encryptionSuiteSet(module); // This call will migrate plain files to encrypted ones if needed
		if (module!=bctoolbox::EncryptionSuite::plain) { // do not set keys for plain module
			settings.secretMaterialSet(*(linphone_factory_get()->evfs_master_key));
		}
	});
}
