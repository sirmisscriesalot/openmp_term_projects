import argparse 
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

parser = argparse.ArgumentParser(description="load log files")
parser.add_argument('--log', help = "location of text file")
parser.add_argument('--inimg', help = "location of the input image file")
parser.add_argument('--outimg', help = "location of the output image file")

args = parser.parse_args()

log_file = args.log
input_img = args.inimg
output_img = args.outimg

x = [0 for i in range(256)]
y_var = [0 for i in range(256)]
y_norm = [0 for i in range(256)]

with open(log_file) as f:
    for i,line in enumerate(f):
        x[i],y_var[i],y_norm[i] = map(int,line.split(' '))

fig, axs = plt.subplots(2,2,figsize = (14,10))

axs[0,0].plot(x,y_var)
axs[0,0].set_title('histogram of original image')
axs[0,0].set_xlabel('pixel intensity')

axs[0,1].plot(x,y_norm)
axs[0,1].set_title('histogram of equalized image')
axs[0,1].set_xlabel('pixel intensity')

axs[1,0].imshow(mpimg.imread(input_img),cmap='gray')
axs[1,0].set_title('original image')

axs[1,1].imshow(mpimg.imread(output_img),cmap='gray')
axs[1,1].set_title('new image')

plt.show()