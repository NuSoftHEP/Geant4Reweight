from ROOT import *

def getChi2(w, v):
  result = []  
  chi2 = 0.
  nbins = 0
  for i in range(1,w.GetNbinsX()+1):
    if(  (w.GetBinError(i) == 0) and (v.GetBinError(i) == 0) ) :
      continue
    
    a = ( (w.GetBinContent(i) - v.GetBinContent(i))**2 ) / ( w.GetBinError(i)**2 + v.GetBinError(i)**2  ) 
    chi2 = chi2 + a 
    nbins = nbins + 1
  result.append(chi2)
  print nbins
  result.append(nbins)
  return result

def getMax(n, w, v):
  nMax = n.GetMaximum()
  wMax = w.GetMaximum()
  vMax = v.GetMaximum()
  if (nMax >= wMax and nMax >= vMax):
    return [ [n,"hist P"], [w,"same hist e1"], [v,"same hist e1"] ]
  elif (wMax >= nMax and wMax >= vMax):
    return [ [w,"hist e1"], [n,"hist same P"], [v,"same hist e1"] ]
  elif (vMax >= nMax and vMax >= wMax):
    return [ [v,"hist e1"], [n,"hist same P"], [w,"same hist e1"] ]
  elif (vMax == nMax and nMax == wMax):
    return [ [n,"hist P"], [w,"same hist e1"], [v,"same hist e1"] ]


def getXMax(n, w, v):
  nMax = float(n.GetNbinsX() * n.GetBinWidth(1))
  wMax = float(w.GetNbinsX() * w.GetBinWidth(1))
  vMax = float(v.GetNbinsX() * v.GetBinWidth(1))

  print nMax, wMax, vMax
  if (nMax >= wMax and nMax >= vMax):
    return float(nMax)
  elif (wMax >= nMax and wMax >= vMax):
    return float(wMax)
  elif (vMax >= nMax and vMax >= wMax):
    return float(vMax)
  elif (vMax == nMax and nMax == wMax):
    return float(nMax)

gStyle.SetOptStat(0)


def drawStyle(n,w,v):
  w.SetLineColor(2)
  w.SetMarkerColor(2)
  w.SetMarkerStyle(20)
  w.SetMarkerSize(.75)
  v.SetLineColor(1)
  n.SetMarkerColor(4)
  n.SetMarkerStyle(25)
