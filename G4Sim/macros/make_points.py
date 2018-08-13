energies = ["50","100","150","200",
            "250","300","400","500",
            "600","700","800"]

unit = "MeV"            

infile = file("mac_base.txt","r")

nomvar = {"nom":[770353837,70597151],"var":[575841909,838392975]}

for samp in ["nom","var"]:
  for e in range(0,len(energies) ):
    for i in range(0, 5):
      infile = file("mac_base.txt","r")
      outFileName = str(energies[e]) + unit + "/" + str(energies[e]) + unit + "_" + str(i) + "_" +samp + ".mac"
      outfile = file(outFileName, "w")
      print e*5 + i  
      print i 
      for line in infile:   
        if "energy" in line: 
          #print "/gps/ene/mono   ", energies[e], unit 
          outfile.write("/gps/ene/mono   " + str(energies[e]) +" " +  unit)
        elif "seed" in line:
          #print "/random/setSeeds", str(770353837 + e*5 + i), str(70597151 + e*5 + i)
          outfile.write("/random/setSeeds " + str(nomvar[samp][0] + e*5 + i) + " " +  str(nomvar[samp][1] + e*5 + i))
        else:
          #print line
          outfile.write(line)

