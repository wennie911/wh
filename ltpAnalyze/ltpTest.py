#coding:utf-8

import os, sys, string, copy
from ltpProcess import CLTPrequest
from ltpProcess import CLTPparse
from ltpProcess import  CLTPanalyze

def mainLoop(sentence, channel):
    ltpR = CLTPrequest(channel)

    retJson = ltpR.getLTPresult(sentence)
    if not len(retJson):
        return
    #print(retJson)
    ltpJ = CLTPparse()
    AltpList = ltpJ.getLTPList(retJson)
    if not len(AltpList):
        return
    for oneList in AltpList:
        #print('*********************',oneList)
        ltpA = CLTPanalyze(oneList)
        analyseList = ltpA.getAllClause()
        for ana in analyseList:
            #print(ana)
            #ltpA.syntaxPrint(ana)
            strFormat = ltpA.syntaxFormat(ana)
            print(strFormat)
            print('--------------------------------------')

if __name__ == '__main__':
    sentence = sys.argv[1]
    channel = sys.argv[2]
    mainLoop(sentence, channel)