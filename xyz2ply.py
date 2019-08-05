from plyfile import PlyData, PlyElement
import sys
import numpy


vertex = []
with open(sys.argv[1]) as f:
    for line in f:
        if line[0] != '#':
            ls = line.strip().split()
            vertex.append((float(ls[0]), float(ls[1]), float(ls[2]), int(ls[3]), int(ls[4]), int(ls[5])))

vertex = numpy.array(vertex, dtype=[('x', 'f4'), ('y', 'f4'), ('z', 'f4'), ('red', 'u1'), ('green', 'u1'), ('blue', 'u1')])
el = PlyElement.describe(vertex, 'vertex')
PlyData([el]).write(sys.argv[2])

