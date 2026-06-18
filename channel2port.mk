######################################
#
# CHANNEL2PORT
#
######################################
OCS_MIDIROUTER_VERSION = 3a7de9ad5d7ae74cf21296ff8949f02457a8af7b
OCS_MIDIROUTER_SITE = https://github.com/OilCanSteve/ocs-channel2port.git
OCS_MIDIROUTER_SITE_METHOD = git
OCS_MIDIROUTER_BUNDLES = ocs-channel2port.lv2

OCS_MIDIROUTER_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) MOD=1 OPTIMIZATIONS="" PREFIX=/usr -C $(@D)/source

define OCS_MIDIROUTER_BUILD_CMDS
	$(OCS_MIDIROUTER_TARGET_MAKE)
endef

define OCS_MIDIROUTER_INSTALL_TARGET_CMDS
	$(OCS_MIDIROUTER_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
endef

$(eval $(generic-package))
