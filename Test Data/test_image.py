import sys
import PIL as Image

"""
ECE 411 Practicum Project Test Image Creation
Joshua Consenz - 11/3/25

Takes an image file location as an argument, converts it to grayscale, and then creates a c header file of the
grayscale image as a 2D matrix. Intended for use with a 480x320 pixel display 
"""
def test_image(file):
    """
    :param file: Image file path
    :return: creates a C header file with a 2D matrix

    Ideal pixel size = 480x320
    """

    img = Image.open(file).convert('LA') # Converts the image to grayscale
    img.save('grayscale.png')

    # Nested for loops to iterate through every pixel and record the magnitude of the grayscale in the file
    rows, columns = img.size
    with open('test_image.h', 'w') as out_image:
        out_image.write(f'float test_image[{rows}][{columns}]')
        out_image.write(' = {')
        for i in range(rows):
            out_image.write('{')
            for j in range(columns):
                L, A = img.getpixel((i, j))
                out_image.write('{L}'.format(L=L))
                # If statement is to prevent last element to be followed by comma
                if j != columns - 1:
                    out_image.write(',')
            out_image.write('}')
            # If statement to prevent last element from bing followed by a comma
            if i != rows - 1:
                out_image.write(',')
        out_image.write('};')

if __name__ == '__main__':
    test_image(sys.argv[1])