#!/usr/bin/python

############################################################
#  default
############################################################
import re

############################################################
#  argument
############################################################
ifile = "test.txt"
lst = [ "pci-eth" , "pci-ib" , "sata-eth" , "sata-ib" , "ssd-eth" , "ssd-ib" ]
cname = "_terasort_01_400G_50_50" 
path = "129.60.81.64_50030/"

############################################################
#  myclass
############################################################
class myclass:
  def __init__(self, perm):
    self.htmllist = perm

  def readspillpage(self, *perm):
    i = 0
    while i < 6:
      sum_sp = 0
      fl = perm[0][i] + cname # make pattern
      opnfile = perm[0][i] + ".txt" # read file
      print " **** " + opnfile + " **** "
      fh = open(opnfile, "r")
      for line in fh:
        line = line.rstrip()
        opnfile2 = fl + "/" + perm[2] + line # read WebUI
        fh2 = open(opnfile2, "r")
        flg = 0
        for lin in fh2:
          lin = lin.rstrip()
#          if lin.find("Reduce shuffle bytes") >= 0:
          if lin.find("Spilled Records") >= 0:
            flg = 2
          if flg > 0:
            flg = flg - 1
            if flg == 0:
              query = re.search(r'[\d]+,.*[\d,]+', lin)
              spill = query.group()
              spill = spill.replace(",", "")
              sum_sp = sum_sp + int( spill )
        fh2.close()
      fh.close()
      i = i + 1
      print "Spilled Redords: " + str( sum_sp )
      print "Average Spilled Redords: " + str( sum_sp/50 )

############################################################
#  main
############################################################
my = myclass(ifile)
my.readspillpage(lst, cname, path)
