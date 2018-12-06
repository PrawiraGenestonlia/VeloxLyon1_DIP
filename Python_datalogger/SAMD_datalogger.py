import serial
import time
import csv

serial_port = '/dev/cu.usbmodem14401'
baud_rate = 115200; #In arduino, Serial.begin(baud_rate)
write_to_file_path = "output.txt";

output_file = open(write_to_file_path, "w+");
ser = serial.Serial(serial_port, baud_rate)
ser.flushInput()

while True:
    line = ser.readline();
    line = line.decode("utf-8") #ser.readline returns a binary, convert to string
#    print(line);
    data=line
    output_file.write(line);
    if (data[0]=="$"):
        a = data.split(";");
        if((a[0]=="$")&(a[9]=="#\n") ):
            print("Current date & time : " + time.strftime("%c"));
            print("Battery level : "+a[1]);
            print("Temperature : "+a[2]);
            print("Spetral Sensor (V) : "+a[3]);
            print("Spetral Sensor (IR) : "+a[4]);
            print("Magnetometer, x: "+a[5]+" ,y: "+a[6]+" ,z: "+a[7]+" ,t: "+a[8]);
            print("\n\n\n");
        else:
            print("Data is corrupted");
    if (data[0]=="@"):
        print("Current date & time : " + time.strftime("%c"));
        print("The satellite is healthy");


