import serial
import time
import csv

ser = serial.Serial('/dev/cu.usbmodem141101')
serial_port = '/dev/cu.usbmodem141101'
ser.flushInput()

# while True:
#     try:
#         ser_bytes = ser.readline()
#         decoded_bytes = float(ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
#         print(decoded_bytes)
#         with open("test_data.csv","a", newline='') as f:
#             writer = csv.writer(f,delimiter=",")
#             writer.writerow([time.strftime("%H : %M : %S"),decoded_bytes])
#     except:
#         print("Keyboard Interrupt")
#         break

baud_rate = 115200; #In arduino, Serial.begin(baud_rate)
write_to_file_path = "output.txt";

output_file = open(write_to_file_path, "w+");
ser = serial.Serial(serial_port, baud_rate)
while True:
    line = ser.readline();
    line = line.decode("utf-8") #ser.readline returns a binary, convert to string
    print(line);
    data=line
    output_file.write(line);
    with open("data.csv", "wb") as csv_file:
        writer = csv.writer(csv_file, delimiter=';')
        for line in data:
            writer.writerow(line)
