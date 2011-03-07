try:
    import classical
except ImportError:
    localBool = False

info = "<font color=blue>INFO:</font>"
error = "<b><font color=red>ERROR:</font></b>"
end = "<br>"

class ClassicalError(Exception): pass

def runRemote(params, runparams, LogLineEdit):
    pass

def runLocal(params, LogLineEdit):
    if not localBool:
        LogLineEdit.insertHtml(error+"Can't run locally without the classical library!"+end)
        raise ClassicalError
    else:
        pass