#coding:utf-8

import os, sys, string, copy, re
from ltpProcess import CLTPrequest
from ltpProcess import CLTPparse
from ltpProcess import  CLTPanalyze

import xlrd
import xlwt

def readExecl(execlpath):
    # 读取 打开工作簿
    workbook = xlrd.open_workbook(execlpath)
    questionList={}
    # 获取sheet 取得工作簿的第一个表名
    sheet_name = workbook.sheet_names()[0]

    # 通过表名称获取表名
    sheet = workbook.sheet_by_name(sheet_name)

    # 条目的总数
    print("nrows =", sheet.nrows)
    print("ncols =", sheet.ncols)

    for i in range(sheet.nrows):
        temp = sheet.row_values(i)
        print(temp[0])
        if temp[0]<=0:
            break

        subSentence = splitSentence(temp[2])
        questionList[int(temp[0])] = subSentence
        #return questionList

    return questionList

#把长句分解成短句 换行 句号
def splitSentence(sentence):
    ts=re.split('[\n\r。]', sentence)
    subSentence=[]
    for ots in ts:
        if not len(ots.strip()):
            continue
        subSentence.append(ots)
    print(subSentence)
    return subSentence

def writeExecl(ExeclPath,QuestionList, AnswerList):
    workbook = xlwt.Workbook()
    sheet = workbook.add_sheet('sheet1')

    rowCount = 0
    for key, values in QuestionList.items():
        sheet.write(rowCount, 0, key)
        sheet.write(rowCount, 1, values)
        oneAnswer = AnswerList[key]
        for oneA in oneAnswer:
            sTemp=''
            for zyxs in oneA['zyxs']:
                sTemp += zyxs
                sTemp += '|'
            sheet.write(rowCount, 2, sTemp)
            sTemp = ''
            for zy in oneA['zy']:
                sTemp += zy
                sTemp += '|'
            sheet.write(rowCount, 3, sTemp)
            sTemp = ''
            for wyxs in oneA['wyxs']:
                sTemp += wyxs
                sTemp += '|'
            sheet.write(rowCount, 4, sTemp)
            sTemp = ''
            for wy in oneA['wy']:
                sTemp += wy
                sTemp += '|'
            sheet.write(rowCount, 5, sTemp)
            sTemp = ''
            for byxs in oneA['byxs']:
                sTemp += byxs
                sTemp += '|'
            sheet.write(rowCount, 6, sTemp)
            sTemp = ''
            for by in oneA['by']:
                sTemp += by
                sTemp += '|'
            sheet.write(rowCount, 7, sTemp)
            rowCount +=1
    workbook.save(ExeclPath)

def mainLoop(execlPath, channel):
    questionList={}
    questionList = readExecl(execlPath)
    answerList={}
    if not len(questionList):
        return

    ltpR = CLTPrequest(channel)
    for key,value in questionList.items():
        print('row=',key)
        print('value=',value)
        ltpzwb = []
        for oneV in value:
            retJson = ltpR.getLTPresult(oneV)
            if not len(retJson):
                continue
            ltpJ = CLTPparse()
            AltpList = ltpJ.getLTPList(retJson)
            if not len(AltpList):
                return
            for oneList in AltpList:
                ltpA = CLTPanalyze(oneList)
                analyseList = ltpA.getAllClause()
                for ana in analyseList:
                    sortL = ltpA.sortLtp(ana)
                    ltpzwb.append(sortL)
        answerList[key] = ltpzwb

    #print(answerList)

    writeExecl('yy.xls', questionList, answerList)
    return


if __name__ == '__main__':
    if len(sys.argv) < 1:
        print("Wrong Parameters execlpath etype")
    elif len(sys.argv) >=1:
        execlPath = sys.argv[1]
        channel = sys.argv[2]
        mainLoop(execlPath, channel)
        exit(0)