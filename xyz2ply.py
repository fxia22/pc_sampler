from plyfile import PlyData, PlyElement
import sys
import numpy as np
import argparse

parser = argparse.ArgumentParser(description='point cloud cutting')
parser.add_argument('--infile', help='input file', required=True)
parser.add_argument('--outfile', help='output file', required=True)
parser.add_argument('--bbmin', nargs=3, type=float, help='bounding box min', default=[-np.inf, -np.inf, -np.inf])
parser.add_argument('--bbmax', nargs=3, type=float, help='bounding box max', default=[np.inf, np.inf, np.inf])

args = parser.parse_args()
minx, miny, minz, maxx, maxy, maxz = -args.bbmin[0], args.bbmin[1], args.bbmin[2], args.bbmax[0], args.bbmax[1], args.bbmax[2]
print(args)

vertex = []
with open(args.infile) as f:
    for line in f:
        if line[0] != '#':
            ls = line.strip().split()
            x,y,z = float(ls[0]), float(ls[1]), float(ls[2])
            if (x >= minx) and (x <= maxx) and (y >= miny) and (y <=maxy) and (z >=minz) and (z <= maxz):
                vertex.append((x, y, z, int(ls[3]), int(ls[4]), int(ls[5])))

vertex = np.array(vertex, dtype=[('x', 'f4'), ('y', 'f4'), ('z', 'f4'), ('red', 'u1'), ('green', 'u1'), ('blue', 'u1')])
el = PlyElement.describe(vertex, 'vertex')
PlyData([el]).write(args.outfile)

