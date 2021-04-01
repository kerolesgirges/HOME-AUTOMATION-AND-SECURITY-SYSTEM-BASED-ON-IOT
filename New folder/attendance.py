import cv2
import numpy as np
import face_recognition
import os
from datetime import datetime
import time
# from PIL import ImageGrab
#images path 
path = 'images_atten'
images = []
classNames = []
#create list of images found in path 
myList = os.listdir(path)

print(myList)#print images found there
name='keoles'
#for loop for creating list of name in my files 
for cl in myList:
  curImg = cv2.imread(f'{path}/{cl}')
  images.append(curImg)
  classNames.append(os.path.splitext(cl)[0])
print(classNames)
# all_names=classNames
# if name in classNames:
#     print('hi man')



#convert images to lists dependes on its features  
def findEncodings(images):
  encodeList = []
  for img in images:
      img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
      encode = face_recognition.face_encodings(img)[0]
      encodeList.append(encode)
  return encodeList

#open csv file to save attendance and check if name there or not 
#so save its attendance time in csv file  
def markAttendance(name):
    with open('attendace.csv','r+') as f :
        myDataList  =f.readlines()
        #print(myDataList)
        nameList=[]
        for line in myDataList:
            entry = line.split(',')
            nameList.append(entry[0])
       # if name not in nameList:
            now =datetime.now()
            dtString = now.strftime('%H:%M:%S')
            dateString=now.strftime('%D')
            f.writelines(f'\n{name},{dtString},{dateString}')


encodeListKnown = findEncodings(images)
print('encoding complete ')

 
cap = cv2.VideoCapture(0)
 
while True:
  success, img = cap.read()
  #img  = captureScreen()
  imgS = cv2.resize(img,(0,0),None,0.25,0.25)
  imgS = cv2.cvtColor(imgS, cv2.COLOR_BGR2RGB)
 
  facesCurFrame   = face_recognition.face_locations(imgS)
  encodesCurFrame = face_recognition.face_encodings(imgS,facesCurFrame)
 
  for encodeFace,faceLoc in zip(encodesCurFrame,facesCurFrame):
    matches = face_recognition.compare_faces(encodeListKnown,encodeFace)
    faceDis = face_recognition.face_distance(encodeListKnown,encodeFace)
    #print(faceDis)
    matchIndex=np.argmin(faceDis)
    if matches[matchIndex]:
        name =classNames[matchIndex].upper()
        #print (name)
        y1,x2,y2,x1 =faceLoc
        y1,x2,y2,x1 =y1*4,x2*4,y2*4,x1*4     # return scale to ins original scale
        cv2.rectangle(img,(x1,y1),(x2,y2),(0,255,0),2)
        cv2.rectangle(img,(x1,y1-35),(x2,y2),(0,255,0),2)
        cv2.putText(img,name,(x1+6,y2-6),cv2.FONT_HERSHEY_COMPLEX,1,(255,255,255),2)
       # markAttendance(name)
    
    cv2.imshow('Webcam',img) 
    #print (name)

  #  if name.lower() in classNames:
    #     print("is me ")
        
  
  if (cv2.waitKey(1) == 13)or (name.lower() in classNames):  #13 is the Enter Key

    print(name,'HI ,door unlocked')  
    markAttendance(name)
    break
  #print(faceDis)
  
cap.release()
cv2.destroyallwindows() 

