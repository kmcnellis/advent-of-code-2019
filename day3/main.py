import math

class Point:
    def __init__(self,x,y,dist=0):
        self.x = x
        self.y = y
        self.dist = dist
    def __eq__(self,other):
        return self.x == other.x and self.y == other.y
    def __ne__(self,other):
        return not self == other
    def __hash__(self):
        return (self.x,self.y).__hash__()
    def __str__(self):
        return "("+str(self.x)+","+str(self.y)+")"



def findCoord(line):
    distance = 0
    x=0
    y=0
    coord = {Point(x=0,y=0)}
    for move in line:
        dir = move[0]
        amount = int(move[1:])
        for i in range(0,amount):
            # print(dir,currPos)
            if dir == "R":
                x += 1
            elif dir == "L":
                x -= 1
            elif dir == "U":
                y += 1
            elif dir == "D":
                y -= 1
            distance += 1
            point = Point(x=x,y=y,dist=distance)
            if not point in coord:
                coord.add(point)
    return coord

def printCoord(coords1,coords2):
    # print(coords1)
    # print(coords2)
    for y in range(10,-10, -1):
        for x in range(-10,10):
            if (x,y)  == (0,0):
                print("O", end = "")
            elif Point(x=x,y=y) in coords1 and Point(x=x,y=y) in coords2:
                print("X", end = "")
            elif Point(x=x,y=y) in coords1:
                print("a", end = "")
            elif Point(x=x,y=y) in coords2:
                print("b", end = "")
            elif x  == 0:
                print("|", end = "")
            elif y  == 0:
                print("-", end = "")
            else:
                print(".", end = "")
        print()

with open("input.txt","r") as f:
    line1 =f.readline().strip().split(',')
    line2 =f.readline().strip().split(',')
    # print(line1)
    coords1 = findCoord(line1)
    # print(line2)
    coords2 = findCoord(line2)
    printCoord(coords1,coords2)

    intersect = coords1.intersection(coords2)
    intersect.remove(Point(x=0,y=0))

    coords1 = list(coords1)
    coords2 = list(coords2)


    minDist = math.inf
    for point in intersect:
        line1 = coords1[coords1.index(point)]
        line2 = coords2[coords2.index(point)]
        dist = line1.dist + line2.dist
        print(line1, line1.dist)
        print(line2, line2.dist)
        if dist < minDist:
            minDist = dist
    print("minDist", minDist)
