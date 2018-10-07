#!/usr/bin/python
# -*- coding: UTF-8 -*-
import time
import urllib.request
import urllib.parse
import json
import hashlib
import base64


def main():
    body = urllib.parse.urlencode({'text': '我晚上吃了个苹果。'}).encode('utf-8')

    url = 'http://ltpapi.xfyun.cn/v1/cws'
    api_key = '8241015c281d8d11119dbdde6383da51'
    param = {"type": "dependent"}

    x_appid = '5baf2bce'
    x_param = base64.b64encode(json.dumps(param).replace(' ', '').encode('utf-8'))
    x_time = int(int(round(time.time() * 1000)) / 1000)
    x_checksum = hashlib.md5(api_key.encode('utf-8') + str(x_time).encode('utf-8') + x_param).hexdigest()
    x_header = {'X-Appid': x_appid,
                'X-CurTime': x_time,
                'X-Param': x_param,
                'X-CheckSum': x_checksum}
    req = urllib.request.Request(url, body, x_header)
    result = urllib.request.urlopen(req)
    result = result.read()
    print(result.decode('utf-8'))
    return


if __name__ == '__main__':
    main()