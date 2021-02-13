APP = swcode
#CC = aarch64-linux-gnu-gcc
# Add any other object files to this list below
APP_OBJS = swcode.o\
           spi_drive.o\
           sys_print.o\
           dig_para.o\
           binding_para.o\
           flash_apply.o\
           sw_pthread.o\
           zk_communicate.o\
           fc_port_ppm.o\
           time_sync.o\
           into_data.o\
           fc_port_mon.o\
           eng_para.o\
           restruct.o\
           serial_cmd.o\

LIBS = -lpthread

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS) -lpthread
clean:
	-rm -f $(APP) *.elf *.gdb *.o
