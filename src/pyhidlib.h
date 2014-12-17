//$Id: pyhidlib.h 583 2014-12-16 12:22:19Z gaul1 $
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

//lib for easy access hid-devices on linux-machines with python
//2014-12-15 gaul1@lifesim.de


#include <linux/hiddev.h>
#include <ctype.h>


//--- types ---
typedef struct _SHidDev{
  struct hiddev_report_info rinfo_i;  //from device
  struct hiddev_report_info rinfo_u;  //to device
  //struct hiddev_field_info finfo;
  struct hiddev_usage_ref_multi ref_i; 
  struct hiddev_usage_ref_multi ref_u;    
  struct hiddev_devinfo device_info;    
  struct hiddev_string_descriptor HsdescSerNum; 
  struct hiddev_string_descriptor HsdescProduct; 
  struct hiddev_string_descriptor HsdescManufacturer; 
  char textBuf[HID_STRING_SIZE]; //used for special strings returned
  int packetLen;    //packetLen of usb device
  int fd;
  int readable;
  int writeable;
}SHidDev;

//--- funcs ---
int getLibVersion(void);
SHidDev* getNewHidDev(void);
int getHidDev(SHidDev* pHidDev, int vid, int pid, const char* ser, int verboseLevel);

int setFeature(SHidDev* pHidDev, const char* data, const int len);
int setControl(SHidDev* pHidDev, const char* data, const int len);
int writeEp(SHidDev* pHidDev, const char* data, const int len);
const char* getFeature(SHidDev* pHidDev);
const char* readEp(SHidDev* pHidDev);
const char* getControl(SHidDev* pHidDev);

//int getDevList(SHidDev* pHidDev); //for future versions

int closeHid(SHidDev* pHidDev);

int devReady(SHidDev* pHidDev);  //1=ok; 0=not available
const char* getDescriptorString(SHidDev* pHidDev, int index);
int read3DescriptorStrings(SHidDev* pHidDev);
int getSernumByRef(SHidDev* pHidDev, char* buf, int maxLen);
const char* getSernum(SHidDev* pHidDev);
const char* getProductString(SHidDev* pHidDev);
const char* getManufacturerString(SHidDev* pHidDev);
int getVid(SHidDev* pHidDev);
int getPid(SHidDev* pHidDev);
int getDevVersion(SHidDev* pHidDev);   //0xMMmm
void setPacketLen(SHidDev* pHidDev, int len);

//eof
