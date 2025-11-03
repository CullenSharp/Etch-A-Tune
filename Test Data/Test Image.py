import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
import cv2


PI2 = np.pi*2

start = 0
end = 2
sample_rate = 4800
data_type = 'float'

def linear_chirp(ts, low=440, high=880, amp=1.0):
    """Compute the waveform of a linear chirp
    
    low: starting frequency
    high: ending frequency
    amp: amplitude
    """
    freqs = np.linspace(low, high, len(ts) - 1)

    dts = np.diff(ts)

    dphis = np.insert(PI2 * freqs * dts, 0, 0)

    phases = np.cumsum(dphis)

    ys = amp * np.cos(phases)

    return ys


def main(signal, image):
    '''
        Write code to generate an interesting signal.
        This could be a simple tone (e.g. A4), 
        But there are bonus points for a signal with a more complicated spectrum (e.g. a chirp).
    '''

    with open('test_vector.h', 'w') as out_file:
        out_file.write(f'float test_vector[{len(signal)}]')
        out_file.write(' = {')
        for i in range(1,len(signal)):
            out_file.write(f'{signal[i]}')
            '''Loop to append comma to element, and stops at last element to remove excess comma'''
            if i != len(signal) - 1:
                    out_file.write(',')
        out_file.write('};')

    rows, columns = image.size
    with open('test_image.h', 'w') as out_image:
        out_image.write(f'float test_image[{rows}][{columns}]')
        out_image.write(' = {')
        for i in range(rows):
            out_image.write('{')
            for j in range(columns):
                L,A = image.getpixel((i,j))
                out_image.write('{L}'.format(L=L))
                if j != columns - 1:
                    out_image.write(',')
            out_image.write('}')
            if i != rows - 1:
                out_image.write(',')
        out_image.write('};')

ts = np.linspace(start, end, sample_rate)
signal = linear_chirp(ts)

img = Image.open('Test Image A 480x320.png').convert('LA')
img.save('grayscale.png')
#img = cv2.imread('grayscale.png')

main(signal, img)