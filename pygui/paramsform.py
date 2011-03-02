
from PyQt4.QtCore import *
from PyQt4.QtGui import *

class ParamsForm(QDialog):
    def __init__(self, width, heigth, parent=None):
        super(ParamsForm, self).__init__(parent)
        

        
if __name__ == "__main__":
    import sys

    app = QApplication(sys.argv)
    form = ParamsForm(64, 128)
    form.show()
    app.exec_()