import serial
import time
import csv

serial_port = '/dev/cu.usbmodem141101'
baud_rate = 115200; #In arduino, Serial.begin(baud_rate)
write_to_file_path = "output.txt";

output_file = open(write_to_file_path, "w+");
ser = serial.Serial(serial_port, baud_rate)
ser.flushInput()

while True:
    line = ser.readline();
    line = line.decode("utf-8") #ser.readline returns a binary, convert to string
    print(line);
    data=line
    output_file.write(line);
#    with open("data.csv", "wb") as csv_file:
#        writer = csv.writer(csv_file, delimiter=';')
#        for line in data:
#            writer.writerow(line)
