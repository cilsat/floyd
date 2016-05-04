#!/usr/bin/python
import numpy as np
import sys

def main(argv):
    b = []
    for n in range(10):
        f = open(argv[1]+str(n)).read().split('\n')[:-1]
        a = np.array([float(r) for l in f for r in l.split(',')])
        a = a.reshape(a.shape[0]/3, 3)
        b.append(a)
    out = np.mean(np.dstack(b), axis=-1)
    out.tofile(argv[1]+'.txt')

if __name__ == "__main__":
    main(sys.argv)
