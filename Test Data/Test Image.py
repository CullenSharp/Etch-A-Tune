import math
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
import operator
import matplotlib.image as mpimg
import scipy as sp


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
        out_file.write('{0} test_vector[] = {')
        for sample in signal:
            out_file.write(f'{sample},')
        out_file.write('};')

    with open('test_image.h', 'w') as out_image:
        out_image.write('{0} test_image[] = {')
        for sample in image:
            out_image.write(f'{sample},')
        out_image.write('};')

ts = np.linspace(start, end, sample_rate)
signal = linear_chirp(ts)
plt.plot(ts, signal)
plt.show()

img = Image.open('Test Image A 480x320.png').convert('LA')
img.save('grayscale.png')
img = np.array(img)

main(signal, img)