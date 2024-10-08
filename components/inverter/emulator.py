import datetime
import serial
import serial.tools.list_ports

CRC16_XMODEM_TABLE = [
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
        ]

def crc16xmodem(data):  # функция расчета CRC
    crc = 0
    table = CRC16_XMODEM_TABLE
    for byte in data:
        crc = ((crc<<8)&0xff00) ^ table[((crc>>8)&0xff)^ord(byte)]
    return crc & 0xffff

def ansver(cmd):  # функция опроса инвертора и предварительной обработки ответа
   c = cmd
   ser.flushInput() # ждем если порт вдруг занят 
   # отправляем в расчет CRC
   crc = crc16xmodem(cmd).to_bytes(2, 'big') 
   #  получаем два байта CRC
   crc0 = crc[0].to_bytes(1, 'big')
   crc1 = crc[1].to_bytes(1, 'big')

   if crc[0]==0x28 or crc[0]==0x0d or crc[0]==0x0a:
      crc0 = (crc[0]+1).to_bytes(1, 'big')

   if crc[1]==0x28 or crc[1]==0x0d or crc[1]==0x0a:
      crc1 = (crc[1]+1).to_bytes(1, 'big')

   # добавляем к запросу CRC и "возврат каретки"
   cmd = cmd.encode('utf-8')
   cmd = cmd+crc0+crc1
   cmd = cmd+b'\r'
   
   ser.write(cmd)  # отправляем запрос в порт
   return c


ser = serial.Serial()
usbcom = "USB VID:PID=0403:6001 SER=A50285BIA"
esp32s3 = "USB VID:PID=303A:1001 SER=80:65:99:C7:A6:88 LOCATION=1-2.3:x.0"

ports = serial.tools.list_ports.comports()

for port, desc, hwid in sorted(ports):
    if usbcom == hwid :
        ser.port = port
        
ser.baudrate = 2400
ser.bytesize = serial.EIGHTBITS #number of bits per bytes
ser.parity = serial.PARITY_NONE #set parity check: no parity
ser.stopbits = serial.STOPBITS_ONE #number of stop bits
ser.timeout = 0.5            #timeout read
ser.xonxoff = False     #disable software flow control
ser.rtscts = False     #disable hardware (RTS/CTS) flow control
ser.dsrdtr = False       #disable hardware (DSR/DTR) flow control
ser.writeTimeout = 0.2     #timeout for write
ser.open() # закрываем порт

ser.flushInput()

#ser.flushOutput() # ждем окончания отправки
#   response = ser.readline()  # забираем ответ


   ##отрезаем три байта в конце и "("  в начале ответа
#   response = response[:-3]
#   resp = response.decode('utf-8')
#   resp = resp.replace("(","")

commnds = {
            "QPI":"(PI30",
            "QID":"(96322210101341",
            "QSID":"(1496322210101341005535",
            "QVFW":"(VERFW:00056.09",
            "QVFW3":"(VERFW:00025.06",
            "VERFW":"(NAK",
            "QPIRI":"(230.0 17.3 230.0 50.0 17.3 4000 4000 24.0 25.5 21.1 28.4 28.4 8 050 040 1 1 2 1 01 0 0 27.0 0 1",
            "QFLAG":"(ExzDabdjkuvy",
                    # Response examples
                    # 226.7 49.9 226.7 49.9 0498 0479 016 427 27.00 005 100 0035 01.9 255.1 00.00 00000 10010110 00 00 00510 110 (2424MSE1)
                    # 225.8 49.9 225.8 49.9 0609 0565 020 427 27.00 005 100 0035 02.2 259.9 00.00 00000 10010110 00 00 00590 110 (2424MSE1)
                    # 247.3 50.0 239.0 50.0 0931 0805 025 360 26.10 007 060 0017 04.6 179.2 00.00 00001 00010110 00 00 00831 011 (Axpert VM IV 24v 3600w)
                    # 232.6 50.0 229.9 49.9 0391 0312 007 402 54.40 042 072 0066 0042 284.6 00.00 00000 00010010 00 00 02901 010 (PIP-5048Mg FW71.85)
                    # 218.1 49.9 218.1 49.9 0327 0295 005 360 51.20 000 100 0037 00.0 000.0 00.00 00000 00010000 00 00 00002 011 0 00 0000 (PIP6048MT)
                    # 230.6 50.0 230.6 50.0 0484 0435 012 421 28.40 000 099 0040 00.0 060.3 00.00 00000 00010110 00 00 00000 111 0 00 0000 (VMIII-4000)
            "QPIGS":"(230.6 50.0 230.6 50.0 0484 0435 012 421 28.40 000 099 0040 00.0 060.3 00.00 00000 00010110 00 00 00000 111 0 00 0000",
            #         BBB.B CC.C DDD.D EE.E FFFF GGGG HHH III JJ.JJ KKK OOO TTTT EE.E UUU.U WW.WW PPPPP bbbbbbbb QQ VV MMMMM bbb Y ZZ AAAA
            "QPIGS2":"(NAK",
            "QPGS1":"(NAK",
            "QMOD":"(L",
            "QPIWS":"(000000000000000000000000000000000000",
            "QDI":"(230.0 50.0 0030 21.0 27.0 28.2 23.0 60 0 0 2 0 0 0 0 0 1 1 1 0 1 0 27.0 0 1",
            "QMCHGCR":"(010 020 030 040 050 060 070 080 090 100 110 120",
            "QMUCHGCR":"(002 010 020 030 040 050 060 070 080 090 100",
            "QOPPT":"(1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0",
            "QCHPT":"(2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 0 0",
            "QT":"(20240815190314",
            "QBEQI":"(0 060 030 040 030 29.20 000 120 0 0000",
            "QMN":"(VMIII-4000",
            "QGMN":"(055",
            "QET":"(00067300",
            "QEY2024":"(00067300",
            "QEM202408":"(00037300",
            "QED20240815":"(00002493",
            "QLT":"(00451700",
            "QLY2024":"(00235500",
            "QLM202408":"(00066400",
            "QLD20240815":"(00003473",
            "QBMS":"(0 099 0 0 0 284 284 211 0040 0090",
            "PBMS":"(NAK",
            "QLED":"(NAK",
            "QWFS":"(1",
        }

        
while True:
    response = ser.read_until(b'\x0D')
    response = response[:-3]
    resp = response.decode('utf-8')
    if resp : print("RX <-- ",resp)
    if resp in commnds : 
        ansver(commnds[resp])
        print(commnds[resp]) 
    else :
        ansver("NAK")
        print("NAK")
    
    
