CC 	= g++
CFLAGS  += -DLINUX -DSQL_WCHART_CONVERT -DLIB_EXPORTS

ARCH = _x86_

#version type(debug or release)
ifeq ($(VERTYPE),DEBUG)
CFLAGS  += -Wall -g -DDEBUG
endif
ifeq ($(VERTYPE),RELEASE)
CFLAGS  +=
endif

#
ifeq ($(OLEVEL),0)
CFLAGS  += 
endif
ifeq ($(OLEVEL),1)
CFLAGS  += -O1
endif
ifeq ($(OLEVEL),2)
CFLAGS  += -O2
endif
ifeq ($(OLEVEL),3)
CFLAGS  += -O3
endif


ifeq ($(GCCTYPE),OLD)
CC = g++343
endif
ifeq ($(GCCTYPE),NEW)
CC = g++
endif
ifeq ($(GCCTYPE),G64M32)
CC = g++
CFLAGS  += -D_686_M32_
endif
ifeq ($(GCCTYPE),X86_64)
CC = g++
CFLAGS  += -D_X64_ -DBUILD_LEGACY_64_BIT_MODE -fPIC
ARCH = _x64_
endif

ifeq ($(ARCH),_x86_)
    INCLUDE += -I/usr/include
    LIB += -L/usr/local/freetds/lib -ltdsodbc
endif
ifeq ($(ARCH),_x64_)
    INCLUDE += -I/usr/include
    LIB += -L/usr/local/freetds/lib -ltdsodbc
endif


#unixodbc include
#INCLUDE += -I/usr/include
#freedtds lib
#LIB += -L/usr/local/freetds/lib -ltdsodbc

ROBJS = #../../logmgr/LogMgr.o ../../common/LockBase.o ../../common/DerivedLock.o ../../common/SafeLock.o ../../common/pthread_cond_t_ex.o
LOBJS = SQLValue.o SQLField.o SQLResult.o SQLBase.o SQLException.o SQLConnection.o test.o
OBJS = $(ROBJS) $(LOBJS)

TARGET = ./test

$(TARGET): $(OBJS)
	$(CC) $(INCLUDE) $(CFLAGS) -o $@ $(OBJS) -lpthread $(LIB)

.cpp.o:
	$(CC) $(INCLUDE) $(CFLAGS) -c $<

clean:
	rm $(LOBJS)
	rm $(TARGET)
	rm ./*.o
