#!/usr/bin/env python

import sys
import os.path
import os  
os.system('cls') 
# This is a tiny script to help you creating a CSV file from a face
# database with a similar hierarchie:
#
#  philipp@mango:~/facerec/data/at$ tree
#  .
#  |-- README
#  |-- s1
#  |   |-- 1.pgm
#  |   |-- ...
#  |   |-- 10.pgm
#  |-- s2
#  |   |-- 1.pgm
#  |   |-- ...
#  |   |-- 10.pgm
#  ...
#  |-- s40
#  |   |-- 1.pgm
#  |   |-- ...
#  |   |-- 10.pgm
#

if __name__ == "__main__":

    #if len(sys.argv) != 2:
    #    print "usage: create_csv <base_path>"
    #    sys.exit(1)

    #BASE_PATH=sys.argv[1]
    BASE_PATH="D:\Software\Visual Studio 2013\Projects\OpenCV\DeepGlintProjectTest\einfacedata"

    SEPARATOR=";"

    fh = open("../at.txt",'w')

    label = 2
    for dirname, dirnames, filenames in os.walk(BASE_PATH):
        for subdirname in dirnames:
            subject_path = os.path.join(dirname, subdirname)
            for filename in os.listdir(subject_path):
                abs_path = "%s/%s" % (subject_path, filename)
                print ("%s%s%d" % (abs_path, SEPARATOR, label));
                fh.write(abs_path)
                fh.write(SEPARATOR)
                fh.write(str(label))
                fh.write("\n")      
            label = label + 1
    fh.close()