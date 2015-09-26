//$Id: pyhidlib.c 583 2014-12-16 12:22:19Z gaul1 $
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

#include "pyhidlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/hiddev.h>
#include <linux/input.h>
#include <time.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>
#include <malloc.h>


#ifndef TRUE
#define TRUE (~0)
#endif

//consts
//const svn_rev_string = "$Rev: 583 $";
//const svnRev = 
const int c_defPacketLen = 64;

//interns
void compr_s32toChar(__s32 * values, int len);
//int devReady(SHidDev* pHidDev);
void fillTxData(SHidDev* pHidDev, const char* data, const int len);
//void copy2Text(SHidDev* pHidDev, __s32* values, int len);

//----
SHidDev* getNewHidDev(void)
{
  SHidDev* h;
  h = malloc(sizeof(SHidDev));
  if(h==NULL) return 0;
  //init values
  memset(h, 0, sizeof(SHidDev));
  h->packetLen = c_defPacketLen;
  return h;
}

int initHid(SHidDev* pHidDev, int len){
  pHidDev->rinfo_u.report_type= HID_REPORT_TYPE_OUTPUT;
  pHidDev->rinfo_i.report_type= HID_REPORT_TYPE_INPUT;
  pHidDev->rinfo_u.report_id= pHidDev->rinfo_i.report_id = HID_REPORT_ID_FIRST;
  pHidDev->rinfo_u.num_fields= pHidDev->rinfo_i.num_fields= 1;
  pHidDev->ref_u.uref.report_type= HID_REPORT_TYPE_OUTPUT;
  pHidDev->ref_i.uref.report_type= HID_REPORT_TYPE_INPUT;
  pHidDev->ref_u.uref.report_id= pHidDev->ref_i.uref.report_id= HID_REPORT_ID_FIRST;
  pHidDev->ref_u.uref.field_index= pHidDev->ref_i.uref.field_index =0;
  pHidDev->ref_u.uref.usage_index= pHidDev->ref_i.uref.usage_index =0;
  pHidDev->ref_u.num_values = len;
  pHidDev->ref_i.num_values = len; 
  return 0;
}

int getVid(SHidDev* pHidDev){
  return pHidDev->device_info.vendor;
}

int getPid(SHidDev* pHidDev){
  return pHidDev->device_info.product;
}
int getDevVersion(SHidDev* pHidDev){
  return pHidDev->device_info.version;
}

int getSernumByRef(SHidDev* pHidDev, char* buf, int maxLen)
{
  int i;
  int n=0;
  int ns=3;
//  int ret = 0;
  struct hiddev_string_descriptor Hstring1;
  Hstring1.index =1;
  Hstring1.value[0]=0;

  Hstring1.index = 82;
  ioctl(pHidDev->fd, HIDIOCGSTRING, &Hstring1);
  if(Hstring1.value[0]){
    printf("Serial number: %s\r\n", Hstring1.value);    
  }

  for( i=0; i<0xff;i++){
    Hstring1.index = i;
    Hstring1.value[0]=0;
    ioctl(pHidDev->fd, HIDIOCGSTRING, &Hstring1);
    if(Hstring1.value[0]){
      printf("more[%d]: %s\r\n", i, Hstring1.value);
      n++;
      if(n==ns) {
        Hstring1.value[maxLen] =0;
        strcpy(buf, (Hstring1.value));
        return strlen(buf);
      }
    }
  }
  return 0;
}

const char* getSernum(SHidDev* pHidDev)
{
  if(pHidDev == NULL)return 0;
  return pHidDev->HsdescSerNum.value;
}


int read3DescriptorStrings(SHidDev* pHidDev)
{
  int i;
  int n=0;

  struct hiddev_string_descriptor sStrDescr;

  for( i=0; i<0xff;i++){
    sStrDescr.index = i;
    sStrDescr.value[0]=0;
    
    ioctl(pHidDev->fd, HIDIOCGSTRING, &sStrDescr);
    if(sStrDescr.value[0]){
      n++;
      
      if(n==1) {
        memcpy((void*)&(pHidDev->HsdescManufacturer), (void*)&sStrDescr, sizeof(struct hiddev_string_descriptor) );
      }
      if(n==2) {
        memcpy((void*)&pHidDev->HsdescProduct, (void*)&sStrDescr, sizeof(struct hiddev_string_descriptor) );
      }
      if(n==3) {
        memcpy((void*)&(pHidDev->HsdescSerNum.index), 
            (void*)&sStrDescr.index, sizeof(struct hiddev_string_descriptor) );
            
        //printf("s#[%s]\r\n", pHidDev->HsdescSerNum.value);
        return 0;
      }
    }
  }
  return 1;
}

const char* getProductString(SHidDev* pHidDev){
  if(pHidDev == NULL)return 0;
  return pHidDev->HsdescProduct.value;
}
const char* getManufacturerString(SHidDev* pHidDev){
  if(pHidDev == NULL)return 0;
  return pHidDev->HsdescManufacturer.value;
}


const char* getDescriptorString(SHidDev* pHidDev, int index)
{
  struct hiddev_string_descriptor sStrDescr;

  pHidDev->textBuf[0] = 0;
  if( NULL == pHidDev ) return 0;
  if( 0 == pHidDev->fd ) return 0;
  
  sStrDescr.index = index;
  sStrDescr.value[0]=0;
  
  
  ioctl(pHidDev->fd, HIDIOCGSTRING, &sStrDescr);
  sStrDescr.value[HID_STRING_SIZE] =0;
  strcpy(pHidDev->textBuf, sStrDescr.value);  
        
  return pHidDev->textBuf;
}


int getHidDev(SHidDev* pHidDev, int vid, int pid, const char* ser, int verbose)
{
  char path[256];    
  int i;
  const int maxIndex = 20;
  
  pHidDev->writeable = 0;
  pHidDev->readable = 0;
  
  for(i=0; i< maxIndex; i++){
    sprintf(path,"/dev/usb/hiddev%d", i);
    if ((pHidDev->fd = open(path, O_RDONLY )) >= 0){
      ioctl(pHidDev->fd, HIDIOCGDEVINFO, &pHidDev->device_info);
      if(verbose){
        printf("hid.vid=%04x;pid=%04x;ver=%04x;\r\n",
          pHidDev->device_info.vendor,
          pHidDev->device_info.product,
          pHidDev->device_info.version);
      }
      //check vid, and pid if given
      if((0==vid) ||
          (
            pHidDev->device_info.vendor==vid 
            && ( pHidDev->device_info.product==pid || 0==pid)
          )
        ) {
        read3DescriptorStrings(pHidDev);
        if(verbose >1){
          printf("vendor[%s], product[%s], ser#[%s]\r\n",
          pHidDev->HsdescManufacturer.value,
          pHidDev->HsdescProduct.value,
          pHidDev->HsdescSerNum.value);
        }
        if(ser == 0){ //any ser# allowed
          i= maxIndex +1;
          pHidDev->readable = TRUE;
          pHidDev->writeable = TRUE;
          return 0;
        }else{  //test for ser# fit
          if(0==strcmp(ser, pHidDev->HsdescSerNum.value)){
            i= maxIndex+1;
            pHidDev->readable = TRUE;
            pHidDev->writeable = TRUE;              
            return 0;
          }
        }
      }
    }
  }
  if(i==maxIndex){
    perror("Can't find device\r\n");
    return (1);
  }
   
  return 0;
}

void setPacketLen(SHidDev* pHidDev, int len)
{
  pHidDev->packetLen = len;
}

int closeHid(SHidDev* pHidDev)
{
  if(pHidDev){
    pHidDev->readable = 0;
    close(pHidDev->fd);
    free(pHidDev);
    pHidDev = 0;
  }
  return 0;
}

int devReady(SHidDev* pHidDev){
  if(NULL == pHidDev) return 0;
  if(0 == pHidDev->fd) return 0;
  if(!pHidDev->readable) return 0;

  return 1;
}

void fillTxData(SHidDev* pHidDev, const char* data, const int len)
{
  int i;
  
  //set report lens
  //pHidDev->ref_u.num_values = len;
  //pHidDev->ref_i.num_values = len; 
  
  //copy data 
  for(i=0; i<len; i++){
    pHidDev->ref_u.values[i] = data[i];
  }
  //fill rest with 0
  while(i < pHidDev->packetLen){
    pHidDev->ref_u.values[i] = 0;
    i++;
  }
}

void compr_s32toChar(__s32* values, int len){
  int i;
  char* pc = (char*)values;
  
  for(i=0; i<len; i++){
    pc[i] = (char)values[i];
  }
}

//void copy2Text(SHidDev* pHidDev, __s32* values, int len){
  //int i;

  //for(i=0; i<len; i++){
    //pHidDev->textBuf[i] = (char)values[i];
  //}
//}

int getLibVersion(void)
{
  return 0x0092;
}

int setFeature(SHidDev* pHidDev, const char* data, const int len)
{
  if(!devReady(pHidDev)) return 0;
  initHid(pHidDev, pHidDev->packetLen);
  fillTxData(pHidDev, data, len);
  
  pHidDev->rinfo_u.report_type = HID_REPORT_TYPE_FEATURE;
  pHidDev->ref_u.uref.report_type = HID_REPORT_TYPE_FEATURE;
  //doit
  ioctl(pHidDev->fd,HIDIOCSUSAGES, &pHidDev->ref_u);
  ioctl(pHidDev->fd,HIDIOCSREPORT, &pHidDev->rinfo_u);
  return pHidDev->packetLen;
}

const char* getFeature(SHidDev* pHidDev)
{
  if(!devReady(pHidDev)) return 0;
  initHid(pHidDev, pHidDev->packetLen);
  pHidDev->rinfo_i.report_type =HID_REPORT_TYPE_FEATURE;
  pHidDev->ref_i.uref.report_type =HID_REPORT_TYPE_FEATURE;  
  //doit  
  ioctl(pHidDev->fd,HIDIOCGREPORT, &pHidDev->rinfo_i);
  ioctl(pHidDev->fd,HIDIOCGUSAGES, &pHidDev->ref_i);
  
  //copy2Text(pHidDev, pHidDev->ref_i.values, len);
  compr_s32toChar(pHidDev->ref_i.values, pHidDev->packetLen);
  return (char*)(pHidDev->ref_i.values);
}

int setControl(SHidDev* pHidDev, const char* data, const int len)
{
  if(!devReady(pHidDev)) return 0;
  initHid(pHidDev,pHidDev->packetLen);
  fillTxData(pHidDev, data, len);
  //doit 
  ioctl(pHidDev->fd,HIDIOCSUSAGES, &pHidDev->ref_u);
  ioctl(pHidDev->fd,HIDIOCSREPORT, &pHidDev->rinfo_u);
  return 0;
}

const char* getControl(SHidDev* pHidDev)
{
  if(!devReady(pHidDev)) return 0;
  initHid(pHidDev,pHidDev->packetLen);
  //doit
  ioctl(pHidDev->fd,HIDIOCGREPORT, &pHidDev->rinfo_i);
  ioctl(pHidDev->fd,HIDIOCGUSAGES, &pHidDev->ref_i);

  compr_s32toChar(pHidDev->ref_i.values, pHidDev->ref_i.num_values);
  return (char*)(pHidDev->ref_i.values);
}

int writeEp(SHidDev* pHidDev, const char* data, const int len)
{
  if(!devReady(pHidDev)) return 0;
  initHid(pHidDev, pHidDev->packetLen);
  fillTxData(pHidDev, data, len);
  //doit
  ioctl(pHidDev->fd,HIDIOCSUSAGES, &pHidDev->ref_u); 
  ioctl(pHidDev->fd,HIDIOCSREPORT, &pHidDev->rinfo_u);
  return 0;
}

const char* readEp(SHidDev* pHidDev)
//~ const __u8* readEp(SHidDev* pHidDev, const int len)
{
  if(!devReady(pHidDev)) return 0;
  initHid(pHidDev , pHidDev->packetLen);
   //doit
  ioctl(pHidDev->fd,HIDIOCGUSAGES, &pHidDev->ref_i); 
  ioctl(pHidDev->fd,HIDIOCGREPORT, &pHidDev->rinfo_i);

  compr_s32toChar(pHidDev->ref_i.values, pHidDev->ref_i.num_values);
  //~ return (__u8*)(pHidDev->ref_i.values);
  return (char*)(pHidDev->ref_i.values);
}

//eof
