# coding:utf-8

import  urllib.request
import  urllib.parse
import os, sys, string, copy
import json
import ssl

URL_LOCAL=u'http://120.76.26.132:8563/ltp'
URL_ONLINE= u'https://api.ltp-cloud.com/analysis/'
API_KEY='k1O7C3R9h0WfQYHJddKIReVUarsQkFnxDeqVyTmW'

class CLTPrequest(object):
    def __init__(self, channel):
        self.url_local = u'http://120.76.26.132:8563/ltp'
        self.url_online=u'https://api.ltp-cloud.com/analysis/'
        self.api_key='k1O7C3R9h0WfQYHJddKIReVUarsQkFnxDeqVyTmW'
        self.onLine = False
        self.ltpRet=''
        if channel == 'local':
            self.onLine = False;#本地
        else:
            self.channel = True;#在线

    def getLTPresult(self,sentence):
        if self.onLine:
            if self.requestLtpLocal(sentence):
                return self.ltpRet
        else:
            if self.requestLtpOnline(sentence):
                return self.ltpRet


    def requestLtpOnline(self, sentence):
        if not len(self.url_online):
            return False
        if not len(self.api_key):
            return False;
        self.ltpRet=''
        print('requestLtpOnline',sentence)
        ssl._create_default_https_context = ssl._create_unverified_context
        args = {
            'api_key':self.api_key,
            'text':sentence,
            'pattern':'dp',
            'format':'json'
        }
        vargs = urllib.parse.urlencode(args)
        vargs = vargs.encode(encoding='UTF8')
        try:
            result = urllib.request.urlopen(self.url_online, vargs, )
            self.ltpRet = result.read().decode('utf-8')

            return True
        except Exception as e:
            print("here is :%s%d" % (__file__, sys._getframe().f_lineno))
            print("Error:%s" % e)
            return False

    def requestLtpLocal(self, sentence):
        if not len(self.url_local):
            return False
        self.ltpRet=''
        data = {'s':sentence, 'type':'dp'}
        try:
            result = urllib.request.urlopen(self.url_local, data=urllib.parse.urlencode(data).encode('utf-8'))

            self.ltpRet = result.read().decode('utf-8')
            #print('requestLtpLocal ltpRet len =', len(self.ltpRet))
            return True
        except Exception as e:
            print("here is :%s%d" % (__file__, sys._getframe().f_lineno))
            print("Error:%s" % e)
            return False

class CLTPparse(object):
    def getLTPList(self, ltpJson):
        if not len(ltpJson):
            return ''

        vData = json.loads(ltpJson)
        AltiList=[]
        ltpList=[]
        for data in vData:
            for data1 in data:
                ltpList=[]
                for data2 in data1:
                    oneltp={}
                    if data2['relate'] == 'WP':#标点符号不要
                        continue
                    oneltp['relate'] = data2['relate']
                    oneltp['parent'] = data2['parent']
                    oneltp['id'] = data2['id']
                    oneltp['pos'] = data2['pos']
                    oneltp['cont'] = data2['cont']
                    oneltp['used'] = False
                    oneltp['found'] = False
                    ltpList.append(oneltp)
                AltiList.append(ltpList)
        return AltiList

class CLTPanalyze(object):
    def __init__(self, ltpList):
        self.ltpList = ltpList
        self.count = 0
        self.firstNodeId = -1

    def clauseFromLTPList(self):
        #以父节点-1为起点，得到所有所有分支
        #找到根节点的一级子节点id
        self.firstNodeId = -1
        for oneltp in self.ltpList:
            if oneltp['parent']==-1:
                self.firstNodeId = oneltp['id']
                break

        rootList = []

        for oneltp in self.ltpList:
            if oneltp['used']:
                continue
            if oneltp['parent'] == -1:
                 oneltp['used'] = True
                 oneRoot=[]
                 oneRoot.append(oneltp)
                 rootList.append(oneRoot)
                 continue
            if oneltp['parent'] == self.firstNodeId and oneltp['relate'] == 'COO':
                 oneltp['used'] = True
                 oneRoot=[]
                 oneRoot.append(oneltp)
                 rootList.append(oneRoot)
                 continue
        print('rootList', rootList)
        if not len(rootList):
            return rootList

        self.count=0
        while self.isNotUesd():

            for oneRoot in rootList:
                for oneChild in oneRoot:
                    if oneChild['found']:
                        continue
                    for oneltp in self.ltpList:
                        if oneltp['used']:
                            continue
                        if oneltp['parent'] == oneChild['id']:
                            oneltp['used'] = True
                            #print('oneltp',oneltp['cont'])
                            oneRoot.append(oneltp)
                    oneChild['found'] = True
                    break

        return rootList

    def isNotUesd(self):
        if self.count >=10:
            return False
        for oneltp in self.ltpList:
            if not oneltp['used']:
                self.count += 1;
                return True
        return False

    def getAllClause(self):
        return self.clauseFromLTPList()


    def syntaxPrint(self, ltpList):
        sortList = self.sortLtp(ltpList)
        #zy
        print('主语：',sortList['zy'])
        print('主语修饰：', sortList['zyxs'])
        print('谓语：', sortList['wy'])
        print('谓语修饰：', sortList['wyxs'])
        print('宾语：', sortList['by'])
        print('宾语修饰：', sortList['byxs'])

    def syntaxFormat(self,ltpList):
        sortList = self.sortLtp(ltpList)
        strFormat = ''
        for one in sortList['zyxs']:
            strFormat += one

        for one in sortList['zy']:
            strFormat += one

        for one in sortList['wyxs']:
            strFormat += one

        for one in sortList['wy']:
            strFormat += one

        for one in sortList['byxs']:
            strFormat += one

        for one in sortList['by']:
            strFormat += one

        return strFormat

    def sortLtp(self, ltpList):
        cooFlag = 0
        currentId = 0
        contsZY = []
        contsZYXS = []
        contsWY = []
        contsWYXS = []
        contsBY = []
        contsBYXS = []

        zyId = 0
        for oneLtp in ltpList:
            if oneLtp['relate'] == 'SBV':
                cooFlag = 1
                currentId = oneLtp['id']
                zyId = oneLtp['id']
                contsZY.append(oneLtp['cont'])
            if cooFlag:
                if oneLtp['relate'] == 'COO' and oneLtp['id'] == currentId + 1:
                    currentId = oneLtp['id']
                    contsZY.append(oneLtp['cont'])
        for oneLtp in ltpList:
            if oneLtp['parent'] == zyId:
                if oneLtp['relate'] == 'ATT' or oneLtp['relate'] == 'ADV' or oneLtp['relate'] == 'CMP' or oneLtp[
                    'relate'] == 'POB':
                    contsZYXS.append(oneLtp['cont'])

        cooFlag = 0
        currentId = 0
        wyId = 0
        for oneLtp in ltpList:
            if oneLtp['relate'] == 'HED' or (oneLtp['relate'] == 'COO' and oneLtp['parent'] == self.firstNodeId):
                contsWY.append(oneLtp['cont'])
                wyId = oneLtp['id']

        for oneLtp in ltpList:
            if oneLtp['parent'] == wyId:
                if oneLtp['relate'] == 'ATT' or oneLtp['relate'] == 'ADV' or oneLtp['relate'] == 'CMP' or oneLtp[
                    'relate'] == 'POB':
                    contsWYXS.append(oneLtp['cont'])

        cooFlag = 0
        currentId = 0
        byId = 0
        for oneLtp in ltpList:
            if oneLtp['relate'] == 'VOB' or oneLtp['relate'] == 'IOB' or oneLtp['relate'] == 'FOB' or oneLtp[
                'relate'] == 'POB':
                cooFlag = 1
                contsBY.append(oneLtp['cont'])
                currentId = oneLtp['id']
                byId = oneLtp['id']
            if cooFlag:
                if oneLtp['relate'] == 'COO' and oneLtp['id'] == currentId + 1:
                    contsBY.append(oneLtp['cont'])
                    currentId = oneLtp['id']

        for oneLtp in ltpList:
            if oneLtp['parent'] == byId:
                if oneLtp['relate'] == 'ATT' or oneLtp['relate'] == 'ADV' or oneLtp['relate'] == 'CMP' or oneLtp[
                    'relate'] == 'POB':
                    contsBYXS.append(oneLtp['cont'])

        ltpSort = {}
        ltpSort['zy'] = contsZY
        ltpSort['zyxs'] = contsZYXS
        ltpSort['wy'] = contsWY
        ltpSort['wyxs'] = contsWYXS
        ltpSort['by'] = contsBY
        ltpSort['byxs'] = contsBYXS
        return ltpSort
