CXX = g++

CXXFLAGS = -O3 -mfpmath=sse -pthread -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format
TSAN =  $(CXXFLAGS) -fsanitize=thread
GPRO3 = -pg $(CXXFLAGS)
GPR = -pg $(WO3)
WO3 = -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format

TARGET = a.out
DEBUGTARGET = debug.out
TSANTARGET = tsan.out

OBJS = main.o msr.o mytime.o extpll.o
CPPFILES = main.cpp msr.cpp mytime.cpp extpll.cpp
HFILES = msr.h mytime.h extpll.h args.h

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(DEBUGTARGET) : $(OBJS)
	$(CXX) $(WO3) -o $(DEBUGTARGET) $(OBJS)

$(TSANTARGET) : $(OBJS)
	$(CXX) $(TSAN) -o $(TSANTARGET) $(OBJS)

main.o : main.cpp msr.h mytime.h
	$(CXX) $(CXXFLAGS) -c main.cpp

msr.o : msr.cpp msr.h mytime.h
	$(CXX) $(CXXFLAGS) -c msr.cpp

mytime.o : mytime.cpp mytime.h
	$(CXX) $(CXXFLAGS) -c mytime.cpp

extpll.o : extpll.cpp extpll.h
	$(CXX) $(CXXFLAGS) -c extpll.cpp

clean :
	rm -f $(OBJS) $(TARGET)

mainzip :
	zip Frolov_PS.zip $(CPPFILES) $(HFILES) makefile

zip :
	zip msr.zip $(CPPFILES) $(HFILES) makefile
