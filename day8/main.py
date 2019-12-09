import math

height = 6
width = 25

# Given the image with multiple layers, return the forground pixel
def primaryPixel(image, x, y):
    for l in range(len(image)):
        if image[l][y][x] == 0 :
            return " "
        elif image[l][y][x] == 1 :
            return "X"
    return " "

with open("input.txt","r") as f:
    # Read Input
    line =f.readline().strip()
    # Calculate number of layers
    numLayers = int(len(line)/(height * width))

    # Form into the image composed of layers
    image = [[[ int(line[(l*height*width)+(y*width)+x]) for x in range(width)] for y in range(height)] for l in range(numLayers)]

    # Print it
    # for l in range(numLayers):
    #     print()
    #     for y in range(height):
    #         print()
    #         for x in range(width):
    #             print(image[l][y][x], end = "")
    #
    # print()

    # Part 1 : Find layer with least 0, return number of two's * number of one's
    leastNum = math.inf
    oneTwo = math.inf
    leastLayer = 0
    for l in range(numLayers):
        countZero = 0
        countOne = 0
        countTwo = 0
        for y in range(height):
            for x in range(width):
                if image[l][y][x] == 0:
                    countZero +=1
                if image[l][y][x] == 1:
                    countOne +=1
                if image[l][y][x] == 2:
                    countTwo +=1
        if countZero < leastNum:
            leastLayer = l
            leastNum = countZero
            oneTwo = countOne * countTwo
    print("least Zeros", leastNum)
    print("layer with least Zero", leastLayer)
    print("ones * twos", oneTwo)

    # Form images into final ascii art
    finalImage = [[[ primaryPixel(image,x,y) for x in range(width)] for y in range(height)] for l in range(numLayers)]

    # Print the image
    for y in range(height):
        print()
        for x in range(width):
            print(finalImage[l][y][x], end = "")
    print()
