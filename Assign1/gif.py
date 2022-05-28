import os
import imageio
import cv2

def getPNGFile(path):
    files=os.listdir(path)
    result=[]
    for f in files:
        if os.path.splitext(f)[-1]=='.png':
            result.append(f)
    result.sort(key=lambda x:int(x.split('.')[0]))
    return result



img_path='./build/'
imgfile=getPNGFile(img_path)
gif_img=[]
for img in imgfile:
    imgname=img_path+img
    image=imageio.imread(imgname)
    image=cv2.resize(image,(300,300))
    gif_img.append(image)
imageio.mimsave('res.gif',gif_img,fps=5)