import datetime
import serial

ser = serial.Serial()
ser.port = "/dev/ttyUSB0"
ser.baudrate = 2400
ser.bytesize = serial.EIGHTBITS #number of bits per bytes
ser.parity = serial.PARITY_NONE #set parity check: no parity
ser.stopbits = serial.STOPBITS_ONE #number of stop bits
ser.timeout = 0.5            #timeout read
ser.xonxoff = False     #disable software flow control
ser.rtscts = False     #disable hardware (RTS/CTS) flow control
ser.dsrdtr = False       #disable hardware (DSR/DTR) flow control
ser.writeTimeout = 0.2     #timeout for write
ser.close() # закрываем порт

print("lsxdfvjsdk")