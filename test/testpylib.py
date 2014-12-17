#!/usr/bin/python
#$Id: testpylib.py 583 2014-12-16 12:22:19Z gaul1 $

#import ctypes
from ctypes import *
import time

hlib='/home/gaul1/prg/prg/c/hidcom/pyhidlib.so'

def string2bytes(text):
  p=0
  bytes = []
  while (p +1) <= len(text):
    b = text[p]
    b = ord(b)
    bytes += [b]
    p+=1
  return bytes
  
def bytes2string(bytes):
  p=0
  text = ""
  for b in bytes:
    text += chr(b)
  return text  

def ctString(cs):
  s= c_char_p(cs)
  s.restype=c_char_p
  return s.value
  
def hex2(num):
  r=hex(num)[2:]
  if len(r) < 2:
    r= '0'+r
  return r
  
def dump(text):
  for c in text:
    print(hex2(ord(c))),
  print
    
def showGetFeature(m,hid):
  rx= m.getFeature(hid,64)
  #print(rx)
  r=ctString(rx)
  print("rxf[%d]:"%len(r))
  dump(r) 
  b=cast(rx,POINTER(c_char*64))[0]
  dump(b)
    
def showGetEp(m,hid):
  rx= m.readEp(hid,64)
  r=ctString(rx)
  print("rx[%d]:"%len(r))
  dump(r) 
  b=cast(rx,POINTER(c_char*64))[0]
  print("rx[%d]:"%len(b))
  dump(b) 
    
def test():
  m=CDLL(hlib)
  print("LibVer:%04x"%m.getLibVersion())

  hid=m.getNewHidDev()
  #print("hid:%d"%hid)

  r=m.getHidDev(hid, 0, 0, 0)
  #m.getHidDev(hid, 0x16c0, 0x05df, "000000001")
  print(".")
  if r!=0:
    return r
  #m.read3DescriptorStrings(hid)
  #print("4:%s"%ctString(m.getDescriptorString(hid,4)))
  #m.getDescriptorString(hid,82)
  print("p:%s"%ctString(m.getProductString(hid)))
  print("manu:%s"%ctString(m.getManufacturerString(hid)))
  print("ser#:%s"%ctString(m.getSernum(hid)))

  #
  txdata = b"\x51\x0d\x01\x01"+"\x00"*22
  
  #  txdata = b"\x00"*64

  print("tx:"),
  dump(txdata)
  #m.setFeature(hid, txdata, len(txdata));
  m.setFeature(hid, txdata, len(txdata));
  
  for i in range(1,5):
    time.sleep(0.001)
    showGetFeature(m,hid) 
  
  for i in range(1,5):
    time.sleep(0.001)  
    showGetEp(m,hid)

  txdata = b"\x64\x00"  #getVersion
  print("tx:"),
  dump(txdata)
  #m.setFeature(hid, txdata, len(txdata));
  m.setFeature(hid, txdata, len(txdata));
  
  for i in range(1,3):
    time.sleep(0.001)
    showGetFeature(m,hid) 
  
  for i in range(1,5):
    time.sleep(0.001)  
    showGetEp(m,hid)


  m.closeHid(hid)
  print("end.")

if __name__ == '__main__':
  test()
  


#m=CDLL(hlib)
#hid=m.getNewHidDev()
#m.getHidDev(hid, 0x16c0, 0x05df, 0)
