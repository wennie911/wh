#coding:utf-8

from filter import Cfilter
import os,sys,string
import json

def parseJson(jsonContent):
    #print "jsoncontent",jsonContent
    retAnswer = json.loads(jsonContent)
    if not len(retAnswer):
        return False
    #print "retAnswer",retAnswer
    print retAnswer['answer']
    return int(retAnswer['isEnd'])
        
def mainLoop():
  connectFlag=1
  aa = Cfilter()
  aa.initFilter()
  print aa.get('开场白')
  while connectFlag:
    print  '请输入句子(exit退出)：'
    inSentence = raw_input()
    if inSentence == 'exit':
       break
    if len(inSentence) == 0:
       continue
    
    retStr =  aa.get(inSentence)
    print "retStr",retStr
    if parseJson(retStr)==True:
       break;
    
    
if __name__ == '__main__':
    mainLoop()
