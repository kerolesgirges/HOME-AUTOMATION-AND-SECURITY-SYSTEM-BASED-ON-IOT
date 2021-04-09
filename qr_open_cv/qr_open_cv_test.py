import cv2
import numpy as np 
from pyzbar.pyzbar import decode

#img = cv2.imread('1.png')
#code= decode(img)
cap =cv2.VideoCapture(0)
#define width and hight 

cap.set(3,640) #width id =3
cap.set(4,480) #hight id =4
with open('mydatafile.text') as f :
    myDataList = f.read().splitlines()

#print(myDataList)
while True:
    success,img = cap.read()
    for barcode in decode(img):
        print(barcode.data)
        #you will find b infront of your id it means it's in bits 
        #convert your data into numbers 
        myData = barcode.data.decode('utf-8') 
        #so only our data only here 
        print(myData)

        if myData in myDataList:
            #print('allowed')
            output ='allowed'
            door_stat ='door_unlocked'
        else:
            #print('not allowed')
            output ='not-allowed'
            door_stat ='door_locked'
        pts =np.array([barcode.polygon],np.int32)
        pts =pts.reshape((-1,1,2))
        #create polygon around qr
        cv2.polylines(img,[pts],True,(255,0,255))
        
        pts2=barcode.rect
        cv2.putText(img,output,(pts2[0],pts2[1]),cv2.FONT_HERSHEY_COMPLEX,0.9,(255,0,255),2)
        cv2.putText(img,door_stat,(pts2[0],pts2[1]+150),cv2.FONT_HERSHEY_COMPLEX,0.9,(255,0,255),2)

    cv2.imshow('result',img) 
   
    if (cv2.waitKey(1) == 13):  #13 is the Enter Key

       #  print(name,'HI ,door unlocked')  
        
     break

#print (code)


