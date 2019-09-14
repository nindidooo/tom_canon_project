from Tkinter import *

class Slider(Frame):
    def __init__(self, parent=None ):
        Frame.__init__(self, parent)
        self.number = 0
        self.slide = Scale(self, orient=HORIZONTAL, command=self.setValue,
                           length=200, sliderlength=20,
                           showvalue=0, tickinterval=1,
                           fro=4, to=9, font=('Arial',9))
        self.text = Label(self, font=('Arial',18))
        self.slide.pack(side=RIGHT, expand=1, fill=X)
        self.text.pack(side=TOP, fill=BOTH)
        self.unimap = {'4':u'\u2074','5':u'\u2075','6':u'\u2076',
                       '7':u'\u2077','8':u'\u2078','9':u'\u2079'}

    def setValue(self, val):
        self.number = (10**(int(val)))
        self.text.configure(text='10%s' %(self.unimap[val]))

root = Tk()
s = Slider(root)
s.pack()
root.mainloop()