
from Tkinter import *
import arduinoCommsB as aC
import atexit
import time
import numpy as np

# Ensure window is in centre of screen

delays = [0, 10000, 5000, 4000, 3000, 2000, 1500, 1000, 950, 900, 850, 800, 750, 700, 650, 600, 550, 500, 450, 400, 350, 300, 250, 200, 150, 100, 90, 75, 50, 48, 30, 25, 24, 23, 22, 16, 12, 10, 5, 2, 1]

def ShowChoice():
    global DelayVal, table_cols, table_rows
    DelayVal = v.get()
    print DelayVal
    delay_entry_val.set('')

    out = 50 * [DelayVal]

    table_rows = []
    for i in range(30+10, 30+20):
        table_cols = []
        for j in range(5):
            te = Entry(masterframe, relief=RIDGE)
            te.grid(row=i, column=j, sticky=NSEW)
            te.insert(END, '%d' % delays[int(DelayVal)])
            table_cols.append(te)
        table_rows.append(table_cols)
    getTableVals()


def center(toplevel):
    toplevel.update_idletasks()
    w = toplevel.winfo_screenwidth()
    h = toplevel.winfo_screenheight()
    size = tuple(int(_) for _ in toplevel.geometry().split('+')[0].split('x'))
    x = w/2 - size[0]/2
    y = h/2 - size[1]/2
    toplevel.geometry("%dx%d+%d+%d" % (size + (x, y)))


# ===========================
# global variables for this module


FocusStatus = 0
ReverseStatus = 0
ShutterStatus = 0
DelayVal = 0.
DelayVal_float = 0.

LoopVal = 0
AdvancedStatus = 0

root = Tk()
center(root)
root.minsize(width=1175, height=800)
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
    
    labelR = Label(masterframe, width=5, height=2)                              # Reverse
    labelBBV1 = Label(masterframe, width=5, height=2)                           # Bounce Back V1
    labelBBV2 = Label(masterframe, width=5, height=2)                           # Bounce Back V1

    # BB V1 Apex
    # labelBB_V1_entry = Entry(masterframe, text='Enter apex delay for cam 1')    # Bounce Back apex values
    # labelBB_V1_entry.grid(row=2, column=4)
    
    # labelBB_V2_entrytext = Label(masterframe, width=20, height=2, text='Camera 1 Apex delay') 

    labelC = Label(masterframe, width=15)              # Delay Value Slider
    labelC_float = Label(masterframe, width=15)        # Delay float Value Slider
    labelD = Label(masterframe, width=20, height=1, text='Delay value (FPS)')    # Text in Enter box
    labelLoop = Label(masterframe, width=5, height=2)                              # Looper
    # labelF = Label(masterframe, width=100, height=2, text='Camera 1\
    #                         Camera 2\
    #                         Camera 3\
    #                         Camera 4\
    #                         Camera 5')                              


    # Focus
    focusButton = Button(masterframe, text="Focus On", fg="white", bg="black")
    focusButton.config(command=lambda: FocusFn(focusButton))

    # Reverse
    reverseButton = Button(masterframe, text="Reverse On", fg="white", bg="black")
    reverseButton.config(command=lambda: ReverseFn(reverseButton))

    # Bounceback V1
    bouncebackButtonV1 = Button(masterframe, text="Bounceback V1 On", fg="white", bg="black")
    bouncebackButtonV1.config(command=lambda: BounceBackFnV1(bouncebackButtonV1))

    # Bounceback V2
    bouncebackButtonV2 = Button(masterframe, text="Bounceback V2 On", fg="white", bg="black")
    bouncebackButtonV2.config(command=lambda: BounceBackFnV2(bouncebackButtonV2))

    # Shutter
    shutterButton = Button(masterframe, text="Shutter", fg="white", bg="black")
    shutterButton.config(command=lambda:  ShutterFn(shutterButton))

    # loop On/Off
    global LoopVal, loop_entry
    LoopVal = StringVar()
    loop_entry = Entry(masterframe, text=LoopVal)
    loop_entry.pack()
    LoopVal.set("1")


    # slider
    # slider = Scale(masterframe, from_=10000, to=0, length=450, tickinterval=1000, resolution=1, orient=HORIZONTAL)
    # slider.config(command=slide)

    # slider_float = Scale(masterframe, from_=0.0, to=0.9, length=100, tickinterval=50, resolution=0.25, orient=HORIZONTAL)
    # slider_float.config(command=slide_float)

    rowr = 0
    for val, fps in enumerate(delays):

        btn_select = Radiobutton(masterframe, 
                      text=fps,
                      padx = 0, 
                      variable=v, 
                      command=ShowChoice,
                      value=val)

        # if val >= len(delays)/2:
        #     print val
        #     rowr = 1
        #     colr = val - len(delays)/2
        # else: 
        #     rowr = 0
        #     colr = val
        #     btn_select.pack()

        if val % 5 == 0:
            print val
            rowr += 1
            colr = 0
            # colr = val - len(delays)/2
        else: 
            
            colr +=1
            btn_select.pack()

        btn_select.grid(row=rowr, column=colr, sticky=NSEW)

    # Enter ms value:
    global delay_entry_val, delay_entry
    delay_entry_val = StringVar(masterframe, value='1')
    print 'delay_entry_val', delay_entry_val
    # delay_entry = Entry(masterframe, text=delay_entry_val)
    # delay_entry.bind("<Return>", lambda event: slider.set(float(delay_entry.get())))
    # delay_entry.bind("<Return>", lambda event: btn_select.set(float(delay_entry.get())))
    # delay_entry.grid(row=25)

    # Table Setup
    global table_rows, table_cols, te, DelayVal

    # table_rows = []
    # for i in range(10, 20):
    #     table_cols = []
    #     for j in range(5):
    #         te = Entry(masterframe, relief=RIDGE)
    #         te.grid(row=i, column=j, sticky=NSEW)
    #         # te.insert(END, '%d' % (0))
    #         te.insert(END, '%d' % delays[int(DelayVal)])
    #         table_cols.append(te)
    #     table_rows.append(table_cols)

    # SendButton
    SendButton = Button(masterframe, text='Send Delays', command=getTableVals).grid()

    # Spacing
    # Focus
    labelA.grid(row=30+1)
    focusButton.grid(row=30+3)

    # Reverse
    labelR.grid(row=30+1)
    reverseButton.grid(row=30+3, column=1)

    # Bounceback v1
    labelBBV1.grid(row=30+2)
    labelBBV2.grid(row=30+2)
    bouncebackButtonV1.grid(row=30+4, column=1)
    bouncebackButtonV2.grid(row=30+4, column=2)
    # labelBB_V2_entrytext.grid(row=30+2, column=3)

    # Shutter
    labelB.grid(row=30+3, column=2)
    shutterButton.grid(row=30+3, column=4)

    loop_entry.grid(row=30+3, column=3)
    labelC.grid(row=30+4, columnspan=8)
    labelC_float.grid(row=30+5, columnspan=8)
    # slider.grid(row=30+4, columnspan=8)
    # slider_float.grid(row=30+3, columnspan=8)
    labelD.grid(row=30+6,)
    labelLoop.grid(row=30+3, column=4)
    # labelF.grid(row=11, columnspan=8)

# =========================
# various callback functions


def find_nearest(array,value):
    value = int(value)
    while value not in array:
        if value >= max(delays):
            value -=1
        else:
            value+=1
    return value


def func(event):
    output = getTableVals()

root.bind('<Return>', func)


def getTableVals():
    global table, DelayVal, LoopVal, tframe, delay, data, model, table, table_rows, delay_entry_val, loop_entry
    LoopVal = loop_entry.get()

    print 'here', DelayVal

    DelayVal = []

    for row in table_rows:
        for col in row:
            # first ensure that no invalid table values were entered
            try:
                # if int(col.get()) not in delays:
                #     index = delays.index(find_nearest(delays, col.get()))
                #     ShowChoice()
                # else:
                #     index = delays.index(int(col.get()))
                # print 'col =', index
                index = delays.index(int(col.get()))
            except:
                ShowChoice()

            DelayVal.append(index)

    print 'now here', DelayVal
    total_delay = 0
    DelayVal = map(int, DelayVal)
    for i in DelayVal:
        total_delay += i
    if total_delay == 0:
        DelayVal[0] = -1

    print '\n3 here', DelayVal
    print
    print FocusStatus, ShutterStatus, LoopVal, ReverseStatus, DelayVal
    
    aC.valToArduino(FocusStatus, ShutterStatus, LoopVal, ReverseStatus, DelayVal)
    # return DelayVal


def FocusFn(btn):
    global FocusStatus, ReverseStatus, ShutterStatus, LoopVal, ReverseStatus, DelayVal
    if FocusStatus == 0:
        FocusStatus = 1
        btn.config(bg="green", fg="black", text='Focus Off')
        print 'Focus On'
    else:
        FocusStatus = 0
        btn.config(fg="white", bg="black", text='Focus On')
        print 'Focus Off'
    getTableVals()

def ReverseFn(btn):
    global FocusStatus, ReverseStatus, ShutterStatus, LoopVal, ReverseStatus, DelayVal
    
    print 'ReverseStatus before', ReverseStatus

    if ReverseStatus == 0:
        ReverseStatus = 1
        btn.config(bg="green", fg="black", text='Reverse Off')
        print 'Reverse On'
    elif ReverseStatus == 1:
        ReverseStatus = 0

        btn.config(fg="white", bg="black", text='Reverse On')
        print 'Reverse Off'
    
    print 'ReverseStatus after', ReverseStatus
    getTableVals()

def hide_me(event):
    event.widget.pack_forget()

def BounceBackFnV1(btn):
    global FocusStatus, ReverseStatus, ShutterStatus, LoopVal, ReverseStatus, DelayVal
    
    print 'ReverseStatus before', ReverseStatus  

    if ReverseStatus == 0:
        ReverseStatus = 2
        btn.config(bg="green", fg="black", text='Bounceback V1 Off')
        print 'Bounceback On'
    
    elif ReverseStatus == 2:
        ReverseStatus = 0

        btn.config(fg="white", bg="black", text='Bounceback V1 On')
        print 'Bounceback Off'
    
    print 'ReverseStatus after', ReverseStatus
    getTableVals()

def BounceBackFnV2(btn):
    global FocusStatus, ReverseStatus, ShutterStatus, LoopVal, ReverseStatus, DelayVal
    
    print 'ReverseStatus before', ReverseStatus  

    if ReverseStatus == 0:
        ReverseStatus = 3
        btn.config(bg="green", fg="black", text='Bounceback V2 Off')
        print 'Bounceback On'
    
    elif ReverseStatus == 3:
        ReverseStatus = 0

        btn.config(fg="white", bg="black", text='Bounceback V2 On')
        print 'Bounceback Off'
    
    print 'ReverseStatus after', ReverseStatus
    getTableVals()

def ShutterFn(btn):
    global FocusStatus, ReverseStatus, ShutterStatus, LoopVal, ReverseStatus, DelayVal

    ShutterStatus = 1
    btn.config(bg="white", fg="black")

    getTableVals()
    # aC.valToArduino(FocusStatus, ShutterStatus, LoopVal, ReverseStatus, DelayVal)
    ShutterStatus = 0
    btn.config(fg="white", bg="black")

    getTableVals()

def slide_float(sval):
    global DelayVal_float, DelayVal
    DelayVal_float = float(sval)
    # slide(DelayVal) 

def slide(sval):
    global FocusStatus, ReverseStatus, ShutterStatus, LoopVal, ReverseStatus, DelayVal, DelayVal_float, table_rows
    sval = float(sval) + float(DelayVal_float)
    print 'sval', sval
    delay_entry_val.set('')

    DelayVal = sval
    out = 50 * [DelayVal]

    table_rows = []
    for i in range(30+10, 20+10):
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


# quit_button = Button(text='Quit', command=quit)
# quit_button.pack(side='top', pady=5, fill='x')
# root.bind('<q>', quit)

# =========================
# code to ensure a clean exit


def exit_handler():
    print 'Quitting...'
    global FocusStatus, ReverseStatus, ShutterStatus, LoopVal, DelayVal, table_rows
    FocusStatus = 0
    ReverseStatus = 0
    ShutterStatus = 0

    aC.valToArduino(FocusStatus, ShutterStatus, LoopVal, ReverseStatus, DelayVal)
    cD.stopListening()
    aC.closeSerial()

atexit.register(exit_handler)



# ======================
# code to start the whole process
if __name__ == '__main__':

    root.lift()
    setupView()
    v = IntVar()
    v.set(0)        # initializing the choice
    root.mainloop()
