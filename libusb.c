#include <stdio.h>
#include <libusb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define debug(fmt...) \
		printf("%s:%d ", __FILE__, __LINE__);\
		printf(fmt);

void get_class_name(int class_code, char *class_name)
{
	switch (class_code){
	
		case LIBUSB_CLASS_PER_INTERFACE:
			strcpy(class_name, "all type");
			break;
		case LIBUSB_CLASS_AUDIO:
			strcpy(class_name, "Audio class");
			break;
		case LIBUSB_CLASS_COMM:
			strcpy(class_name, "Communications class");
			break;
		case LIBUSB_CLASS_HID:
			strcpy(class_name, "human Interface Dvice class");
			break;
		case LIBUSB_CLASS_PHYSICAL:
			strcpy(class_name, "Physical");
			break;
		case LIBUSB_CLASS_PRINTER:
			strcpy(class_name, "Printer class");
			break;
		case LIBUSB_CLASS_IMAGE:
			strcpy(class_name, "Image class");
			break;
		case LIBUSB_CLASS_MASS_STORAGE:
			strcpy(class_name, "Mass storage class");
			break;
		case LIBUSB_CLASS_HUB:
			strcpy(class_name, "Hub class");
			break;
		case LIBUSB_CLASS_DATA:
			strcpy(class_name, "Data class");
			break;
		case LIBUSB_CLASS_SMART_CARD:
			strcpy(class_name, "Smart Card");
			break;
		case LIBUSB_CLASS_CONTENT_SECURITY:
			strcpy(class_name, "Content Security");
			break;
		case LIBUSB_CLASS_VIDEO:
			strcpy(class_name, "Video");
			break;
		case LIBUSB_CLASS_PERSONAL_HEALTHCARE:
			strcpy(class_name, "Personal Healthcare");
			break;
		case LIBUSB_CLASS_DIAGNOSTIC_DEVICE:
			strcpy(class_name, "Diagnostic Device");
			break;
		case LIBUSB_CLASS_WIRELESS:
			strcpy(class_name, "Wireless class");
			break;
		case LIBUSB_CLASS_APPLICATION:
			strcpy(class_name, "Application class");
			break;
		case LIBUSB_CLASS_VENDOR_SPEC:
			strcpy(class_name, "Class is vendor-specific");
			break;
		default:
			strcpy(class_name, "Unknown class");
			break;
	}
} 

void get_usb_name(char *usb_id, char *usb_name)
{
	char cmd[128];
	char tmp[128];
	sprintf(cmd, "./get_name_by_id.sh %s", usb_id);
	FILE *fp = popen(cmd, "r");
	if (NULL == fp){
		debug("popen error\n");
		return ;
	}
	if ( NULL != fgets(tmp, sizeof(tmp), fp))
		bzero(usb_name, 128*sizeof(char));
		strncpy(usb_name, tmp, strlen(tmp)-1);

	pclose(fp);
}

int main()
{
	int ret = -1;
	libusb_context *context;
	if ((ret = libusb_init(&context)) < 0){
		debug("libusb_init error:%d", ret);
		return -1;
	}

	struct libusb_device **list;
	struct libusb_device *device;
	int i = 0;
	char *usb_name = NULL;
	char usb_id[16];
	char usb_info[512];
	if (libusb_get_device_list(context, &list) < 0)
		return -1;
	int fd = open("/opt/EastedClient/usb_info", O_RDWR | O_CREAT | O_TRUNC);
	if (fd < 0)
		goto end;

	usb_name = (char *)malloc(128*sizeof(char));
	if (NULL == usb_name){
		close(fd);
		goto end;
	}
	
	struct libusb_device_descriptor desc;
	struct libusb_config_descriptor *config;
	const struct libusb_interface *interface;
	int type = 0;
	while ( (device = list[i++]) != NULL){
		if ( 0 == libusb_get_device_descriptor(device, &desc)){
			sprintf(usb_id, "%04lx:%04lx", desc.idVendor, desc.idProduct);
			if (0 == libusb_get_active_config_descriptor(device, &config)){
				interface = config->interface;
				struct libusb_interface_descriptor *altsetting = 
					(struct libusb_interface_descriptor *) interface->altsetting;
				type = altsetting->bInterfaceClass;
			}
			get_usb_name(usb_id, usb_name);
			sprintf(usb_info, "type=%d %s name=%s\n", type, usb_id, usb_name);
			write(fd, usb_info, strlen(usb_info));
		}
	}
	free(usb_name);
	close(fd);
end:
	libusb_free_device_list(list, 0);

	return 0;
}
