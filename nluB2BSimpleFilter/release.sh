#!/bin/bash
PLATFORM=$1
LOCAL_PATH=$PWD
ENCRYPT=encryptdatalibs
echo "local path=\"$LOCAL_PATH\""
if [ $PLATFORM = "linux" ]
then
  echo "you are choose platform "$PLATFORM" .";
else
  echo "warning: you mush give me a platform such as rk3066 rk3288 linux, or I can do nothing for you !";
  exit 0;
fi

rm -rf libnlu.so

make

if [ ! -f "libnlu.so" ]
then
  echo "make error !";
  exit 0;
fi

echo "make success !"


#echo "Begain decode database..."
#rm -rf $ENCRYPT/
cd dataLibs
#chmod +x sqlite3E
#python GenerateEncryptDB.py
cp config.xml ./
#mv $ENCRYPT/ $LOCAL_PATH
cd ..

echo "decode database over!"
