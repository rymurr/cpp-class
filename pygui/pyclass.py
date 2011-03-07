
import sys
import os
import platform

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import qrc_resources
import helpform
import paramsform
import runclass

__version__ = "0.5.0"

#TODO: add paramsform as a QDockWidget?
#TODO: connect log to output of runs, HTML it for prettiness
#TODO: add some sort of status bar?
#TODO: change program name and title bar

info = "<font color=blue>INFO:</font>"
error = "<b><font color=red>ERROR:</font></b>"
end = "<br>"

class MainWindow(QMainWindow):
    
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        
        self.image = QImage()
        self.dirty = False
        self.runAllBool = False
        self.runBinningBool = False
        self.runICGenBool = False
        self.runParamsBool = True
        self.runParamsTestBool = False
        self.runPlotBool = False
        self.runTrajsBool = False        
        
        self.imageLabel = QLabel()
        self.imageLabel.setMinimumSize(500, 400)
        self.imageLabel.setAlignment(Qt.AlignCenter)
        self.imageLabel.setContextMenuPolicy(Qt.ActionsContextMenu)
        self.setCentralWidget(self.imageLabel)
        
        logDockWidget = QDockWidget("Log", self)
        logDockWidget.setObjectName("LogDockWidget")
        logDockWidget.setAllowedAreas(Qt.LeftDockWidgetArea|Qt.RightDockWidgetArea)
        #TODO: this may need to be changed from a QListWidget depending on how I implement it
        self.textEditView = QTextEdit()
        self.textEditView.setReadOnly(True)
        logDockWidget.setWidget(self.textEditView)
        self.addDockWidget(Qt.RightDockWidgetArea, logDockWidget)
        
        fileNewAction = self.createAction("&New...", self.fileNew,
                QKeySequence.New, "filenew", "Create a new simulation")
        fileOpenAction = self.createAction("&Open...", self.fileOpen,
                QKeySequence.Open, "fileopen",
                "Open an existing simulation archive")
        fileSaveAction = self.createAction("&Save", self.fileSave,
                QKeySequence.Save, "filesave", "Save the simulation")
        fileSaveAsAction = self.createAction("Save &As...", self.fileSaveAs
                , icon="filesaveas", tip="Save the simulation using a new name")
        fileQuitAction = self.createAction("&Quit", self.close,
                "Ctrl+Q", "filequit", "Close the application")
        
        self.runParamsAction = self.createAction("&Parameter List...", self.paramList,
                                                 "Ctrl+P", "runparams", "Set Parameters")
        self.runParamsTestAction = self.createAction("Parameters &Test", self.paramTest,
                                                     "Ctrl+T", "runparamstest", "Test Paramters")
        self.runICGenAction = self.createAction("&Generate Initial Conditions", self.genICs,
                                                "Ctrl+G", "runicgen", "Generate Initial Conditions")
        self.runTrajsAction = self.createAction("Run &Trajectories", self.runTrajs,
                                                "Ctrl+R", "runtrajs", "Run Trajectories")
        self.runBinningAction = self.createAction("&Bin Trajectories", self.binTrajs,
                                                  "Ctrl+B", "runbins", "Bins Trajectories based on initial parameters")
        self.runPlotAction = self.createAction("&Plots trajectories", self.plotTrajs,
                                               "Ctrl+P", "runplot", "Plots binned trajectories")
        self.runAllAction = self.createAction("Runs &All", self.runAll,
                                              "Ctrl+A", "runall", "Create run and plot parameters")
        
        viewLogAction = self.createAction("View &Log", self.viewLog, "Display Log", checkable=True)
        viewParamsAction = self.createAction("View P&arameters", self.paramView,
                                             "Alt+P", "view Parameter List", checkable=True)
                
        helpAboutAction = self.createAction("&About Classical Propagator GUI",
                self.helpAbout)
        helpHelpAction = self.createAction("&Help", self.helpHelp,
                QKeySequence.HelpContents)

        fileMenu = self.menuBar().addMenu("&File")
        self.addActions(fileMenu,(fileNewAction, fileOpenAction,
                fileSaveAction, fileSaveAsAction, None,
                fileQuitAction))
        self.runMenu = self.menuBar().addMenu("&Run")
        self.addActions(self.runMenu, (self.runParamsAction,self.runParamsTestAction,
                                       self.runICGenAction,self.runTrajsAction,
                                       self.runBinningAction,self.runPlotAction,
                                       self.runAllAction))
        viewMenu = self.menuBar().addMenu("&View")
        self.addActions(viewMenu, (viewLogAction,viewParamsAction))
        helpMenu = self.menuBar().addMenu("&Help")
        self.addActions(helpMenu,(helpAboutAction,helpHelpAction))
        
        fileToolbar = self.addToolBar("File")
        fileToolbar.setObjectName("FileToolBar")
        self.addActions(fileToolbar, (fileNewAction,fileOpenAction,fileSaveAction,fileSaveAsAction))
        
        self.runToolbar = self.addToolBar("Run")
        self.runToolbar.setObjectName("RunToolBar")
        self.addActions(self.runToolbar, (self.runParamsAction,self.runParamsTestAction,
                                          self.runICGenAction,self.runTrajsAction,
                                          self.runBinningAction,self.runPlotAction, None,
                                          self.runAllAction))
        
        self.setEnabled()

    def setEnabled(self):
        self.runAllAction.setEnabled(self.runAllBool)
        self.runParamsAction.setEnabled(self.runParamsBool)
        self.runParamsTestAction.setEnabled(self.runParamsTestBool)
        self.runICGenAction.setEnabled(self.runICGenBool)
        self.runTrajsAction.setEnabled(self.runTrajsBool)
        self.runBinningAction.setEnabled(self.runBinningBool)
        self.runPlotAction.setEnabled(self.runPlotBool)
        
    def paramList(self):
        dialog = paramsform.ParamsForm()
        if dialog.exec_():
            self.params = dialog.paramsDict()
            if dialog.local:
                self.local = True
            else:
                self.local = False
                self.runparams = dialog.runParamsDict()
        self.runParamsTestBool = True
        self.runAllBool = True
        self.runBinningBool = False
        self.runICGenBool = False
        self.runPlotBool = False
        self.runTrajsBool = False 
        self.setEnabled()
        self.textEditView.insertHtml(info + "Parameter List created successfully."+end)
        

    def convertDict(self):
        self.filename = "input.cfg"
        try:
            f = open(self.filename, 'w')
        except IOError:
            self.textEditView.insertHtml(error + "Failed to create configuration file."+end)
        for k,v in self.params.items():
            f.write(k + " = " + str(v) + "\n")
        f.close()
    
    def runIf(self, text, fileMap=None):
        try:
            if self.local:
                runclass.runLocal(self.params,self.textEditView, fileMap)
            else:
                runclass.runRemote(self.params,self.runparams,self.textEditView, fileMap)
        except runclass.ClassicalError: 
            self.textEditView.insertHtml(error + text + " failed to run"+end)
        else:
            self.textEditView.insertHtml(info + text + " ran successfully"+end)
            
    def paramTest(self):
        self.convertDict()
        fileMap = {"cfg":self.filename,}
        self.params["run-type"] = 1
        self.runIf("Configuration", fileMap)
        self.runICGenBool = True
        self.setEnabled()
 
            
    def genICs(self):
        self.params["run-type"] = 2
        self.runIf("Initial conditions")
        self.runTrajsBool = True
        self.setEnabled()
    
    def runTrajs(self):
        self.params["run-type"] = 3
        self.runIf("Trajectories")
        self.runBinningBool = True
        self.setEnabled()
    
    def binTrajs(self):
        self.params["run-type"] = 4
        self.runIf("Binning")
        self.runPlotBool = True
        self.setEnabled()
    
    def plotTrajs(self):
        return
    
    def runAll(self):
        self.params["run-type"] = 5
        self.runIf("Simulation ")
    
    def paramView(self):
         return
    
    def viewLog(self):
        return
     
    def fileNew(self):
        return
    
    def fileOpen(self):
        return
    
    def fileSave(self):
        return
    
    def close(self):
        return
    
    def fileSaveAs(self):
        return
    
    def helpHelp(self):
        form = helpform.HelpForm("index.html", self)
        form.show()
    
    def helpAbout(self):
        QMessageBox.about(self, "About Classical Propagator GUI",
        """<b>Classical Propagator</b> v %s
        <p>Copyright &copy; 2011 Ryan Murray 
        All rights reserved.
        <p>This application can be used to perform
        classical propagator simulations which approximate the Schrodinger
        equation for an electron in a strong laser field.
        <p>Python %s - Qt %s - PyQt %s on %s""" % (
        __version__, platform.python_version(),
        QT_VERSION_STR, PYQT_VERSION_STR, platform.system()))
    
    def createAction(self, text, slot=None, shortcut=None, icon=None,
                 tip=None, checkable=False, signal="triggered()"):
        action = QAction(text, self)
        if icon is not None:
            action.setIcon(QIcon(":/%s.png" % icon))
        if shortcut is not None:
            action.setShortcut(shortcut)
        if tip is not None:
            action.setToolTip(tip)
            action.setStatusTip(tip)
        if slot is not None:
            self.connect(action, SIGNAL(signal), slot)
        if checkable:
            action.setCheckable(True)
        return action

    def addActions(self, target, actions):
        for action in actions:
            if action is None:
                target.addSeparator()
            else:
                target.addAction(action)


        
def main():    
    app = QApplication(sys.argv)
    app.setOrganizationName("Imperial College London")
    app.setOrganizationDomain("qols.ph.ic.ac.uk")
    app.setApplicationName("PyQt Classical Propagator")
    app.setWindowIcon(QIcon(":/icon.png"))
    form = MainWindow()
    form.show()
    app.exec_()
    
main()    
