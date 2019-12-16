import re
import numpy
from string import Template

class Universe(object):
    def __init__(self, planets):
        self.planets = tuple(map(lambda x: x.copy(), planets))

    def energy(self):
        total = 0
        for p in self.planets:
            total += p.energy()
        return total

    def __str__(self):
        result = ""
        for p in self.planets:
            result += p.__str__()+"\n"
        return result

    def __eq__(self, other):
        for i in range(len(self.planets)):
            if self.planets[0] != other.planets[0]:
                return False
        return True

    def __ne__(self, other):
        return not self == other

    def __hash__(self):
        return hash(self.planets)

class UniverseX(Universe):
    def __eq__(self, other):
        for i in range(len(self.planets)):
            if self.planets[i].posX != other.planets[i].posX:
                return False
            if self.planets[i].velX != other.planets[i].velX:
                return False
        return True

    def __ne__(self, other):
        return not self == other

    def __hash__(self):
        return hash(tuple(map(lambda x: x.posX+x.velX, self.planets)))

class UniverseY(Universe):
    def __eq__(self, other):
        for i in range(len(self.planets)):
            if self.planets[i].posY != other.planets[i].posY:
                return False
            if self.planets[i].velY != other.planets[i].velY:
                return False
        return True

    def __ne__(self, other):
        return not self == other

    def __hash__(self):
        return hash(tuple(map(lambda x: x.posY+x.velY, self.planets)))

class UniverseZ(Universe):
    def __eq__(self, other):
        for i in range(len(self.planets)):
            if self.planets[i].posZ != other.planets[i].posZ:
                return False
            if self.planets[i].velZ != other.planets[i].velZ:
                return False
        return True

    def __ne__(self, other):
        return not self == other

    def __hash__(self):
        return hash(tuple(map(lambda x: x.posZ+x.velZ, self.planets)))


class Planet(object):
    def __init__(self, coordinates = None):
        self.posX = 0
        self.posY = 0
        self.posZ = 0

        self.velX = 0
        self.velY = 0
        self.velZ = 0

        if coordinates is not None:
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

    def __eq__(self, other):
        if isinstance(other, Planet):
            return \
                self.posX == other.posX and \
                self.velX == other.velX and \
                self.posY == other.posY and \
                self.velY == other.velY and \
                self.posZ == other.posZ and \
                self.velZ == other.velZ
        return False

    def __ne__(self, other):
        return not self == other

    def __hash__(self):
        return (self.posX<<1) + (self.posY<<2) + (self.posZ<<3) + (self.velX<<4) + (self.velY<<5) + (self.velZ<<6)

    def copy(self):
        new = Planet()
        new.posX = self.posX
        new.posY = self.posY
        new.posZ = self.posZ

        new.velX = self.velX
        new.velY = self.velY
        new.velZ = self.velZ

        return new



with open("input.txt","r") as f:
    # Read Input
    planets = list(map(lambda x: Planet(x.strip()), f.readlines()))
    for p in planets:
        print(p)
    print()

    initialX = UniverseX(planets)
    initialY = UniverseY(planets)
    initialZ = UniverseZ(planets)

    i = 0
    countX = None
    countY = None
    countZ = None
    found = 0
    while True:
        i +=1
        planets[0].adjust(planets[1])
        planets[0].adjust(planets[2])
        planets[0].adjust(planets[3])
        planets[1].adjust(planets[2])
        planets[1].adjust(planets[3])
        planets[2].adjust(planets[3])

        for p in planets:
            p.move()

        if countX is None:
            u = UniverseX(planets)
            if u == initialX:
                print(i, "steps for X")
                found += 1
                countX = i

        if countY is None:
            u = UniverseY(planets)
            if u == initialY:
                print(i, "steps for Y")
                found += 1
                countY = i


        if countZ is None:
            u = UniverseZ(planets)
            if u == initialZ:
                print(i, "steps for Z")
                found += 1
                countZ = i


        if i % 10000 == 0:
            print(i, "steps:")

        if found == 3:
            total = numpy.lcm.reduce([countX, countY, countZ])
            print(countX, "count X")
            print(countY, "count Y")
            print(countZ, "count Z")
            print(total, "total")

            # print("universe\n",u)
            # for j in range(len(initial.planets)):
            #     print(initial.planets[j],u.planets[j],initial.planets[j] == u.planets[j])
            quit()

            # print(positions)

        # for p in planets:
        #     print(p)
        # print()

    print("energy of 0",planets[0].energy())
    print("energy of 1",planets[1].energy())
    print("energy of 2",planets[2].energy())
    print("energy of 3",planets[3].energy())
    print("Total energy",planets[0].energy()+planets[1].energy()+planets[2].energy()+planets[3].energy())
