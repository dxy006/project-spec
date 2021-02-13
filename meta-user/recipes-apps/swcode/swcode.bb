#
# This file is the swcode recipe.
#

SUMMARY = "Simple swcode application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI =" file://common.h\
       file://sys_print.c\
       file://sys_print.h\
       file://spi_drive.c\
       file://spi_drive.h\
       file://dig_para.c\
       file://dig_para.h\
       file://swcode.c \
       file://binding_para.c\
       file://binding_para.h\
       file://flash_apply.c\
       file://flash_apply.h\
       file://zk_communicate.c\
       file://zk_communicate.h\
       file://fc_port_ppm.c\
       file://fc_port_ppm.h\
       file://fc_port_mon.c\
       file://fc_port_mon.h\
       file://sw_pthread.c\
       file://sw_pthread.h\
       file://time_sync.c\
       file://time_sync.h\
       file://into_data.c\
       file://into_data.h\
       file://restruct.c\
       file://restruct.h\
       file://eng_para.c\
       file://eng_para.h\
       file://serial_cmd.c\
       file://serial_cmd.h\
       file://Makefile \
		  "

S = "${WORKDIR}"

do_compile() {
	     oe_runmake
}

do_install() {
	     install -d ${D}${bindir}
	     install -m 0755 swcode ${D}${bindir}
}
