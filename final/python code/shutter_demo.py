
from Tkinter import *
import arduinoCommsB as aC
import atexit
import time
import numpy as np

# Ensure window is in centre of screen


def center(toplevel):
    toplevel.update_idletasks()
    w = toplevel.winfo_screenwidth()
    h = toplevel.winfo_screenheight()
    size = tuple(int(_) for _ in toplevel.geometry().split('+')[0].split('x'))
    x = w/2 - size[0]/2
    y = h/2 - size[1]/2
    toplevel.geometry("%dx%d+%d+%d" % (size + (x, y)))

# =========================
# code to ensure a clean exit


def exit_handler():
    print 'Quitting...'
    cD.stopListening()
    aC.closeSerial()

atexit.register(exit_handler)

# ===========================
# global variables for this module


FocusStatus = 0
ShutterStatus = 0
DelayVal = 0
LoopVal = 0
StopStatus = 0
AdvancedStatus = 0

root = Tk()
center(root)
root.minsize(width=1000, height=600)
center(root)
root.title("BULLET TIME")

root.attributes('-topmost', True)
root.lift()
root.attributes('-topmost', False)
# the next line must come after  root = Tk() so that a StringVar()
# can be created in checkForData.
import arduinoCheckForData as cD  # so we can refer to its variables

#======================
# get first view ready


def setupView():
    global masterframe
    masterframe = Frame()
    masterframe.pack()

    selectPort()
    # mainScreen()


#======================
# definition of screen to choose the Serial Port


def selectPort():
    global masterframe, radioVar
    for child in masterframe.winfo_children():
        child.destroy()
    radioVar = StringVar()

    lst = aC.listSerialPorts()

    l1 = Label(masterframe, width=15, height=10, text="Select Port")
    l1.pack()

    if len(lst) > 0:
        for n in lst:
            r1 = Radiobutton(masterframe, text=n, variable=radioVar, value=n)
            r1.config(command=radioBtnPress)
            r1.pack(anchor=W)
    else:
        l2 = Label(masterframe, text="No Serial Port Found")
        l2.pack()

# ======================
# definition of main screen to control Arduino


def mainScreen():
    global masterframe, text, table
    for child in masterframe.winfo_children():
        child.destroy()

    labelA = Label(masterframe, width=5, height=2)                              # Focus
    labelB = Label(masterframe, width=15, height=2, text='Enter loop value:')                              # Shutter
    labelC = Label(masterframe, width=15)              # Delay Value Slider
    labelD = Label(masterframe, width=20, height=1, text='Delay value (ms)')     # Text in Enter box
    labelE = Label(masterframe, width=5, height=2)                              # Looper
    labelF = Label(masterframe, width=5, height=2)

    # Focus
    focusButton = Button(masterframe, text="Focus On", fg="white", bg="black")
    focusButton.config(command=lambda: FocusFn(focusButton))

    # Shutter
    shutterButton = Button(masterframe, text="Shutter", fg="white", bg="black")
    shutterButton.config(command=lambda:  ShutterFn(shutterButton))

    # loop On/Off
    global LoopVal, loop_entry
    LoopVal = StringVar()
    loop_entry = Entry(masterframe, text=LoopVal)
    loop_entry.pack()
    LoopVal.set("1")

    # # loop On/Off
    # global LoopVal, loop_entry
    # LoopVal = StringVar()
    # loop_entry = Entry(masterframe, text=LoopVal)
    # loop_entry.pack()
    # LoopVal.set("1")
    # LoopVal = loop_entry.get()
    # # loop_entry.bind("<Return>", lambda: LoopVal.set(int(loop_entry.get())))
    # loop_entry.grid(row=5)


    # slider
    slider = Scale(masterframe, from_=0, to=1000, length=500, tickinterval=100, orient=HORIZONTAL)
    slider.config(command=slide)

    # Enter ms value:
    global delay_entry_val, delay_entry
    delay_entry_val = StringVar(masterframe, value='1')
    delay_entry = Entry(masterframe, text=delay_entry_val)
    delay_entry.bind("<Return>", lambda event: slider.set(int(delay_entry.get())))
    delay_entry.grid(row=5)

    # Table Setup
    global table_rows, table_cols, te

    table_rows = []
    for i in range(10, 20):
        table_cols = []
        for j in range(5):
            te = Entry(masterframe, relief=RIDGE)
            te.grid(row=i, column=j, sticky=NSEW)
            te.insert(END, '%d' % (i))
            table_cols.append(te)
        table_rows.append(table_cols)

    # SendButton
    SendButton = Button(masterframe, text='Send Delays', command=getTableVals).grid()

    # Spacing
    labelA.grid(row=0)
    focusButton.grid(row=1)
    labelB.grid(row=1, column=2)
    shutterButton.grid(row=1, column=4)

    loop_entry.grid(row=1, column=3)
    labelC.grid(row=2, columnspan=8)
    slider.grid(row=4, columnspan=8)
    labelD.grid(row=4,)
    labelE.grid(row=1, column=4)
    labelF.grid(row=9, column=4)

# =========================
# various callback functions


def func(event):
    output = getTableVals()

root.bind('<Return>', func)


def getTableVals():
    global table, DelayVal, LoopVal, tframe, delay, data, model, table, table_rows, delay_entry_val, loop_entry
    LoopVal = loop_entry.get()

    DelayVal = []
    for row in table_rows:
        for col in row:
            DelayVal.append(col.get())

    DelayVal = map(int, DelayVal)

    print FocusStatus, ShutterStatus, LoopVal, StopStatus, DelayVal
    
    aC.valToArduino(FocusStatus, ShutterStatus, LoopVal, StopStatus, DelayVal)
    # return DelayVal


def FocusFn(btn):
    global FocusStatus, ShutterStatus, LoopVal, StopStatus, DelayVal
    if FocusStatus == 0:
        FocusStatus = 1
        btn.config(bg="green", fg="black", text='Focus Off')
        print 'Focus On'
    else:
        FocusStatus = 0
        btn.config(fg="white", bg="black", text='Focus On')
        print 'Focus Off'
    getTableVals()
    # getTableVals()
    # getTableVals()
    # getTableVals()
# def LoopFn(btn):
#     global FocusStatus, ShutterStatus, LoopVal, StopStatus, DelayVal
#     if LoopVal == 0:
#         LoopVal = 1
#         btn.config(bg="green", fg="black", text='Loop Off')
#         print 'Loop On'
#     else:
#         LoopVal = 0
#         ShutterStatus = 0
#         StopStatus = 1

#         btn.config(fg="white", bg="black", text='Loop On')
#         print 'Loop Off'
#     getTableVals()
#     StopStatus = 0
#     getTableVals()
#     StopStatus = 0


def ShutterFn(btn):
    global FocusStatus, ShutterStatus, LoopVal, StopStatus, DelayVal

    ShutterStatus = 1
    btn.config(bg="white", fg="black")

    getTableVals()
    # aC.valToArduino(FocusStatus, ShutterStatus, LoopVal, StopStatus, DelayVal)
    ShutterStatus = 0
    btn.config(fg="white", bg="black")
    getTableVals()


def slide(sval):
    global FocusStatus, ShutterStatus, LoopVal, StopStatus, DelayVal, table_rows
    sval = int(sval)
    delay_entry_val.set('')

    DelayVal = sval
    out = 50 * [DelayVal]

    table_rows = []
    for i in range(10, 20):
        table_cols = []
        for j in range(5):
            te = Entry(masterframe, relief=RIDGE)
            te.grid(row=i, column=j, sticky=NSEW)
            te.insert(END, '%d' % (DelayVal))
            table_cols.append(te)
        table_rows.append(table_cols)
    getTableVals()


def radioBtnPress():
    global radioVar
    aC.setupSerial(radioVar.get())
    cD.listenForData()
    mainScreen()


def quit(event=None):
    root.destroy()

quit_button = Button(text='Quit', command=quit)
quit_button.pack(side='top', pady=5, fill='x')
root.bind('<q>', quit)

# ======================
# code to start the whole process
if __name__ == '__main__':

    root.lift()
    setupView()
    root.mainloop()
