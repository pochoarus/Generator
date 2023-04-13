# Instructions to install GENIE and run NNSFnu

Tested in CentOS Linux release 7.9.2009 (Core) with access to sft.cern.ch (CVFMS).

# Installation
```
export GENIE=/genie/will/be/installed/in/this/path

git clone -b NNSFnu https://github.com/pochoarus/Generator.git $GENIE

source /cvmfs/sft.cern.ch/lcg/releases/gcc/7.3.0-cb1ee/x86_64-centos7/setup.sh
source /cvmfs/sft.cern.ch/lcg/releases/ROOT/6.12.04-abd9a/x86_64-centos7-gcc7-opt/bin/thisroot.sh
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/pythia6/429.2-63d8b/x86_64-centos7-gcc7-opt/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/lhapdf/6.2.1-4b9c6/x86_64-centos7-gcc7-opt/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/log4cpp/2.8.3-aeffd/x86_64-centos7-gcc7-opt/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/tbb/2018_U1-d3621/x86_64-centos7-gcc7-opt/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/GSL/2.1-36ee5/x86_64-centos7-gcc7-opt/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/libxml2/2.9.7-830a9/x86_64-centos7-gcc7-opt/lib:$LD_LIBRARY_PATH

cd $GENIE
./configure --disable-lhapdf5 --enable-lhapdf6 --disable-apfel --with-lhapdf6-inc=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/lhapdf/6.2.1-4b9c6/x86_64-centos7-gcc7-opt/include --with-lhapdf6-lib=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/lhapdf/6.2.1-4b9c6/x86_64-centos7-gcc7-opt/lib --with-pythia6-lib=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/pythia6/429.2-63d8b/x86_64-centos7-gcc7-opt/lib --with-libxml2-inc=/cvmfs/sft.cern.ch/lcg/releases/libxml2/2.9.7-830a9/x86_64-centos7-gcc7-opt/include/libxml2 --with-libxml2-lib=/cvmfs/sft.cern.ch/lcg/releases/libxml2/2.9.7-830a9/x86_64-centos7-gcc7-opt/lib --with-log4cpp-inc=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/log4cpp/2.8.3-aeffd/x86_64-centos7-gcc7-opt/include --with-log4cpp-lib=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/log4cpp/2.8.3-aeffd/x86_64-centos7-gcc7-opt/lib
sed -i 's/lPythia6/lpythia6/g' src/make/Make.include 
make

# Creating enviroment script
cat > $GENIE/setup.sh << EOL
source /cvmfs/sft.cern.ch/lcg/releases/gcc/7.3.0-cb1ee/x86_64-centos7/setup.sh
source /cvmfs/sft.cern.ch/lcg/releases/ROOT/6.12.04-abd9a/x86_64-centos7-gcc7-opt/bin/thisroot.sh
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/pythia6/429.2-63d8b/x86_64-centos7-gcc7-opt/lib:\$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/lhapdf/6.2.1-4b9c6/x86_64-centos7-gcc7-opt/lib:\$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/log4cpp/2.8.3-aeffd/x86_64-centos7-gcc7-opt/lib:\$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/tbb/2018_U1-d3621/x86_64-centos7-gcc7-opt/lib:\$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/GSL/2.1-36ee5/x86_64-centos7-gcc7-opt/lib:\$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/libxml2/2.9.7-830a9/x86_64-centos7-gcc7-opt/lib:\$LD_LIBRARY_PATH
export GENIE=$GENIE
export LD_LIBRARY_PATH=\$GENIE/lib:\$LD_LIBRARY_PATH
export PATH=\$GENIE/bin:\$PATH
export LHAPATH=/cvmfs/sft.cern.ch/lcg/releases/MCGenerators/lhapdf/6.2.1-4b9c6/x86_64-centos7-gcc7-opt/share/LHAPDF/:\$GENIE/data/evgen/pdfs/
export HEDIS_SF_DATA_PATH=\$GENIE/data/evgen/hedis-sf/
EOL

```

# Download NNSFnu grids (in LHADPF6 format) and create tune
```
cd /path/to/genie/install/dir

source setup.sh

#download nnsfnu grid files (in this example we use Oxygen)
cd $GENIE/data/evgen/pdfs/
wget https://data.nnpdf.science/NNSFnu/NNSFnu_O_lowQ.tar.gz
wget https://data.nnpdf.science/NNSFnu/NNSFnu_O_highQ.tar.gz
tar xvfz NNSFnu_O_lowQ.tar.gz
tar xvfz NNSFnu_O_highQ.tar.gz
rm NNSFnu_O_lowQ.tar.gz
rm NNSFnu_O_highQ.tar.gz

# create tune for this specific target (in this example we name it '03' but you can use any number between 00 and 99) 
cd $GENIE
cp -r config/GNNSFnu22_XXa config/GNNSFnu22_03a
cp -r config/GNNSFnu22_XXb config/GNNSFnu22_03b
cp -r config/GNNSFnu22_XXc config/GNNSFnu22_03c
sed -i 's/NNSFvPDFGridName/NNSFnu_O_lowQ/g' config/GNNSFnu22_03a/CommonParam.xml
sed -i 's/NNSFvPDFGridName/NNSFnu_O_highQ/g' config/GNNSFnu22_03b/CommonParam.xml
sed -i 's/NNSFvPDFGridName/NNSFnu_O_highQ/g' config/GNNSFnu22_03c/CommonParam.xml
```

# Generate HEDIS SF grids and compute total cross section splines
```
cd /path/to/genie/install/dir

source setup.sh

# in this example we are using member 0 of the set. To use other replicas change the member number.
MEMBER=000

# this will store the HEDIS SF grids in HEDIS_SF_DATA_PATH
gmkhedissf --tune GNNSFnu22_03a_00_${MEMBER}
gmkhedissf --tune GNNSFnu22_03b_00_${MEMBER}
gmkhedissf --tune GNNSFnu22_03c_00_${MEMBER}

#each tune will take less than an hour and it computes for 201 knots from 100GeV to 10EeV
gmkspl -p 14 -t 1000010010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03a_00_${MEMBER} -o GNNSFnu22_03a_${MEMBER}_numu_p.xml
gmkspl -p 14 -t 1000000010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03a_00_${MEMBER} -o GNNSFnu22_03a_${MEMBER}_numu_n.xml
gmkspl -p 14 -t 1000010010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03b_00_${MEMBER} -o GNNSFnu22_03b_${MEMBER}_numu_p.xml
gmkspl -p 14 -t 1000000010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03b_00_${MEMBER} -o GNNSFnu22_03b_${MEMBER}_numu_n.xml
gmkspl -p 14 -t 1000010010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03c_00_${MEMBER} -o GNNSFnu22_03c_${MEMBER}_numu_p.xml
gmkspl -p 14 -t 1000000010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03c_00_${MEMBER} -o GNNSFnu22_03c_${MEMBER}_numu_n.xml
gmkspl -p -14 -t 1000010010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03a_00_${MEMBER} -o GNNSFnu22_03a_${MEMBER}_numub_p.xml
gmkspl -p -14 -t 1000000010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03a_00_${MEMBER} -o GNNSFnu22_03a_${MEMBER}_numub_n.xml
gmkspl -p -14 -t 1000010010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03b_00_${MEMBER} -o GNNSFnu22_03b_${MEMBER}_numub_p.xml
gmkspl -p -14 -t 1000000010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03b_00_${MEMBER} -o GNNSFnu22_03b_${MEMBER}_numub_n.xml
gmkspl -p -14 -t 1000010010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03c_00_${MEMBER} -o GNNSFnu22_03c_${MEMBER}_numub_p.xml
gmkspl -p -14 -t 1000000010 -n 201 --event-generator-list CCHEDIS --tune GNNSFnu22_03c_00_${MEMBER} -o GNNSFnu22_03c_${MEMBER}_numub_n.xml

#merging splines
gspladd -f GNNSFnu22_03a_${MEMBER}_numu_p.xml,GNNSFnu22_03a_${MEMBER}_numub_p.xml,GNNSFnu22_03a_${MEMBER}_numu_n.xml,GNNSFnu22_03a_${MEMBER}_numub_n.xml -o GNNSFnu22_03a_${MEMBER}.xml
gspladd -f GNNSFnu22_03b_${MEMBER}_numu_p.xml,GNNSFnu22_03b_${MEMBER}_numub_p.xml,GNNSFnu22_03b_${MEMBER}_numu_n.xml,GNNSFnu22_03b_${MEMBER}_numub_n.xml -o GNNSFnu22_03b_${MEMBER}.xml
gspladd -f GNNSFnu22_03c_${MEMBER}_numu_p.xml,GNNSFnu22_03c_${MEMBER}_numub_p.xml,GNNSFnu22_03c_${MEMBER}_numu_n.xml,GNNSFnu22_03c_${MEMBER}_numub_n.xml -o GNNSFnu22_03c_${MEMBER}.xml

#creating root files
gspl2root -l -e 10,1e10 -p 14,-14 -t 1000010010,1000000010 -f GNNSFnu22_03a_${MEMBER}.xml --tune GNNSFnu22_03a_00_${MEMBER} --event-generator-list CCHEDIS -o GNNSFnu22_03a_${MEMBER}.root
gspl2root -l -e 10,1e10 -p 14,-14 -t 1000010010,1000000010 -f GNNSFnu22_03b_${MEMBER}.xml --tune GNNSFnu22_03b_00_${MEMBER} --event-generator-list CCHEDIS -o GNNSFnu22_03b_${MEMBER}.root
gspl2root -l -e 10,1e10 -p 14,-14 -t 1000010010,1000000010 -f GNNSFnu22_03c_${MEMBER}.xml --tune GNNSFnu22_03c_00_${MEMBER} --event-generator-list CCHEDIS -o GNNSFnu22_03c_${MEMBER}.root
```

# Script to plot precomputed cross section from one replica
```
import ROOT
import numpy as np
import matplotlib.pyplot as plt

def scale_sf(file,nu,Z,A) :
    print('SF',file.GetName())
    gr_p = file.Get(nu+"_H1/dis_cc")
    gr_n = file.Get(nu+"_n/dis_cc")
    gr = ROOT.TGraph()
    for i in range(gr_p.GetN()) :
        ene  = gr_p.GetX()[i]
        xsec = (Z*gr_p.Eval(ene) + (A-Z)*gr_n.Eval(ene))/A
        gr.SetPoint(gr.GetN(),ene,xsec/ene)
    return gr

def merge(gr1,gr2,gr3) :
    print('Merge')
    gr = ROOT.TGraph()
    for i in range(gr1.GetN()) :
        ene  = gr1.GetX()[i]
        xsec = gr1.GetY()[i]+gr2.GetY()[i]+gr3.GetY()[i]
        gr.SetPoint(gr.GetN(),ene,xsec)
    return gr


idx    = '03'
Z      = 8
A      = 16
member = '000'
nu    = 'nu_mu'

file_sf      = ROOT.TFile("./GNNSFnu22_"+idx+"a_"+member+".root")
file_yd_low  = ROOT.TFile("./GNNSFnu22_"+idx+"b_"+member+".root")
file_yd_high = ROOT.TFile("./GNNSFnu22_"+idx+"c_"+member+".root")

gr_sf      = scale_sf(file_sf,nu,Z,A)    
gr_yd_low  = scale_sf(file_yd_low,nu,Z,A)
gr_yd_high = scale_sf(file_yd_high,nu,Z,A)

gr_me      = merge(gr_sf,gr_yd_low,gr_yd_high)

fig = plt.figure(figsize=(7,5))
ax1 = plt.subplot()
ax1.plot(gr_me.GetX(), gr_me.GetY())
ax1.set_ylabel('$\sigma/E~{\rm [10^{-38}cm^{2}/GeV/nucleon]}$')
ax1.set_xlabel('$E~{\rm [GeV]}$')
ax1.set_xscale('log')
ax1.set_yscale('linear')
plt.savefig('./xsec.pdf')
plt.close()
```


# Script to plot precomputed cross section with error bands from replicas
```
import ROOT
import numpy as np
import matplotlib.pyplot as plt

def getstat(elist,xs) :
    high = np.nanpercentile(xs,84,axis=0)
    low  = np.nanpercentile(xs,16,axis=0)
    mn   = ( high + low )/2.
    std  = ( high - low )/2.
    gr_mn,gr_lw,gr_up = ROOT.TGraph(),ROOT.TGraph(),ROOT.TGraph()
    for i,ene in enumerate(elist) :
        gr_mn.SetPoint(gr_mn.GetN(),ene,mn[i]/ene)
        gr_lw.SetPoint(gr_lw.GetN(),ene,(mn[i]-std[i])/ene)
        gr_up.SetPoint(gr_up.GetN(),ene,(mn[i]+std[i])/ene)
    return [gr_mn,gr_lw,gr_up]

def scale_set(idx,Z,A) :
    print('SET',idx[0],idx[1])

    SETS = [ str(s).zfill(3) for s in range(1,idx[1]+1) ]

    file = ROOT.TFile("./GNNSFnu22_"+idx[0]+"_00_000.root")
    elist = [ file.Get("nu_mu_H1/dis_cc").GetX()[i] for i in range(file.Get("nu_mu_H1/dis_cc").GetN()) ]

    xs_nu  = np.zeros((len(SETS),len(elist)))
    xs_nub = np.zeros((len(SETS),len(elist)))
    for s,setid in enumerate(SETS) :
        file = ROOT.TFile("./GNNSFnu22_"+idx[0]+"_00_"+setid+".root")
        gr_nu_p  = file.Get("nu_mu_H1/dis_cc")
        gr_nu_n  = file.Get("nu_mu_n/dis_cc")
        gr_nub_p = file.Get("nu_mu_bar_H1/dis_cc")
        gr_nub_n = file.Get("nu_mu_bar_n/dis_cc")
        for i in range(gr_nu_p.GetN()) : 
            xs_nu[s,i]  = (Z*gr_nu_p.GetY()[i] + (A-Z)*gr_nu_n.GetY()[i])/A
            xs_nub[s,i] = (Z*gr_nub_p.GetY()[i] + (A-Z)*gr_nub_n.GetY()[i])/A

    return getstat(elist,xs_nu),getstat(elist,xs_nub)


def merge_set(gr1,gr2,gr3) :
    print('Merge Errors')

    gr_mn,gr_lw,gr_up = ROOT.TGraph(),ROOT.TGraph(),ROOT.TGraph()

    for i in range(gr1[0].GetN()) :
        ene  = gr1[0].GetX()[i]
        mn   = gr1[0].GetY()[i]+gr2[0].GetY()[i]+gr3[0].GetY()[i]
        std1 = gr1[2].GetY()[i]-gr1[0].GetY()[i]
        std2 = gr2[2].GetY()[i]-gr2[0].GetY()[i]
        std3 = gr3[2].GetY()[i]-gr3[0].GetY()[i]
        std  = np.sqrt(np.power(std1,2)+np.power(std2,2)+np.power(std3,2))
        gr_mn.SetPoint(gr_mn.GetN(),ene,mn)
        gr_lw.SetPoint(gr_lw.GetN(),ene,mn-std)
        gr_up.SetPoint(gr_up.GetN(),ene,mn+std)
    return [gr_mn,gr_lw,gr_up]

idx = '03'
Z   = 8
A   = 16

gr_nu_sf_rep,gr_nub_sf_rep           = scale_set([idx+'a',200],Z,A)
gr_nu_yd_low_rep,gr_nub_yd_low_rep   = scale_set([idx+'b',200],Z,A)
gr_nu_yd_high_rep,gr_nub_yd_high_rep = scale_set([idx+'c',200],Z,A)

gr_nu_me_rep  = merge_set(gr_nu_sf_rep,gr_nu_yd_low_rep,gr_nu_yd_high_rep)
gr_nub_me_rep = merge_set(gr_nub_sf_rep,gr_nub_yd_low_rep,gr_nub_yd_high_rep)

fig = plt.figure(figsize=(7,5))
ax1 = plt.subplot()
ax1.plot(gr_nu_me_rep[0].GetX(),  gr_nu_me_rep[0].GetY(), color='blue')
ax1.fill_between(gr_nu_me_rep[0].GetX(), gr_nu_me_rep[1].GetY(), gr_nu_me_rep[2].GetY(), alpha=0.3, color='blue')
ax1.plot(gr_nub_me_rep[0].GetX(),  gr_nub_me_rep[0].GetY(), color='red')
ax1.fill_between(gr_nub_me_rep[0].GetX(), gr_nub_me_rep[1].GetY(), gr_nub_me_rep[2].GetY(), alpha=0.3, color='red')
ax1.set_ylabel(r'$\sigma/E~{\rm [10^{-38}cm^{2}/GeV/nucleon]}$')
ax1.set_xlabel(r'$E~{\rm [GeV]}$')
ax1.set_xscale('log')
ax1.set_yscale('linear')
plt.savefig('./xsec.pdf')
plt.close()
```



