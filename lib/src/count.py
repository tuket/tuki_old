#!/usr/bin/env python
commentSymbol = "//"

import sys
import os, os.path
import Tkinter
import tkMessageBox

top = Tkinter.Tk()
top.geometry('220x100')

extensions = sys.argv[1:]
extensions.extend([".cpp", ".hpp"])

currentDir = os.getcwd()

codeFiles = []
for root, _, files in os.walk(currentDir):
    for f in files:
        fullpath = os.path.join(root, f)
        if '.git' not in fullpath:
            for extension in extensions:
            	if fullpath.endswith(extension):
                    codeFiles.append(fullpath)


numLines = 0
blankLines = 0
commentLines = 0

for fileToCheck in codeFiles:
    with open(fileToCheck) as f:
        for line in f:
            numLines += 1

            lineWithoutWhitespace = line.strip()
            if not lineWithoutWhitespace:
                blankLines += 1
            elif lineWithoutWhitespace.startswith(commentSymbol):
                commentLines += 1

res =  'Lines:         ' + str(numLines) + '\n'
res += 'Blank lines:   ' + str(blankLines) + '\n'
res += 'Comment lines: ' + str(commentLines) + '\n'
res += 'Code lines:    ' + str(numLines - blankLines - commentLines) + '\n'

text = Tkinter.Text(top)
text.insert(Tkinter.END, res)
text.pack()

top.mainloop()