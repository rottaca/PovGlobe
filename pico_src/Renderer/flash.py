#!/usr/bin/env python3

import subprocess
import sys
import time
import os

if len(sys.argv) != 2:
     print(f"Usage: {sys.argv[0]} <binary-path-uf2>")
     exit()
     
     
binary = sys.argv[1]    
mountpoint = "/media/rpi-boot"
pico_flash_trigger_baud=1200

if not binary.endswith("uf2"):
   print(f"Not a uf2 file: {binary}")
   exit()  
   
if not os.path.exists(binary):
   print(f"uf2 files does not exist: {binary}")
   exit()

def run_cmd(cmd, raise_error=False, input=None):
  print(f"--> {cmd}")
  res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True, text=True, input=input)
  if raise_error and res.returncode != 0:
    raise subprocess.CalledProcessError()
  else:
    return res.stdout
  

while(True):
     try:
          tty = run_cmd(f"dmesg | grep 'USB ACM device'")
          if len(tty) > 0:
            # Get tty device name from dmesg output
            tty = tty.strip().split("\n")[-1].split(":")[2].lstrip()
          else:
            print("Pico not connected as serial device.")
            print("Not able to force it into flash mode automatically.")  
          
          run_cmd(f"minicom -b {pico_flash_trigger_baud} -o -D /dev/{tty}", input="^Ax^M")
          
          dev = ""
          while(len(dev)==0):
            time.sleep(1)
            print("Waiting for device...")
            dev = run_cmd(f"fdisk -l | grep -A 8 RP2")
          
          dev = dev.split("\n")[-2].split(" ")[0]

          print("Rpi Pico found.")  
          
          if not os.path.exists(mountpoint):
            run_cmd(f"mkdir -p {mountpoint}")
            
          print(f"Mounting device {dev}")
          run_cmd(f"mount {dev} {mountpoint}", raise_error=True)

          print(f"Copy {binary}...")
          run_cmd(f"cp {binary} {mountpoint}/", raise_error=True)
          break

     except subprocess.CalledProcessError as e:
          print("Retrying...")
          time.sleep(1)

try:
          print(f"Unmounting device {dev}")
          run_cmd(f"umount {mountpoint}", raise_error=True)
          print("done")
               
except subprocess.CalledProcessError as e:
  print("Unmount failed. Might not be an error...")                          
          