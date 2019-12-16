import re
from string import Template

class Planet(object):
    def __init__(self, coordinates):
        self.posX = 0
        self.posY = 0
        self.posZ = 0

        self.velX = 0
        self.velY = 0
        self.velZ = 0

        m = re.match(r'<x=(?P<x>-*\d*), y=(?P<y>-*\d*), z=(?P<z>-*\d*)>', coordinates)

        self.posX = int(m.group('x'))
        self.posY = int(m.group('y'))
        self.posZ = int(m.group('z'))

    def __str__(self):
        return 'pos=<x={p.posX:>3}, y={p.posY:>3}, z={p.posZ:>3}>, vel=<x={p.velX:>3}, y={p.velY:>3}, z={p.velZ:>3}>'\
        .format(p=self)

    def adjust(self, planet):
        if self.posX > planet.posX:
            self.velX -=1
            planet.velX +=1
        elif self.posX < planet.posX:
            self.velX +=1
            planet.velX -=1

        if self.posY > planet.posY:
            self.velY -=1
            planet.velY +=1
        elif self.posY < planet.posY:
            self.velY +=1
            planet.velY -=1

        if self.posZ > planet.posZ:
            self.velZ -=1
            planet.velZ +=1
        elif self.posZ < planet.posZ:
            self.velZ +=1
            planet.velZ -=1

    def move(self):
        self.posX += self.velX
        self.posY += self.velY
        self.posZ += self.velZ

    def reset(self):
        self.velX = 0
        self.velY = 0
        self.velZ = 0


    def kinetic(self):
        return abs(self.velX) + abs(self.velY) + abs(self.velZ)

    def potential(self):
        return abs(self.posX) + abs(self.posY) + abs(self.posZ)

    def energy(self):
        return self.kinetic() * self.potential()

with open("input.txt","r") as f:
    # Read Input
    planets = list(map(lambda x: Planet(x.strip()), f.readlines()))
    print(0,"steps")
    for p in planets:
        print(p)
    print()
    for i in range(1000):
        planets[0].adjust(planets[1])
        planets[0].adjust(planets[2])
        planets[0].adjust(planets[3])
        planets[1].adjust(planets[2])
        planets[1].adjust(planets[3])
        planets[2].adjust(planets[3])

        print(i+1,"steps")
        for p in planets:
            p.move()

        # for p in planets:
        #     print(p)
        # print()

    print("energy of 0",planets[0].energy())
    print("energy of 1",planets[1].energy())
    print("energy of 2",planets[2].energy())
    print("energy of 3",planets[3].energy())
    print("Total energy",planets[0].energy()+planets[1].energy()+planets[2].energy()+planets[3].energy())
