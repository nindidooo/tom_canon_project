import tkinter as tk

delays = [10000, 5000, 4000, 3000, 2000, 1500, 1000, 950, 900, 850, 800, 750, 700, 650, 600, 550, 500, 450, 400, 350, 300, 250, 200, 150, 100, 90, 75, 50, 48, 30, 25, 24, 23.976, 22, 16, 12, 10, 5, 2, 1]

root = tk.Tk()

v = tk.IntVar()
v.set(0)        # initializing the choice


def ShowChoice():
    print(v.get())

tk.Label(root, 
         text="""Choose your fps:""",
         justify = tk.LEFT,
         padx = 20)

for val, fps in enumerate(delays):

    btn = tk.Radiobutton(root, 
                  text=fps,
                  padx = 1, 
                  variable=v, 
                  command=ShowChoice,
                  value=val)

    if val >= len(delays)/2:
        print val
        row = 1
        col = val - len(delays)/2
    else: 
        row =0
        col = val
    btn.grid(row=row, column=col)

root.mainloop()