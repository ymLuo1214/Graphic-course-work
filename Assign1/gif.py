import os
import imageio

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
    gif_img.append(imageio.imread(imgname))
imageio.mimsave('res.gif',gif_img,fps=5)