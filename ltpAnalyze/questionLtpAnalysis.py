#coding:utf-8

import os, sys, string, copy
from ltpProcess import CLTPrequest
from ltpProcess import CLTPparse
from ltpProcess import  CLTPanalyze

import xlrd
import xlwt

def readExecl(execlpath):
    # 读取 打开工作簿
    workbook = xlrd.open_workbook(execlpath)
    questionList=[]
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
        if len(temp[0])<=0:
            break
        questionList[temp[0]] = temp[1]
        return questionList
        #print(temp[0])
    return questionList

def writeExecl(ExeclPath,QuestionList, AnswerList):
    workbook = xlwt.Workbook()
    sheet = workbook.add_sheet('sheet1')

    rowCount = 0
    for key, values in QuestionList.items():
        sheet.write(rowCount, 0, values)
        for oneAnswer in AnswerList[key]:
            sTemp=''
            for zyxs in oneAnswer['zyxs']:
                sTemp += zyxs
            sheet.write(rowCount, 1, values)
            sTemp = ''
            for zy in oneAnswer['zy']:
                sTemp += zyxs
            sheet.write(rowCount, 2, values)
            sTemp = ''
            for zyxs in oneAnswer['wyxs']:
                sTemp += zyxs
            sheet.write(rowCount, 3, values)
            sTemp = ''
            for wy in oneAnswer['wy']:
                sTemp += zyxs
            sheet.write(rowCount, 4, values)
            sTemp = ''
            for zyxs in oneAnswer['byxs']:
                sTemp += zyxs
            sheet.write(rowCount, 5, values)
            sTemp = ''
            for by in oneAnswer['by']:
                sTemp += zyxs
            sheet.write(rowCount, 6, values)
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
        retJson = ltpR.getLTPresult(value)
        if not len(retJson):
            continue
        ltpJ = CLTPparse()
        AltpList = ltpJ.getLTPList(retJson)
        if not len(AltpList):
            return
        ltpzwb=[]
        for oneList in AltpList:
            ltpA = CLTPanalyze(oneList)
            analyseList = ltpA.getAllClause()
            for ana in analyseList:
                sortL = ltpA.sortLtp(ana)
                ltpzwb.append(sortL)
        answerList[key] = ltpzwb

    return


if __name__ == '__main__':
    if len(sys.argv) < 1:
        print("Wrong Parameters execlpath etype")
    elif len(sys.argv) >=1:
        execlPath = sys.argv[1]
        channel = sys.argv[2]
        mainLoop(execlPath, channel)
        exit(0)