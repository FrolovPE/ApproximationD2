QT += widgets


TARGET        = a.out
HEADERS       = window.h \
		newton.h \
		spline.h 
		

SOURCES       = main.cpp \
                window.cpp \
		newton.cpp \
		spline.cpp 
	

PROJECT_NAME  = graph.pro

mainzip.commands = zip Frolov_PS.zip $$SOURCES $$HEADERS  $$PROJECT_NAME
QMAKE_EXTRA_TARGETS += mainzip


zip.commands = zip Approx.zip $$SOURCES $$HEADERS  $$PROJECT_NAME
QMAKE_EXTRA_TARGETS += zip
