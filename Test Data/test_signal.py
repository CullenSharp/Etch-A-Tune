import numpy as np
"""
ECE 411 Practicum Project Test Signal Creation
Joshua Consenz - 11/3/25

When called, creates a chirp signal of variable length between two variable frequencies.
The output is recorded into a C header file for testing purposes
Default values:
    time: 2s
    sample_rate: 4800
    low pitch: A4 440 Hz
    high pitch: A5 880 Hz
    amplitude: 1.0
"""
def test_signal(start = 0, end = 2, sample_rate = 4800, low = 440, high = 880, amp = 1.0):
    """
    :param start: start time of signal
    :param end: end time of signal
    :param sample_rate: sample rate of signal
    :param low: low frequency chirp start
    :param high: high frequency chirp end
    :param amp: amplitude of signal
    :return: a C header file
    """

    # Creates linear chirp from starting to ending frequency
    PI2 = 2*np.pi
    ts = np.linspace(start, end, sample_rate)
    freqs = np.linspace(low, high, len(ts) - 1)
    dts = np.diff(ts)
    dphis = np.insert(PI2 * freqs * dts, 0, 0)
    phases = np.cumsum(dphis)
    ys = amp * np.cos(phases)

    # For loop to iterate through each element of the chirp, and record it in the output file
    with open('test_vector.h', 'w') as out_file:
        out_file.write(f'float test_vector[{len(ys)}]')
        out_file.write(' = {')
        for i in range(1,len(ys)):
            out_file.write(f'{ys[i]}')
            # Loop to append comma to element, and stops at last element to remove excess comma
            if i != len(ys) - 1:
                    out_file.write(',')
        out_file.write('};')



if __name__ == '__main__':
    test_signal()

