import platform

from PyQt4.QtCore import *
from PyQt4.QtGui import *

StyleSheet = '''
    QLineEdit[valid="false"] {background-color: rgb(255, 80, 80);}
'''

#TODO: add formatting to general, field, run and time options
#TODO: split up classes into separate files
#TODO: set tool tips

class ParamsForm(QDialog):
    def __init__(self, params=None, parent=None):
        super(ParamsForm, self).__init__(parent)
        
        self.params = {"ndim":4,"dims":[1,1,1,1],"run-type":1,"input_file":"input.dat","output_file":"output.dat"}
         
        buttonBox = QDialogButtonBox(QDialogButtonBox.Ok| QDialogButtonBox.Cancel)
        self.connect(buttonBox, SIGNAL("accepted()"),self, SLOT("accept()"))
        self.connect(buttonBox, SIGNAL("rejected()"),self, SLOT("reject()"))
                
        tabWidget = QTabWidget()
        self.generalWidget = GeneralParamsWidget()
        tabWidget.addTab(self.generalWidget, "&General")
        self.timeWidget = TimeParamsWidget()
        tabWidget.addTab(self.timeWidget, "&Time")
        self.wfWidget = WfParamsWidget()
        tabWidget.addTab(self.wfWidget, "&Wavefunction")
        self.fieldWidget = FieldParamsWidget()
        tabWidget.addTab(self.fieldWidget, "&Fields")
        self.runWidget = RunParamsWidget()
        tabWidget.addTab(self.runWidget, "&Runtime")
        
        layout = QVBoxLayout()
        layout.addWidget(tabWidget)
        layout.addWidget(buttonBox)
        self.setLayout(layout)
    
    def runParamsDict(self):
        return self.runparams        

    def paramsDict(self):
        return self.params
    
    def accept(self):
        class DimsError(Exception): pass
        #class FileError(Exception): pass
        
        try:
            self.params["ndim"] = self.generalWidget.ndimSpinbox.value()
            self.params["dims"] = []
            for i in unicode(self.generalWidget.dimsLineEdit.text()).split(","):
                self.params["dims"].append(int(i))
            #i build a list of ints and then replace it with a text string because I want to test the values 
            #being sent to the c++ program, I may comment out the above line depending on how I interface to
            #the c++ routines same goes for the fields below
            self.params["run-type"] = self.generalWidget.distBox.currentIndex()+1
            self.params["input_file"] = self.generalWidget.inFileLineEdit.text()
            self.params["output_file"] = self.generalWidget.outFileLineEdit.text()
            
            self.params["tinitial"] = self.timeWidget.tiSpinBox.value()
            self.params["tfinal"] = self.timeWidget.tfSpinBox.value()
            self.params["dt"] = float(self.timeWidget.dtLineEdit.text())
            
            self.params["wf-type"] = self.wfWidget.wfTypeBox.currentIndex()+1
            self.params["pot-type"] = self.wfWidget.potTypeBox.currentIndex()+1
            self.params["smoothing"] = float(self.wfWidget.smoothingLineEdit.text())
            self.params["rnuc"] = self.wfWidget.rnucDoubleSpinBox.value()
            self.params["theta-nuc"] = self.wfWidget.thetaDoubleSpinBox.value()
            self.params["ip"] = self.wfWidget.ipDoubleSpinBox.value()
            if self.params["pot-type"] == 2:
                self.params["charges"] = self.wfWidget.chargesLineEdit.text()
            else:
                self.params["charges"] = self.wfWidget.chargeLineEdit.text()

            self.params["nfield"] = self.fieldWidget.nfieldSpinBox.value()
            self.params["env"] = self.fieldWidget.envComboBox.currentIndex()+1
            self.params["omega"] = []
            for i in unicode(self.fieldWidget.omegaLineEdit.text()).split(","):
                self.params["omega"].append(float(i))
            self.params["ef"] = []
            for i in unicode(self.fieldWidget.efLineEdit.text()).split(","):
                self.params["ef"].append(float(i))
            self.params["ce"] = []
            for i in unicode(self.fieldWidget.ceLineEdit.text()).split(","):
                self.params["ce"].append(float(i))
            self.params["fwhm"] = []
            for i in unicode(self.fieldWidget.fwhmLineEdit.text()).split(","):
                self.params["fwhm"].append(float(i))
            
            self.params["nthreads"] = self.runWidget.nthreadSpinBox.value()
            
            self.runparams={}            
            if self.runWidget.runLocationComboBox.currentIndex() == 0:
                self.local = True
            else:
                self.local = False
                self.runparams["user"] = self.runWidget.userLineEdit.text()
                self.runparams["pass"] = self.runWidget.passLineEdit.text()
                self.runparams["server"] = self.runWidget.serverLineEdit.text()
                self.runparams["binary"] = self.runWidget.binaryLineEdit.text()
                self.runparams["script"] = self.runWidget.scriptComboBox.currentIndex()            
            
            if not len(self.params["dims"]) == self.params["ndim"]:
                raise DimsError, ("The number of trajectories must be the same as the dimensionality")
            if not len(self.params["omega"]) == self.params["nfield"]:
                raise DimsError, ("The number of frequencies must be the same as the number of fields")
            if not len(self.params["ef"]) == self.params["nfield"]:
                    raise DimsError, ("The number of field strengths must be the same as the number of fields")
            if self.params["env"] == 3 or self.params["env"] == 4:
                if not len(self.params["ce"]) == self.params["nfield"]:
                    raise DimsError, ("The number of CE phases must be the same as the number of fields")
                if not len(self.params["fwhm"]) == self.params["nfield"]:
                    raise DimsError, ("The number of FWHMs must be the same as the number of fields")
                        
            self.params["dims"] = self.generalWidget.dimsLineEdit.text()
            self.params["omega"] = self.fieldWidget.omegaLineEdit.text()
            self.params["ef"] = self.fieldWidget.efLineEdit.text()
            self.params["ce"] = self.fieldWidget.ceLineEdit.text()
            self.params["fwhm"] = self.fieldWidget.fwhmLineEdit.text()
            
        #these are to test for the existence of a few files, the files may not exist so it isnt terribly important
        #    if not QFile.exists(self.params["input_file"]):
        #        raise FileError, ("input file does not exist")
        #    if not QFile.exists(self.params["output_file"]):
        #        raise FileError, ("output file does not exist")
        except DimsError, e:
            QMessageBox.warning(self, "Dimensionality Error", unicode(e))
            self.generalWidget.dimsLineEdit.selectAll()
            self.generalWidget.dimsLineEdit.setFocus()
            return
        except ValueError, e:
            QMessageBox.warning(self, "Value Error", unicode(e))
            return
        #except FileError, e:
        #    QMessageBox.warning(self, "File Error", unicode(e))
        #    return
        QDialog.accept(self)

#TODO: set tooltips!!
        
class GeneralParamsWidget(QWidget):
    def __init__(self, parent = None):
        super(GeneralParamsWidget, self).__init__(parent)

        ndimLabel = QLabel("Number of Dimensions:")
        self.ndimSpinbox = QSpinBox()
        self.ndimSpinbox.setRange(2,400)
        self.ndimSpinbox.setValue(4)
        self.ndimSpinbox.setToolTip("Set Number of Dimensions, must be greater than 2")
        self.ndimSpinbox.setStatusTip(self.ndimSpinbox.toolTip())
        self.connect(self.ndimSpinbox, SIGNAL("valueChanged(int)"), self.ndimChange)
        
        dimsLabel = QLabel("Trajectories per Dimension:")
        self.dimsLineEdit = QLineEdit("1,1,1,1")
        self.dimsLineEdit.setProperty("valid", (True))
        self.connect(self.dimsLineEdit, SIGNAL("textChanged(QString)"), self.ndimChange)
        
        distType = {1:"Monte-Carlo",2:"Linear"}
        distLabel = QLabel("Distribution Type:")
        self.distBox = QComboBox()
        for k,v in distType.items():
            self.distBox.insertItem(k, v)

        inFileLabel = QLabel("Input File Name:")
        self.inFileButton = QPushButton("&Open")
        self.inFileLineEdit = QLineEdit()
        self.inFileLineEdit.setMinimumWidth(200)
        self.inFileLineEdit.setMaximumWidth(600)
        self.inFileLineEdit.setText("input.dat")
        self.connect(self.inFileButton, SIGNAL("clicked()"), lambda who="in": self.changeButton(who))
        
        outFileLabel = QLabel("Output File Name:")
        self.outFileButton = QPushButton("&Open")
        self.outFileLineEdit = QLineEdit()
        self.outFileLineEdit.setMinimumWidth(200)
        self.outFileLineEdit.setMaximumWidth(600)
        self.outFileLineEdit.setText("output.dat")
        self.connect(self.outFileButton, SIGNAL("clicked()"), lambda who="out": self.changeButton(who))
        
        generalLayout = QGridLayout()
        generalLayout.addWidget(ndimLabel, 0, 0)
        generalLayout.addWidget(self.ndimSpinbox, 1, 0)
        generalLayout.addWidget(dimsLabel, 2, 0)
        generalLayout.addWidget(self.dimsLineEdit, 3, 0)
        generalLayout.addWidget(distLabel, 4, 0)
        generalLayout.addWidget(self.distBox, 5, 0)
        generalLayout.addWidget(inFileLabel, 0, 1)
        generalLayout.addWidget(self.inFileLineEdit, 1, 1)
        generalLayout.addWidget(outFileLabel, 2, 1)
        generalLayout.addWidget(self.outFileLineEdit, 3, 1)
        generalLayout.addWidget(self.inFileButton, 1, 2)
        generalLayout.addWidget(self.outFileButton, 3, 2)
        
        self.setLayout(generalLayout)
        self.setStyleSheet(StyleSheet)
        
    def ndimChange(self):
        if len(unicode(self.dimsLineEdit.text()).split(",")) == self.ndimSpinbox.value():
            self.dimsLineEdit.setProperty("valid",QVariant(True))
            self.setStyleSheet(StyleSheet)
        else:
            self.dimsLineEdit.setProperty("valid",QVariant(False))
            self.setStyleSheet(StyleSheet)
        
    def changeButton(self,who):
        findFile = QFileDialog()
        findFile.setFileMode(QFileDialog.AnyFile)
        if platform.system() == "Darwin":
            findFile.setOption(QFileDialog.DontUseNativeDialog,True)
        if not findFile.exec_():
      # exit if cancel
            return
        filenames = findFile.selectedFiles()
        filename = filenames.takeFirst()
        #filename = QFileDialog.getSaveFileName(self, 'Open file','.') 
        if who == "in":
            self.inFileLineEdit.setText(filename)
        else:
            self.outFileLineEdit.setText(filename)

class TimeParamsWidget(QWidget):
    def __init__(self, parent = None):
        super(TimeParamsWidget, self).__init__(parent)
        
        tiLabel = QLabel("Start Time:")
        self.tiSpinBox = QSpinBox()
        self.tiSpinBox.setRange(-1000000000,1000000000)
        self.tiSpinBox.setValue(0)
        self.tiSpinBox.setMaximumWidth(80)
        
        tfLabel = QLabel("End Time:")
        self.tfSpinBox = QSpinBox()
        self.tfSpinBox.setRange(-1000000000,1000000000)
        self.tfSpinBox.setValue(100)
        self.tfSpinBox.setMaximumWidth(80)
        
        dtLabel = QLabel("Initial Time Step:")
        self.dtLineEdit = QLineEdit("0.001")
        self.dtLineEdit.setMaximumWidth(80)
        
        tifLayout = QHBoxLayout()
        tifLayout.addStretch()
        for item in [tiLabel, self.tiSpinBox, tfLabel, self.tfSpinBox]:
            tifLayout.addWidget(item)
        tifLayout.addStretch()
        
        dtLayout = QHBoxLayout()
        dtLayout.addStretch()
        for item in [dtLabel, self.dtLineEdit]:
            dtLayout.addWidget(item)
        dtLayout.addStretch()
            
        layout = QVBoxLayout()
        layout.addLayout(tifLayout)
        layout.addLayout(dtLayout)
        
        self.setLayout(layout)
        
class WfParamsWidget(QWidget):
    def __init__(self, parent = None):
        super(WfParamsWidget, self).__init__(parent)
        
        wfType = {1:"Hydrogen Atom-like", 2:"Hydrogen Molecule-like", 3:"GAMESS-US checkpoint", 4:"Wavefunction on Grid"}
        wfTypeLabel = QLabel("Wavefunction type:")
        self.wfTypeBox = QComboBox()
        for k,v in wfType.items():
            self.wfTypeBox.insertItem(k, v)
        self.wfTypeBox.setCurrentIndex(0)
            
        potType = wfType
        potTypeLabel = QLabel("Potential type:")
        self.potTypeBox = QComboBox()
        for k,v in potType.items():
            self.potTypeBox.insertItem(k, v)
        self.potTypeBox.setCurrentIndex(0)
            
        ipLabel = QLabel("Ionization Potential:")
        self.ipDoubleSpinBox = QDoubleSpinBox()
        self.ipDoubleSpinBox.setRange(0,10)
        self.ipDoubleSpinBox.setDecimals(3)
        self.ipDoubleSpinBox.setSingleStep(0.001)
        self.ipDoubleSpinBox.setValue(0.5)
        
        smoothingLabel = QLabel("Smoothing Parameter:")
        self.smoothingLineEdit = QLineEdit("1E-4")
        self.smoothingLineEdit.setMaximumWidth(80)

        chargeLabel = QLabel("Charge on core:")
        self.chargeLineEdit = QLineEdit("1")
        self.chargeLineEdit.setMaximumWidth(50)
                
        chargesLabel = QLabel("Charges on cores:")
        self.chargesLineEdit = QLineEdit("1,1")
        self.chargesLineEdit.setMaximumWidth(50)
        
        rnucLabel = QLabel("Internuclear Seperation:")
        self.rnucDoubleSpinBox = QDoubleSpinBox()
        self.rnucDoubleSpinBox.setRange(0,10)
        self.rnucDoubleSpinBox.setDecimals(3)
        self.rnucDoubleSpinBox.setSingleStep(0.001)
        self.rnucDoubleSpinBox.setValue(4.0)
        self.rnucDoubleSpinBox.setMaximumWidth(80)
        
        thetaLabel = QLabel("Molecular Orientation:")
        self.thetaDoubleSpinBox = QDoubleSpinBox()
        self.thetaDoubleSpinBox.setRange(0,360)
        self.thetaDoubleSpinBox.setDecimals(1)
        self.thetaDoubleSpinBox.setSingleStep(0.1)
        self.thetaDoubleSpinBox.setValue(0.0)
        self.thetaDoubleSpinBox.setMaximumWidth(80)
        

        blankWidget = QWidget()
        
        atomWidget = QWidget()
        atomLayout = QHBoxLayout()
        #atomLayout.addStretch()
        atomLayout.addWidget(chargeLabel)
        atomLayout.addWidget(self.chargeLineEdit)
        atomLayout.addStretch()
        atomWidget.setLayout(atomLayout)
        
        molWidget = QWidget()
        molLayout = QVBoxLayout()
        molUpLayout = QHBoxLayout()
        molUpLayout.addWidget(chargesLabel)
        molUpLayout.addWidget(self.chargesLineEdit)
        molUpLayout.addWidget(rnucLabel)
        molUpLayout.addWidget(self.rnucDoubleSpinBox)
        molDownLayout = QHBoxLayout()
        molDownLayout.addWidget(thetaLabel)
        molDownLayout.addWidget(self.thetaDoubleSpinBox)
        molDownLayout.addStretch()
        molLayout.addLayout(molUpLayout)
        molLayout.addLayout(molDownLayout)
        molWidget.setLayout(molLayout)
        
        self.stackedWidget = QStackedWidget()
        self.stackedWidget.addWidget(atomWidget)
        self.stackedWidget.addWidget(molWidget)
        self.stackedWidget.addWidget(blankWidget)


        labelLayout = QHBoxLayout()
        labelLayout.addWidget(wfTypeLabel)
        labelLayout.addWidget(self.wfTypeBox)
        itemLayout = QHBoxLayout()
        itemLayout.addWidget(potTypeLabel)
        itemLayout.addWidget(self.potTypeBox)
        smoothLayout = QHBoxLayout()
        smoothLayout.addWidget(ipLabel)
        smoothLayout.addWidget(self.ipDoubleSpinBox)
        smoothLayout.addWidget(smoothingLabel)
        smoothLayout.addWidget(self.smoothingLineEdit) 
        
        wfLayout = QVBoxLayout()
        wfLayout.addLayout(labelLayout)
        wfLayout.addLayout(itemLayout)
        wfLayout.addLayout(smoothLayout)
        wfLayout.addWidget(self.stackedWidget)
        
        self.connect(self.potTypeBox,SIGNAL("currentIndexChanged(QString)"),self.changeStacked)      
        
        self.setLayout(wfLayout)
        
    def changeStacked(self, text):
        if text == "Hydrogen Atom-like":
            self.stackedWidget.setCurrentIndex(0)
        elif text == "Hydrogen Molecule-like":
            self.stackedWidget.setCurrentIndex(1)
        else:
            self.stackedWidget.setCurrentIndex(2)
    
class FieldParamsWidget(QWidget):
    def __init__(self, parent = None):
        super(FieldParamsWidget, self).__init__(parent)

        nfieldLabel = QLabel("Number of fields:")
        self.nfieldSpinBox = QSpinBox()
        self.nfieldSpinBox.setRange(0,100)
        self.nfieldSpinBox.setValue(1)
        self.connect(self.nfieldSpinBox, SIGNAL("valueChanged(int)"), self.nfieldChange)
        
        efLabel = QLabel("Strength per Field (a.u.):")
        self.efLineEdit = QLineEdit("0.01")
        self.efLineEdit.setProperty("valid", (True))
        self.connect(self.efLineEdit, SIGNAL("textChanged(QString)"), self.nfieldChange)
        
        fwhmLabel = QLabel("FWHM per Field (a.u.):")
        self.fwhmLineEdit = QLineEdit("0.")
        self.fwhmLineEdit.setProperty("valid", (True))
        self.connect(self.fwhmLineEdit, SIGNAL("textChanged(QString)"), self.nfieldChange)
        
        omegaLabel = QLabel("Frequency per Field (a.u.):")
        self.omegaLineEdit = QLineEdit("0.057")
        self.omegaLineEdit.setProperty("valid", (True))
        self.connect(self.omegaLineEdit, SIGNAL("textChanged(QString)"), self.nfieldChange)
        
        ceLabel = QLabel("CEP per Field (a.u.):")
        self.ceLineEdit = QLineEdit("0.")
        self.ceLineEdit.setProperty("valid", (True))
        self.connect(self.ceLineEdit, SIGNAL("textChanged(QString)"), self.nfieldChange)
        
        envType = {1:"Static", 2:"Constant Cosine", 3:"Numerical", 4:"Gaussian", 5:"Sine Squared"}
        envLabel = QLabel("Type of Envelope:")
        self.envComboBox = QComboBox()
        for k,v in envType.items():
            self.envComboBox.insertItem(k, v)
        self.envComboBox.setCurrentIndex(1)
        
        blankWidget = QWidget()
        envWidget = QWidget()
        envLayout = QHBoxLayout()
        for item in [fwhmLabel, self.fwhmLineEdit, ceLabel, self.ceLineEdit]:
            envLayout.addWidget(item)
        envWidget.setLayout(envLayout)
          
        self.stackedWidget = QStackedWidget()
        self.stackedWidget.addWidget(blankWidget)
        self.stackedWidget.addWidget(envWidget)

        
        layout = QVBoxLayout()
        topLayout = QHBoxLayout()
        for item in [nfieldLabel, self.nfieldSpinBox, efLabel, self.efLineEdit]:
            topLayout.addWidget(item)
        midLayout = QHBoxLayout()
        for item in [envLabel, self.envComboBox, omegaLabel, self.omegaLineEdit]:
            midLayout.addWidget(item)
        
        layout.addLayout(topLayout)
        layout.addLayout(midLayout)
        layout.addWidget(self.stackedWidget)
        
        self.connect(self.envComboBox,SIGNAL("currentIndexChanged(QString)"),self.changeStacked)  
                
        self.setLayout(layout)
   
    def nfieldChange(self):
        edits = [self.efLineEdit,
                 self.ceLineEdit,
                 self.fwhmLineEdit,
                 self.omegaLineEdit]
        nfields = self.nfieldSpinBox.value()
        for i in edits:
            if len(unicode(i.text()).split(",")) == nfields:
                i.setProperty("valid",QVariant(True))
            else:
                i.setProperty("valid",QVariant(False))
        self.setStyleSheet(StyleSheet)       
        
    def changeStacked(self, text):
        if text == "Static" or text == "Constant Cosine" or text == "Numerical":
            self.stackedWidget.setCurrentIndex(0)
        else:
            self.stackedWidget.setCurrentIndex(1)      
            
class RunParamsWidget(QWidget):
    def __init__(self, parent = None):
        super(RunParamsWidget, self).__init__(parent)
        
        nthreadLabel = QLabel("Number of threads:")
        self.nthreadSpinBox = QSpinBox()
        self.nthreadSpinBox.setRange(1,1000)
        self.nthreadSpinBox.setValue(1)
        
        runLocationType = {1:"local", 2:"remote"}
        runLocationLabel = QLabel("Simulation Location:")
        self.runLocationComboBox = QComboBox()
        for k,v in runLocationType.items():
            self.runLocationComboBox.insertItem(k, v)
            
        userLabel = QLabel("Username:")
        self.userLineEdit = QLineEdit("user")
        passLabel = QLabel("Password:")
        self.passLineEdit = QLineEdit("password")
        self.passLineEdit.setEchoMode(QLineEdit.Password)
        serverLabel = QLabel("Server:")
        self.serverLineEdit = QLineEdit("example.com")
        binaryLabel = QLabel("Binary Name:")
        self.binaryLineEdit = QLineEdit("cpp-class")
        scriptLabel = QLabel("Script type")
        self.scriptComboBox = QComboBox()
        self.scriptComboBox.insertItems(1, ["shell script","PBS script"])
        
        #TODO: connect user and password etc. to QSettings to store them.
        blankWidget = QWidget()
        remoteWidget = QWidget()
        remoteLayout = QVBoxLayout()
        remoteTopLayout = QHBoxLayout()       
        for item in [userLabel, self.userLineEdit, passLabel, self.passLineEdit]:
            remoteTopLayout.addWidget(item)
        remoteBottomLayout = QHBoxLayout()
        for item in [serverLabel, self.serverLineEdit, binaryLabel, self.binaryLineEdit, scriptLabel, self.scriptComboBox]:
            remoteBottomLayout.addWidget(item)
        remoteLayout.addLayout(remoteTopLayout)
        remoteLayout.addLayout(remoteBottomLayout)
        remoteWidget.setLayout(remoteLayout)
          
        self.stackedWidget = QStackedWidget()
        self.stackedWidget.addWidget(blankWidget)
        self.stackedWidget.addWidget(remoteWidget)
        
        layout = QVBoxLayout()
        topRow = QHBoxLayout()
        for item in [nthreadLabel, self.nthreadSpinBox, runLocationLabel, self.runLocationComboBox]:
            topRow.addWidget(item)
        layout.addLayout(topRow)
        layout.addWidget(self.stackedWidget)
        
        self.connect(self.runLocationComboBox,SIGNAL("currentIndexChanged(QString)"),self.changeStacked)  
                
        self.setLayout(layout)
        
    def changeStacked(self, text):
        if text == "local":
            self.stackedWidget.setCurrentIndex(0)
        else:
            self.stackedWidget.setCurrentIndex(1)  
            
if __name__ == "__main__":
    import sys

    app = QApplication(sys.argv)
    #form = GeneralParamsWidget()
    form = ParamsForm()
    form.show()
    app.exec_()
        