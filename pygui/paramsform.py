
from PyQt4.QtCore import *
from PyQt4.QtGui import *

#from labellededit import *
StyleSheet = '''
    QLineEdit[valid="false"] {background-color: rgb(255, 80, 80);}
'''

class ParamsForm(QDialog):
    def __init__(self, width, heigth, params=None, parent=None):
        super(ParamsForm, self).__init__(parent)
 
        
                
        tabWidget = QTabWidget()
        generalWidget = GeneralParamsWidget()
        tabWidget.addTab(generalWidget, "&General")
        layout = QVBoxLayout()
        layout.addWidget(tabWidget)
        self.setLayout(layout)
        
        
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
        
        distType = {"Monte-Carlo":1,"Linear":2}
        distLabel = QLabel("Distribution Type:")
        self.distBox = QComboBox()
        for k,v in distType.items():
            self.distBox.addItem(k, v)

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
           
 #   def inChangeLineEdit(self):
 #       pass
    
if __name__ == "__main__":
    import sys

    app = QApplication(sys.argv)
    #form = GeneralParamsWidget()
    form = ParamsForm(64, 128)
    form.show()
    app.exec_()