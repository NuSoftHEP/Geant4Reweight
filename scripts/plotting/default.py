from ROOT import * 

def set_default_style():
  gStyle.SetOptStat(0)
  gStyle.SetPadTickX(1)
  gStyle.SetPadTickY(1)
# gStyle.SetNdivisions(505,"XY")

def set_pad_style():
  gPad.SetLeftMargin(gPad.GetLeftMargin()*1.10)
#  gPad.SetRightMargin(gPad.GetRightMargin()*1.10)
#  gPad.SetTopMargin(gPad.GetTopMargin()*1.10)
  gPad.SetBottomMargin(gPad.GetBottomMargin()*1.10)

def set_pdf_style(pdf,titleX,titleY):
#  pdf.SetTitle("")
  pdf.GetXaxis().SetNdivisions(505)
  pdf.GetYaxis().SetNdivisions(505)
  pdf.GetXaxis().SetTitleSize(.06)
  pdf.GetXaxis().SetTitleOffset(.8)
  pdf.GetYaxis().SetTitleSize(.06)
  pdf.GetYaxis().SetTitleOffset(.65)
  pdf.GetXaxis().SetTitle(titleX)
  pdf.GetYaxis().SetTitle(titleY)
#  Plot 10,11
  pdf.GetYaxis().SetLabelSize(.045)
  pdf.GetXaxis().SetLabelSize(.045)

  #Norm Params
 # pdf.GetYaxis().SetLabelSize(.06)
 # pdf.GetXaxis().SetLabelSize(.07)

  # correlation matrices
 # pdf.GetYaxis().SetLabelSize(.06)
 # pdf.GetXaxis().SetLabelSize(.06)


#  pdf.GetXaxis().SetLabelOffset(.2*pdf.GetXaxis().GetLabelOffset() )
#  pdf.GetYaxis().SetLabelOffset(.03*pdf.GetYaxis().GetLabelOffset() )

  pdf.GetYaxis().SetTitleFont(42)
  pdf.GetXaxis().SetTitleFont(42)
  pdf.GetXaxis().SetLabelFont(42)
  pdf.GetYaxis().SetLabelFont(42)
  pdf.SetTitleFont(42)



