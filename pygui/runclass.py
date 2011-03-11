#try:
#    import classical
#    localBool = True
#except ImportError:
#    localBool = False
import subprocess
import os.path

info = "<font color=blue>INFO:</font>"
error = "<b><font color=red>ERROR:</font></b>"
end = "<br>"

class ClassicalError(Exception): pass

def runRemote(params, runparams, LogLineEdit, fileMap=None):
    pass

#TODO: make output prettier, add a print html method to params perhaps? 
#another flag to say wether we are in python or on the CLI? 
def runLocal(params, LogLineEdit, fileMap=None):
    if os.path.exists("../cpp-class"):
        localBool = True
    else:
        localBool = False
        
    if not localBool:
        LogLineEdit.insertHtml(error+"Can't run locally without the classical library!"+end)
        raise ClassicalError
    else:
#        if not fileMap == None:
        proc = Run(["/home/ryan/workspace/cpp-class/./cpp-class","-alsologtostderr"]) 
        Trace(proc, LogLineEdit)
            #classical.pyrun_main(2,["pyclass","-alsologtostderr"],fileMap["cfg"])
#        else:
            #classical.pyrun_main(2,["pyclass","-aslologtostderr"],"input.cfg")
            
def Run(command):
    proc = subprocess.Popen(command, bufsize=1,
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        universal_newlines=True)
    return proc

def Trace(proc, LogLineEdit):
    for line in proc.stdout:
        LogLineEdit.insertHtml(strip(line))
    
def strip(line):
    try:
        lineo = line.split("]")
        errcode = lineo[0][0]
        retstring = errHtml(errcode) + lineo[1] + end
    except IndexError:
        retstring = line
         
    return retstring

def errHtml(errcode):
    if errcode == "I":
        return info
    elif errcode == "E":
        return error
    else:
        return "Need new error code for: " + errcode
            
if __name__ == '__main__':
    proc = Run(["/home/ryan/workspace/cpp-class/./cpp-class","-alsologtostderr"])
    Trace(proc)