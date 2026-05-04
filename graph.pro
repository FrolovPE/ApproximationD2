QT += widgets


TARGET        = a.out

HEADERS       = args.h  \
				extpll.h  \
				msr.h  \
				mytime.h  \
				window.h 
		

SOURCES       = main.cpp \
				extpll.cpp\
				msr.cpp  \
				mytime.cpp  \
				window.cpp 
	

PROJECT_NAME  = graph.pro

mainzip.commands = zip Frolov_PS.zip $$SOURCES $$HEADERS  $$PROJECT_NAME
QMAKE_EXTRA_TARGETS += mainzip


zip.commands = zip Approx2D.zip $$SOURCES $$HEADERS  $$PROJECT_NAME
QMAKE_EXTRA_TARGETS += zip
