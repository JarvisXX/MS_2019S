from PIL import Image


# draw figure
def draw_feature(filename, array, save=0):
    # if len(array.shape) == 1:
    #     x = 20
    # else:
    #     x = 20 * array.shape[1]
    # y = 20 * array.shape[0]
    x = array.shape[0] # 7
    y = array.shape[1] # 7
    num = array.shape[2] # 16
    image_x = x
    image_y = y * num
    im = Image.new("RGB", (image_x, image_y))
    for i in range(image_x):
        for j in range(image_y):
            array_x = i
            array_y = j % y
            array_num = int(j / y)
            value = array[array_y][array_x][array_num]
            saturation = int(255 * value)
            im.putpixel((i, j), (255-saturation, 255-saturation, 255-saturation))
        # raw_input()
    if save == 0:
        im.show(filename)
    else:
        im.save('fig/' + filename + '.png')


def draw_image(filename, array, save=0):
    # if len(array.shape) == 1:
    #     x = 20
    # else:
    #     x = 20 * array.shape[1]
    # y = 20 * array.shape[0]
    x = array.shape[0] # 7
    y = array.shape[1] # 7
    image_x = x
    image_y = y
    im = Image.new("RGB", (image_x, image_y))
    for i in range(image_x):
        for j in range(image_y):
            value = array[j][i]
            saturation = int(value)
            im.putpixel((i, j), (255-saturation, 255-saturation, 255-saturation))
        # raw_input()
    if save == 0:
        im.show(filename)
    else:
        im.save('fig/' + filename + '.png')