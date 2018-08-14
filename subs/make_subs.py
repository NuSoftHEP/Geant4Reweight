energies = ["50","100","150","200",
            "250","300","400","500",
            "600","700","800"]

unit = "MeV"            

samps_to_vars = {"nom":"-i 1. -e 1.", 
                 "var_inel1.5_elast1": "-i 1.5 -e 1.",
                 "var_inel1_elast1.5": "-i 1. -e 1.5",
                 "var_inel1.5_elast1.5": "-i 1.5 -e 1.5"}
                 

for samp in ["nom","var_inel1.5_elast1","var_inel1_elast1.5","var_inel1.5_elast1.5"]:
#  for e in range(0,len(energies) ):
  for e in energies:
    infile = file("sub_base.txt","r")
    outFileName = samp[0:3] + "/" + "GeantReweight_batch_" + e + unit + "_" + samp + ".sh" 
    outfile = file(outFileName, "w")
    for line in infile:
      if "./G4Thin" in line:
        newline = "./G4Thin -c ${_CONDOR_SCRATCH_DIR}/code/macros/macros/"+e+unit+"/"+e+unit+"_${PROCESS}_"+samp[0:3] +".mac -o thin_"+samp+"_"+e+unit+"_${PROCESS}.root " + samps_to_vars[samp] 
        outfile.write(newline)
      elif "ifdh cp thin" in line:
        newline = "ifdh cp thin_"+samp+"_"+e+unit+"_${PROCESS}.root /pnfs/dune/scratch/users/calcuttj/GeantReweight/Thin/${JOBSUBJOBID:0:10}/thin_"+samp+"_"+e+unit+"_${PROCESS}.root\n" 
        outfile.write(newline)
      else:
        outfile.write(line)
    newline = "cd ${_CONDOR_SCRATCH_DIR}/code/ReweightBase-build/\n" 
    outfile.write(newline)
    if "nom" in samp: 
      newline = "./G4Reweight -f ${_CONDOR_SCRATCH_DIR}/code/G4Sim-build/thin_nom_"+e+unit+"_${PROCESS}.root -o thin_"+e+unit+"_nom_inel1.5_elast1_${PROCESS}.root -i 1.5 -e 1. \n"
      outfile.write(newline)

      newline = "ifdh cp thin_"+e+unit+"_nom_inel1.5_elast1_${PROCESS}.root /pnfs/dune/scratch/users/calcuttj/GeantReweight/Thin/${JOBSUBJOBID:0:10}/thin_"+e+unit+"_nom_inel1.5_elast1_${PROCESS}.root\n"  
      outfile.write(newline)

      newline = "./G4Reweight -f ${_CONDOR_SCRATCH_DIR}/code/G4Sim-build/thin_nom_"+e+unit+"_${PROCESS}.root -o thin_"+e+unit+"_nom_inel1_elast1.5_${PROCESS}.root -i 1. -e 1.5 \n"
      outfile.write(newline)

      newline = "ifdh cp thin_"+e+unit+"_nom_inel1_elast1.5_${PROCESS}.root /pnfs/dune/scratch/users/calcuttj/GeantReweight/Thin/${JOBSUBJOBID:0:10}/thin_"+e+unit+"_nom_inel1_elast1.5_${PROCESS}.root\n"  
      outfile.write(newline)

      newline = "./G4Reweight -f ${_CONDOR_SCRATCH_DIR}/code/G4Sim-build/thin_nom_"+e+unit+"_${PROCESS}.root -o thin_"+e+unit+"_nom_inel1.5_elast1.5_${PROCESS}.root -i 1.5 -e 1.5 \n"
      outfile.write(newline)

      newline = "ifdh cp thin_"+e+unit+"_nom_inel1.5_elast1.5_${PROCESS}.root /pnfs/dune/scratch/users/calcuttj/GeantReweight/Thin/${JOBSUBJOBID:0:10}/thin_"+e+unit+"_nom_inel1.5_elast1.5_${PROCESS}.root\n"  
      outfile.write(newline)
    else:
      newline = "./G4Reweight -f ${_CONDOR_SCRATCH_DIR}/code/G4Sim-build/thin_"+samp+"_"+e+unit+"_${PROCESS}.root -o thin_"+e+unit+"_"+samp+"_${PROCESS}.root -i 1. -e 1. \n"
      outfile.write(newline)

      newline = "ifdh cp thin_"+e+unit+"_"+samp+"_${PROCESS}.root /pnfs/dune/scratch/users/calcuttj/GeantReweight/Thin/${JOBSUBJOBID:0:10}/thin_"+e+unit+"_"+samp+"_${PROCESS}.root\n"
      outfile.write(newline)
