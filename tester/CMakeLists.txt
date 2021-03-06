############################################################################
# CMakeLists.txt
# Copyright (C) 2014  Belledonne Communications, Grenoble France
#
############################################################################
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
############################################################################

set(OTHER_LIBS_FOR_TESTER bctoolbox bctoolbox-tester ortp mediastreamer ${BELLESIP_TARGETNAME} belr ${XML2_LIBRARIES} ${XSD_LIBRARIES})

if(INTL_FOUND)
	list(APPEND OTHER_LIBS_FOR_TESTER ${INTL_LIBRARIES})
endif()
if(SQLITE3_FOUND)
	list(APPEND OTHER_LIBS_FOR_TESTER ${SQLITE3_LIBRARIES})
endif()
if(ZLIB_FOUND)
	if (ANDROID)
		# Starting NDK r21, libz.a has issues: https://github.com/android/ndk/issues/1179
		list(APPEND OTHER_LIBS_FOR_TESTER z)
	else()
		list(APPEND OTHER_LIBS_FOR_TESTER ${ZLIB_LIBRARIES})
	endif()
endif()

set(SOUND_FILES
	sounds/ahbahouaismaisbon.wav
	sounds/hello44100.wav
	sounds/hello8000.mkv
	sounds/hello8000_mkv_ref.wav
	sounds/hello8000.wav
	sounds/oldphone.wav
	sounds/ringback.wav
	sounds/sintel_trailer_opus_h264.mkv
	sounds/sintel_trailer_opus_vp8.mkv
	sounds/sintel_trailer_pcmu_h264.mkv
	sounds/vrroom.wav
)

set(SIPP_FILES
	sipp/call_invite_200ok_without_contact_header.xml
	sipp/call_with_audio_mline_before_video_in_sdp.xml
	sipp/call_with_multiple_audio_mline_in_sdp.xml
	sipp/call_with_multiple_video_mline_in_sdp.xml
	sipp/call_with_video_mline_before_audio_in_sdp.xml
	sipp/sip_update_within_icoming_reinvite_with_no_sdp.xml
)

set(CERTIFICATE_ALT_FILES
	certificates/altname/agent.pem
	certificates/altname/cafile.pem
	certificates/altname/openssl-altname.cnf
)

set(CERTIFICATE_CN_FILES
	certificates/cn/agent.pem
	certificates/cn/cafile.pem
	certificates/cn/openssl-cn.cnf
)

set(CERTIFICATE_CLIENT_FILES
	certificates/client/cert.pem
	certificates/client/key.pem
	certificates/client/cert2.pem
	certificates/client/key2.pem
	certificates/client/cert3.pem
	certificates/client/key3.pem
	certificates/client/cert2-signed-by-other-ca.pem
	certificates/client/user1_cert.pem
	certificates/client/user1_key.pem
	certificates/client/user1_multiple_aliases_cert.pem
	certificates/client/user1_multiple_aliases_key.pem
	certificates/client/user2_cert.pem
	certificates/client/user2_key.pem
	certificates/client/user2_CN_cert.pem
	certificates/client/user2_CN_key.pem
	certificates/client/user2_revoked_cert.pem
	certificates/client/user2_revoked_key.pem
)

set(CERTIFICATE_FILES ${CERTIFICATE_ALT_FILES} ${CERTIFICATE_CN_FILES} ${CERTIFICATE_CLIENT_FILES})

set(DB_FILES
	db/friends.db
	db/linphone.db
	db/messages.db
	db/chatrooms.db
)

set(RC_FILES
	rcfiles/account_creator_rc
	rcfiles/assistant_create.rc
	rcfiles/carddav_rc
	rcfiles/claire_rc
	rcfiles/claire_sips_rc
	rcfiles/chloe_lime_x3dh_rc
	rcfiles/chloe_rc
	rcfiles/conference_focus_rc
	rcfiles/empty_rc
	rcfiles/friends_rc
	rcfiles/groupchat_lime_x3dh_rc
	rcfiles/groupchat_rc
	rcfiles/invalid_friends_rc
	rcfiles/invalid_friends_xml
	rcfiles/laure_call_logs_rc
	rcfiles/laure_lime_x3dh_rc
	rcfiles/laure_rc_udp
	rcfiles/laure_tcp_rc
	rcfiles/lime_x3dh_xml_rc
	rcfiles/marie2_rc
	rcfiles/marie_bodyless_rc
	rcfiles/marie_dependent_proxy_rc
	rcfiles/marie_dual_proxy_2_rc
	rcfiles/marie_dual_proxy_rc
	rcfiles/marie_early_rc
	rcfiles/marie_h264_rc
	rcfiles/marie_lime_x3dh_no_server_rc
	rcfiles/marie_lime_x3dh_rc
	rcfiles/marie_nat64_rc
	rcfiles/marie_quality_reporting_rc
	rcfiles/marie_rc
	rcfiles/marie_rc_rtcp_xr
	rcfiles/marie_remote_404_rc
	rcfiles/marie_remote_default_values_rc
	rcfiles/marie_remote_https_rc
	rcfiles/marie_remote_invalid_rc
	rcfiles/marie_remote_invalid_uri_rc
	rcfiles/marie_remote_localfile2_rc
	rcfiles/marie_remote_localfile_android_rc
	rcfiles/marie_remote_localfile_rc
	rcfiles/marie_remote_localfile_win10_rc
	rcfiles/marie_remote_rc
	rcfiles/marie_session_timers_rc
	rcfiles/marie_sips_rc
	rcfiles/marie_transient_remote_rc
	rcfiles/marie_v4proxy_rc
	rcfiles/marie_well_known_port_rc
	rcfiles/marie_zrtp_aes256_rc
	rcfiles/marie_zrtp_b256_rc
	rcfiles/marie_zrtp_ecdh255_rc
	rcfiles/marie_zrtp_ecdh448_rc
	rcfiles/marie_zrtp_srtpsuite_aes256_rc
	rcfiles/michelle_rc_udp
	rcfiles/multi_account_rc
	rcfiles/pauline_alt_rc
	rcfiles/pauline_as_friend_rc
	rcfiles/pauline_external_rc
	rcfiles/pauline_h264_rc
	rcfiles/pauline_lime_x3dh_rc
	rcfiles/pauline_nat64_rc
	rcfiles/pauline_rc
	rcfiles/pauline_rc_rtcp_xr
	rcfiles/pauline_sips_rc
	rcfiles/pauline_tcp_rc
	rcfiles/pauline_tls_client_rc
	rcfiles/pauline_tunnel_verify_server_certificate_rc
	rcfiles/pauline_v4proxy_rc
	rcfiles/pauline_well_known_port_rc
	rcfiles/pauline_wild_rc
	rcfiles/pauline_zrtp_aes256_rc
	rcfiles/pauline_zrtp_b256_rc
	rcfiles/pauline_zrtp_ecdh255_rc
	rcfiles/pauline_zrtp_ecdh448_rc
	rcfiles/pauline_zrtp_srtpsuite_aes256_rc
	rcfiles/remote_zero_length_params_rc
	rcfiles/stun_rc
	rcfiles/zero_length_params_rc
	rcfiles/marie_bodyless_rc
	rcfiles/groupchat_rc
	rcfiles/groupchat_lime_x3dh_rc
	rcfiles/arthur_rc 
)

set(IMAGE_FILES
	images/linphone.svg
	images/linphonesiteqr.jpg
	images/linphonesiteqr_captured.jpg
	images/nowebcamCIF.jpg
	images/nowebcamVGA.jpg
)

set(VCARD_FILES
	vcards/thousand_vcards.vcf
	vcards/vcards.vcf
)

set(OTHER_FILES
	tester_hosts
	local_tester_hosts
)

set(IOS_RESOURCES_FILES
	${OTHER_FILES}
	${DB_FILES}
	certificates
	images
	rcfiles
	sipp
	sounds
	vcards
)

set(SOURCE_FILES_C
	account_creator_tester.c
	accountmanager.c
	audio_bypass_tester.c
	audio_routes_tester.c
	audio_video_conference_tester.c
	call_ice_tester.c
	call_multicast_tester.c
	call_multi_tester.c
	call_push_incoming_tester.c
	call_recovery_tester.c
	call_secure_tester.c
	call_single_tester.c
	call_video_tester.c
	call_with_rtp_bundle_tester.c
	complex_sip_case_tester.c
	dtmf_tester.c
	eventapi_tester.c
	external_domain_tester.c
	flexisip_tester.c
        group_chat_tester.c
	group_chat_ephemeral_tester.c
	group_chat_secure_tester.c
	group_chat_tester.c
	liblinphone_tester.c
	log_collection_tester.c
	message_tester.c
	offeranswer_tester.c
	player_tester.c
	presence_server_tester.c
	presence_tester.c
	proxy_config_tester.c
	quality_reporting_tester.c
	register_tester.c
	remote_provisioning_tester.c
	session_timers_tester.c
	setup_tester.c
	shared_core_tester.c
	stun_tester.c
	tester.c
	tunnel_tester.c
	vcard_tester.c
	video_quality_tester.c
	shared_core_tester.c
	local_conference_tester.cpp
	video_tester.c
)
if(ENABLE_LIME_X3DH)
    list(APPEND SOURCE_FILES_C group_chat_secure_tester.c)
endif()

set(SOURCE_FILES_CXX
	clonable-object-tester.cpp
	contents-tester.cpp
	cpim-tester.cpp
	multipart-tester.cpp
	property-container-tester.cpp
	utils-tester.cpp
	lime-user-authentication-tester.cpp
	vfs_encryption-tester.cpp
)

if(ENABLE_ADVANCED_IM)
	list(APPEND SOURCE_FILES_CXX conference-event-tester.cpp)
endif()

if(ENABLE_DB_STORAGE)
	list(APPEND SOURCE_FILES_CXX main-db-tester.cpp)
endif()

set(HEADER_FILES
	liblinphone_tester.h
	tools/private-access.h
	tools/tester.h
)

set(SOURCE_FILES_OBJC )
if(APPLE)
	if (IOS)
		list(APPEND SOURCE_FILES_OBJC liblinphone_tester_ios.m)
	endif()
endif()

set(GROUP_CHAT_BENCHMARK_SOURCE_C
	accountmanager.c
	tester.c
	group_chat_tester.c
	group_chat_benchmark.c
)

set(GROUP_CHAT_BENCHMARK_HEADERS
	liblinphone_tester.h
	tools/tester.h
)


# TODO: Remove me later!
list(REMOVE_ITEM STRICT_OPTIONS_CPP "-Wconversion" "-Werror=conversion" "-Wcast-align" "-Werror=cast-align")
if (ANDROID AND CMAKE_C_COMPILER_ID STREQUAL "GNU") #restrict to Android as it seems to break regular linux
	list (APPEND STRICT_OPTIONS_C "-std=c99")
endif()

bc_apply_compile_flags(SOURCE_FILES_C STRICT_OPTIONS_CPP STRICT_OPTIONS_C)
bc_apply_compile_flags(SOURCE_FILES_CXX STRICT_OPTIONS_CPP STRICT_OPTIONS_CXX)
bc_apply_compile_flags(SOURCE_FILES_OBJC STRICT_OPTIONS_CPP STRICT_OPTIONS_OBJC)

bc_apply_compile_flags(GROUP_CHAT_BENCHMARK_SOURCE_C STRICT_OPTIONS_CPP STRICT_OPTIONS_C)

add_definitions("-DLINPHONE_TESTER")

if(MSVC)
	get_source_file_property(MESSAGE_TESTER_C_COMPILE_FLAGS message_tester.c COMPILE_FLAGS)
	set(MESSAGE_TESTER_C_COMPILE_FLAGS "${MESSAGE_TESTER_C_COMPILE_FLAGS} /wd4996") # Disable "was declared deprecated" warnings
	set_source_files_properties(message_tester.c PROPERTY COMPILE_FLAGS "${MESSAGE_TESTER_C_COMPILE_FLAGS}")
	get_source_file_property(VCARD_TESTER_C_COMPILE_FLAGS vcard_tester.c COMPILE_FLAGS)
	set(VCARD_TESTER_C_COMPILE_FLAGS "${VCARD_TESTER_C_COMPILE_FLAGS} /wd4996") # Disable "was declared deprecated" warnings
	set_source_files_properties(vcard_tester.c PROPERTY COMPILE_FLAGS "${VCARD_TESTER_C_COMPILE_FLAGS}")
endif()

if(NOT IOS OR NOT CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
	# Executable must be available on root path, not host one
	find_program(SIPP_PROGRAM NAMES sipp sipp.exe ONLY_CMAKE_FIND_ROOT_PATH)
	if(SIPP_PROGRAM)
		add_definitions(-DHAVE_SIPP=1)
		add_definitions(-DSIPP_COMMAND="${SIPP_PROGRAM}")
	endif()
endif()

# on mobile platforms, we compile the tester as a library so that we can link with it directly from native applications
if(ANDROID OR IOS)
	add_library(linphonetester SHARED ${HEADER_FILES} ${SOURCE_FILES_C} ${SOURCE_FILES_CXX})
	target_include_directories(linphonetester PRIVATE ${LINPHONE_INCLUDE_DIRS} ${LIBXSD_INCLUDE_DIRS})
	target_link_libraries(linphonetester ${LINPHONE_LIBS_FOR_TOOLS} ${OTHER_LIBS_FOR_TESTER})
	#TODO: replace by if(APPLE) when we want to make apple framework on linphone-desktop too
	if(IOS)
		if(IOS)
			set(MIN_OS ${LINPHONE_IOS_DEPLOYMENT_TARGET})
		else()
			set(MIN_OS ${CMAKE_OSX_DEPLOYMENT_TARGET})
		endif()
		set_target_properties(linphonetester PROPERTIES
			FRAMEWORK TRUE
			MACOSX_FRAMEWORK_IDENTIFIER com.belledonne-communications.linphonetester
			MACOSX_FRAMEWORK_INFO_PLIST "${CMAKE_SOURCE_DIR}/build/osx/Info.plist.in"
			PUBLIC_HEADER "${HEADER_FILES}"
		)
	endif()
	install(TARGETS linphonetester
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		FRAMEWORK DESTINATION Frameworks
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)
	install(FILES "liblinphone_tester.h"
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/linphone
		PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
	)
elseif(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
	add_library(linphone_tester_static STATIC ${HEADER_FILES} ${SOURCE_FILES_C} ${SOURCE_FILES_CXX})
	target_include_directories(linphone_tester_static PRIVATE ${LINPHONE_INCLUDE_DIRS})
	target_link_libraries(linphone_tester_static ${LINPHONE_LIBS_FOR_TOOLS} ${OTHER_LIBS_FOR_TESTER})

	set(RUNTIME_COMPONENT_SOURCES
		liblinphone_tester_windows.cpp
		liblinphone_tester_windows.h
	)
	add_library(linphone_tester_runtime MODULE ${RUNTIME_COMPONENT_SOURCES})
	target_compile_options(linphone_tester_runtime PRIVATE "/wd4996")
	target_link_libraries(linphone_tester_runtime linphone_tester_static)
	set_target_properties(linphone_tester_runtime PROPERTIES VS_WINRT_COMPONENT TRUE)
	set_target_properties(linphone_tester_runtime PROPERTIES LIBRARY_OUTPUT_NAME "BelledonneCommunications.Linphone.Tester")
	set_target_properties(linphone_tester_runtime PROPERTIES LINK_FLAGS "/WINMDFILE:BelledonneCommunications.Linphone.Tester.winmd")

	install(TARGETS linphone_tester_runtime
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/BelledonneCommunications.Linphone.Tester.winmd" DESTINATION lib)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/BelledonneCommunications.Linphone.Tester.pdb
			DESTINATION ${CMAKE_INSTALL_LIBDIR}
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
	endif()
endif()

if(NOT ANDROID AND NOT CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
	if(NOT IOS)
		add_executable(liblinphone_tester ${HEADER_FILES} ${SOURCE_FILES_C} ${SOURCE_FILES_CXX} ${SOURCE_FILES_OBJC})
		set_target_properties(liblinphone_tester PROPERTIES LINK_FLAGS "${LINPHONE_LDFLAGS}")
		set_target_properties(liblinphone_tester PROPERTIES LINKER_LANGUAGE CXX)
		set_target_properties(liblinphone_tester PROPERTIES C_STANDARD 99)
		target_include_directories(liblinphone_tester PRIVATE ${LINPHONE_INCLUDE_DIRS})
		target_link_libraries(liblinphone_tester ${LINPHONE_LIBS_FOR_TOOLS} ${OTHER_LIBS_FOR_TESTER})
                if(MSVC)
                        if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
                                install(FILES $<TARGET_PDB_FILE:liblinphone_tester>
                                        DESTINATION ${CMAKE_INSTALL_BINDIR}
                                        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
                                )
                        endif()
                endif()

		install(TARGETS liblinphone_tester
			RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)

		add_executable(groupchat_benchmark ${GROUP_CHAT_BENCHMARK_HEADERS} ${GROUP_CHAT_BENCHMARK_SOURCE_C})
		set_target_properties(groupchat_benchmark PROPERTIES LINK_FLAGS "${LINPHONE_LDFLAGS}")
		set_target_properties(groupchat_benchmark PROPERTIES LINKER_LANGUAGE CXX)
		set_target_properties(groupchat_benchmark PROPERTIES C_STANDARD 99)
		target_include_directories(groupchat_benchmark PRIVATE ${LINPHONE_INCLUDE_DIRS})
		target_link_libraries(groupchat_benchmark ${LINPHONE_LIBS_FOR_TOOLS} ${OTHER_LIBS_FOR_TESTER})

		install(TARGETS groupchat_benchmark
			RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		)

	endif()
	install(FILES ${CERTIFICATE_ALT_FILES} DESTINATION "${CMAKE_INSTALL_DATADIR}/liblinphone_tester/certificates/altname")
	install(FILES ${CERTIFICATE_CLIENT_FILES} DESTINATION "${CMAKE_INSTALL_DATADIR}/liblinphone_tester/certificates/client")
	install(FILES ${CERTIFICATE_CN_FILES} DESTINATION "${CMAKE_INSTALL_DATADIR}/liblinphone_tester/certificates/cn")
	install(FILES ${DB_FILES} DESTINATION "${CMAKE_INSTALL_DATADIR}/liblinphone_tester/db")
	install(FILES ${IMAGE_FILES} DESTINATION "${CMAKE_INSTALL_DATADIR}/liblinphone_tester/images")
	install(FILES ${OTHER_FILES} DESTINATION "${CMAKE_INSTALL_DATADIR}/liblinphone_tester")
	install(FILES ${RC_FILES} DESTINATION "${CMAKE_INSTALL_DATADIR}/liblinphone_tester/rcfiles")
	install(FILES ${SIPP_FILES} DESTINATION "${CMAKE_INSTALL_DATADIR}/liblinphone_tester/sipp")
	install(FILES ${SOUND_FILES} DESTINATION "${CMAKE_INSTALL_DATADIR}/liblinphone_tester/sounds")
	install(FILES ${VCARD_FILES} DESTINATION "${CMAKE_INSTALL_DATADIR}/liblinphone_tester/vcards")
endif()
