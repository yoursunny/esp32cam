'''
steps to exicute this application:
1. exicute the WifiCam.ion file in ESP32 cam module which is available at examples->WifiCam->WifiCam.ion.
2. before exicution make shure that the wifi cretentials are saves accordingly.
3. After uploading the code to the esp32 restart the esp32 cam module.
4. open the serial monitor and note down the IP address and chane the 18th line of this code with that ip address
5. Now exicute this python code in local computer and make shure that the computer is also connected to the same netwok
6. Now you can view the camera output in the windows poped by the python and see the face and ee detection working in real time.

'''

import cv2
import urllib.request
import numpy as np
 
f_cas= cv2.CascadeClassifier(cv2.data.haarcascades+'haarcascade_frontalface_default.xml')
eye_cascade=cv2.CascadeClassifier(cv2.data.haarcascades +'haarcascade_eye.xml')
url='http://xxx.xxx.xxx.xxx/cam-lo.jpg' #change the IP address to the IP address to which the ESP32 cam is connected to
##'''cam.bmp / cam-lo.jpg /cam-hi.jpg / cam.mjpeg '''
cv2.namedWindow("Live Transmission", cv2.WINDOW_AUTOSIZE)
 
while True:
    img_resp=urllib.request.urlopen(url)
    imgnp=np.array(bytearray(img_resp.read()),dtype=np.uint8)
    img=cv2.imdecode(imgnp,-1)
    gray=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    face=f_cas.detectMultiScale(gray,scaleFactor=1.1,minNeighbors=5)
    for x,y,w,h in face:
        cv2.rectangle(img,(x,y),(x+w,y+h),(0,0,255),3)
        roi_gray = gray[y:y+h, x:x+w]
        roi_color = img[y:y+h, x:x+w]
        eyes = eye_cascade.detectMultiScale(roi_gray)
        for (ex,ey,ew,eh) in eyes:
            cv2.rectangle(roi_color,(ex,ey),(ex+ew,ey+eh),(0,255,0),2)
 
 
    cv2.imshow("live transmission",img)
    key=cv2.waitKey(5)
    if key==ord('q'):
        break
 
cv2.destroyAllWindows()