#!/usr/bin/env python3

import subprocess
import sys
import time
import os

if len(sys.argv) != 2:
     print(f"Usage: {sys.argv[0]} <binary-path-uf2>")
     exit()
     
     
binary = sys.argv[1]    

if not binary.endswith("uf2"):
   print(f"Not a uf2 file: {binary}")
   exit()  
   
if not os.path.exists(binary):
   print(f"uf2 files does not exist: {binary}")
   exit()

while(True):
     try:
          subprocess.check_output(f"mkdir -p /media/rpi-boot",
               stderr=subprocess.STDOUT,
               shell=True)


          dev = subprocess.check_output(f"fdisk -l | grep -A 8 RP2",
               stderr=subprocess.STDOUT,
               shell=True)

          dev = str(dev)
          dev = dev.split("\\n")[-2].split(" ")[0]

          print(f"Mounting device {dev}")
          subprocess.check_output(f"mount {dev} /media/rpi-boot",
               stderr=subprocess.STDOUT,
               shell=True)

          print(f"Copy {binary}...")
          subprocess.check_output(f"cp {binary} /media/rpi-boot/",
               stderr=subprocess.STDOUT,
               shell=True)
          break

     except subprocess.CalledProcessError as e:
          print("Retrying...")
          time.sleep(1)

try:
          print(f"Unmounting device {dev}")
          subprocess.check_output(f"umount /media/rpi-boot",
               stderr=subprocess.STDOUT,
               shell=True)
          print("done")
               
except subprocess.CalledProcessError as e:
  print("Unmount failed. Might not be an error...")                          
          