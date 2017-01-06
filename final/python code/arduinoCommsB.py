import serial
import time
import sys
import glob
import numpy as np

# global variables for module
startMarker = 60
endMarker = 62

#========================

def valToArduino(Focus, Shutter, Loop, Reverse, Delay):
    
    # Delay = list(500 * np.ones(50, dtype=int))
    print Loop

    # print len(Delay)

    # sendStr = "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" %(Focus, Shutter, Loop, Reverse, Delay[00], Delay[01], Delay[02], Delay[03], Delay[04], Delay[05], Delay[06], Delay[07], Delay[08], Delay[09], Delay[10], Delay[11], Delay[12], Delay[13], Delay[14], Delay[15], Delay[16], Delay[17], Delay[18], Delay[19])

    sendStr = "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" % (Focus, Shutter, Loop, Reverse, 
    Delay[0], Delay[1], Delay[2], Delay[3], Delay[4], Delay[5], Delay[6], Delay[7], Delay[8], Delay[9], 
    Delay[10], Delay[11], Delay[12], Delay[13], Delay[14], Delay[15], Delay[16], Delay[17], Delay[18], Delay[19], 
    Delay[20], Delay[21], Delay[22], Delay[23], Delay[24], Delay[25], Delay[26], Delay[27], Delay[28], Delay[29], 
    Delay[30], Delay[31], Delay[32], Delay[33], Delay[34], Delay[35], Delay[36], Delay[37], Delay[38], Delay[39], 
    Delay[40], Delay[41], Delay[42], Delay[43], Delay[44], Delay[45], Delay[46], Delay[47], Delay[48], Delay[49])

    # print "SENDSTR %s" %( sendStr)
    # print list(sendStr)
    print 'Sending delays'
    for i, j in zip(Delay, range(len(Delay))):
        print 'Software Delay val_', j, i

    print 'sendStr', sendStr
    sendToArduino(sendStr)

    # print ser.readline()
#========================

def listSerialPorts():
    # http://stackoverflow.com/questions/12090503/listing-available-com-ports-with-python

    """Lists serial ports

    :raises EnvironmentError:
        On unsupported or unknown platforms
    :returns:
        A list of available serial ports
    """
    if sys.platform.startswith('win'):
        ports = ['COM' + str(i + 1) for i in range(256)]

    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this is to exclude your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')

    # elif sys.platform.startswith('darwin'):
    #     ports = glob.glob('/dev/tty.w*')

    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.usb*')        
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

#========================

def setupSerial(serPort):

    global  ser

    baudRate = 19200 # 115200 # 
    ser = serial.Serial(serPort, baudRate)
    print "Serial port " + serPort + " opened  Baudrate " + str(baudRate)

    waitForArduino()

#========================

def closeSerial():

    global ser
    if 'ser' in globals():
        ser.close()
        print "Serial Port Closed"
    else:
        print "Serial Port Not Opened"

#========================

def sendToArduino(sendStr):

    global startMarker, endMarker, ser

    print 'sendStr', sendStr
    # print 'ARDUINO:',ser.readline() # Read the newest output from the Arduino

    ser.write(chr(startMarker))
    ser.write(sendStr)
    ser.write(chr(endMarker))


#===========================

def recvFromArduino(timeOut): # timeout in seconds eg 1.5

    global startMarker, endMarker, ser

    #~ print "Called with T-O %s" %(timeOut)

    dataBuf = ""
    x = "z" # any value that is not an end- or startMarker
    startTime = time.time()

    # wait for the start marker
    while  ord(x) != startMarker:
        if time.time() - startTime >= timeOut:
            return('<<')
        if ser.inWaiting() > 0: # because ser.read() blocks
            x = ser.read()


    # save data until the end marker is found
    while ord(x) != endMarker:
        if time.time() - startTime >= timeOut:
            return('>>')
        if ord(x) != startMarker:
            dataBuf = dataBuf + x
        if ser.inWaiting() > 0:
            x = ser.read()
        else:
            x = chr(startMarker) # crude way to prevent repeat characters
                                 #   when no data is received


    # print out ARDUINO content in this format: Serial.print("<DelayVal>");
    print dataBuf
    return(dataBuf)

#============================


def waitForArduino():

   # wait until the Arduino sends 'Arduino Ready' - allows time for Arduino reset
   # it also ensures that any bytes left over from a previous message are discarded

    print "Waiting for Arduino to reset"

    msg = ""
    while msg.find("Arduino is ready") == -1:

        msg = recvFromArduino(10)

        print msg
        print
