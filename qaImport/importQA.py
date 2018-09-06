# -*- coding: utf-8 -*-

import sqlite3
import xlrd
import xlwt
import sys

G_CONN = ''

#连接sqlite
def connectSql(dbpath):
    try:
        global G_CONN
        G_CONN = sqlite3.connect(dbpath)
        return True
    except Exception as e:
        print("here is :%s%d" % (__file__, sys._getframe().f_lineno))
        print("Error:%s" % e)
        return False

#读取execl
def readExecl(execlpath):
    # 读取 打开工作簿
    workbook = xlrd.open_workbook(execlpath)

    # 获取sheet 取得工作簿的第一个表名
    sheet_name = workbook.sheet_names()[0]

    # 通过表名称获取表名
    sheet = workbook.sheet_by_name(sheet_name)

    # 条目的总数
    print("nrows =", sheet.nrows)
    print("ncols =", sheet.ncols)

    groupId=0
    classifyId=0
    question=''
    answer=''
    audio=''
    isEnd=0
    isBreak=0

    for i in range(sheet.nrows):
        if i==0:
            continue

        temp = sheet.row_values(i)
        print(temp)

        #第一列 组id
        if temp[0]:
            groupId=int(temp[0])
            print("groupId=",groupId)
        #第二列 question
        if len(temp[1]):
            question=temp[1]
            print("question=",question)
            #增加一条qa记录
            setQuestion(groupId, question)
        #第三列 answer
        if len(temp[2]):
             answer=temp[2]
             print("answer=", answer)
             # 增加一条qa记录
             setAnswer(groupId, answer)
        #第四列 type
        if len(temp[3]):
             type=temp[3]
             #得到type对应的classifyId
             classifyId= getClassifyId(type)
             print("classifyId=", classifyId)
        #第五列 audio
        if len(temp[4]):
             audio=temp[4]
             print("audio=",audio)
             #第六列 isEnd
             isEnd = int(temp[5])
             print("isEnd=", isEnd)
             #第七列 isBreak
             isBreak = int(temp[6])
             print("isBreak=", isBreak)
             # 增加一条group记录
             setGroup(groupId, classifyId, audio, isEnd, isBreak)

#插入question
def setQuestion(groupId, question):
    global G_CONN
    G_CONN.execute("insert into T_QA_LIST(F_GROUP_ID, F_TYPE, F_SENTENCE) values(%d, 'question','%s' )"%(groupId, question))
    G_CONN.commit()
#插入answer
def setAnswer(groupId, answer):
    global G_CONN
    G_CONN.execute("insert into T_QA_LIST(F_GROUP_ID, F_TYPE, F_SENTENCE) values(%d, 'answer','%s' )"%(groupId, answer))
    G_CONN.commit()

#插入qagroup
def setGroup(groupId, classifyId, audio, isEnd, isBreak):
    global G_CONN
    G_CONN.execute("insert into T_QA_GROUP_LIST(F_GROUP_ID, F_CLASSIFY_ID, F_AUDIO, F_IS_END, F_IS_BREAK) values(%d, %d, '%s', %d, %d)"%(groupId, classifyId, audio, isEnd, isBreak))
    G_CONN.commit()

#得到classifyId
def getClassifyId(type):
    global G_CONN
    cursorFD = G_CONN.execute("select F_ID from T_QA_CLASSIFY_LIST where F_TYPE = '%s'"%type)
    result = cursorFD.fetchall()
    for oneResult in result:
        return int(oneResult[0])

def mainLoop(execlPath, dbPath):
    global G_CONN
    connectSql(dbPath)
    readExecl(execlPath)
    G_CONN.close()
if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Wrong Parameters")
        sys.exit()
    execlPath = sys.argv[1]
    dbPath = sys.argv[2]
    mainLoop(execlPath, dbPath)