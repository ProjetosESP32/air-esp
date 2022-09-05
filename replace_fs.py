Import("env")
import platform

print("Replace MKSPIFFSTOOL with mklittlefs")

osSystem = platform.system()

if osSystem == "Windows":
  env.Replace(MKSPIFFSTOOL = "mklittlefs.exe")
elif osSystem == "Linux":
  env.Replace(MKSPIFFSTOOL = "mklittlefs")
