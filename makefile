#	By Ninjihaku Software (C) 2015
bindir := bin/
objdir := bin/obj/
expdir := experimental/
$(srclist) : EvenAwesomerSockets.cpp ServerMessages.cpp User.cpp Room.cpp newmain.cpp Resources/flockcon.rc
objlist-exp := ${objdir}EvenAwesomerSockets-e.obj ${objdir}ServerMessages-e.obj ${objdir}dmodule-e.obj ${objdir}User-e.obj ${objdir}Room-e.obj ${objdir}Main-e.obj ${objdir}flockcon-exp.res
objlist-rel := ${objdir}EvenAwesomerSockets.obj ${objdir}ServerMessages.obj ${objdir}Room.obj ${objdir}User.obj ${objdir}Main.obj ${objdir}flockcon.res
objlist-deb := ${objdir}EvenAwesomerSockets-d.obj ${objdir}ServerMessages-d.obj ${objdir}Room-d.obj ${objdir}User-d.obj ${objdir}Main-d.obj ${objdir}flockcon-debug.res
objlist-all := ${objlist-rel} ${objlist-deb}
objflags := -std=c++11
exerelflags := -static-libgcc -lm -lws2_32
exedebflags := -lm -lws2_32
exenames := ${bindir}flockcon.exe ${bindir}flockcon-debug.exe
bkdir := ../flockback

#Compile and link everything
all: $(srclist)
	g++ -c EvenAwesomerSockets.cpp -o ${objdir}EvenAwesomerSockets.obj ${objflags}
	g++ -c ServerMessages.cpp -o ${objdir}ServerMessages.obj ${objflags}
	g++ -c User.cpp -o ${objdir}User.obj ${objflags}
	g++ -c Room.cpp -o ${objdir}Room.obj ${objflags}
	g++ -c newmain.cpp -o ${objdir}Main.obj -static-libgcc ${objflags}
	g++ -c -g EvenAwesomerSockets.cpp -o ${objdir}EvenAwesomerSockets-d.obj -D_debug_ ${objflags}
	g++ -c -g ServerMessages.cpp -o ${objdir}ServerMessages-d.obj -D_debug_ ${objflags}
	g++ -c -g User.cpp -o ${objdir}User-d.obj -D_debug_ ${objflags}
	g++ -c -g Room.cpp -o ${objdir}Room-d.obj -D_debug_ ${objflags}
	g++ -c -g newmain.cpp -o ${objdir}Main-d.obj -static-libgcc -D_debug_ ${objflags}
	windres -iResources/flockcon.rc -O coff -D_release_ -o ${objdir}flockcon.res
	windres -iResources/flockcon.rc -O coff -D_debug_ -o ${objdir}flockcon-debug.res
	g++ -O2 ${objlist-rel} ${exerelflags} -o bin/flockcon.exe
	g++ -g ${objlist-deb} ${exedebflags} -D_debug_ -o bin/flockcon-debug.exe

#Compile and link only the release version
release-all: $(srclist)
	g++ -c EvenAwesomerSockets.cpp -o ${objdir}EvenAwesomerSockets.obj ${objflags}
	g++ -c ServerMessages.cpp -o ${objdir}ServerMessages.obj ${objflags}
	g++ -c User.cpp -o ${objdir}User.obj ${objflags}
	g++ -c Room.cpp -o ${objdir}Room.obj ${objflags}
	g++ -c newmain.cpp -o ${objdir}Main.obj -static-libgcc ${objflags}
	windres -iResources/flockcon.rc -O coff -D_release_ -o ${objdir}flockcon.res
	g++ -O2 ${objdir}EvenAwesomerSockets.obj ${objdir}ServerMessages.obj ${objdir}Main.obj ${objdir}flockcon.res ${exerelflags} -o bin/flockcon.exe

#Compile and link only the debug version
debug-all: $(srclist)
	g++ -c -g EvenAwesomerSockets.cpp -o ${objdir}EvenAwesomerSockets-d.obj -D_debug_ ${objflags}
	g++ -c -g ServerMessages.cpp -o ${objdir}ServerMessages-d.obj -D_debug_ ${objflags}
	g++ -c -g User.cpp -o ${objdir}User-d.obj -D_debug_ ${objflags}
	g++ -c -g Room.cpp -o ${objdir}Room-d.obj -D_debug_ ${objflags}
	g++ -c -g newmain.cpp -o ${objdir}Main-d.obj -static-libgcc -D_debug_ ${objflags}
	g++ -g ${objdir}EvenAwesomerSockets-d.obj ${objdir}ServerMessages-d.obj ${objdir}Main-d.obj ${objdir}flockcon-debug.res ${exedebflags} -o bin/flockcon-debug.exe
	
#Compile only the network manager
sockets: EvenAwesomerSockets.cpp
	g++ -c EvenAwesomerSockets.cpp -o ${objdir}EvenAwesomerSockets.obj ${objflags}
	g++ -c -g EvenAwesomerSockets.cpp -o ${objdir}EvenAwesomerSockets-d.obj -D_debug_ ${objflags}
	
#Compile only the Server messaging
server-messages: ServerMessages.cpp
	g++ -c ServerMessages.cpp -o ${objdir}ServerMessages.obj ${objflags}
	g++ -c -g ServerMessages.cpp -o ${objdir}ServerMessages-d.obj -D_debug_ ${objflags}

#Compile only the User class
user: User.cpp
	g++ -c User.cpp -o ${objdir}User.obj ${objflags}
	g++ -c -g User.cpp -o ${objdir}User-d.obj -D_debug_ ${objflags}
	
#Compile only the Room class
room: Room.cpp
	g++ -c Room.cpp -o ${objdir}Room.obj ${objflags}
	g++ -c -g Room.cpp -o ${objdir}Room-d.obj -D_debug_ ${objflags}
	
#Compile only the main core
main: newmain.cpp
	g++ -c newmain.cpp -o ${objdir}Main.obj ${objflags}
	g++ -c -g newmain.cpp -o ${objdir}Main.obj -static-libgcc -D_debug_ ${objflags}
	
#Compile only the resources
project-resources: Resources/flockcon.rc
	windres -iResources/flockcon.rc -O coff -D_release_ -o ${objdir}flockcon.res
	windres -iResources/flockcon.rc -O coff -D_debug_ -o ${objdir}flockcon-debug.res
	
#Link everything
linkall: $(objlist-all)
	g++ -O2 ${objlist-rel} ${exerelflags} -o bin/flockcon.exe
	g++ -g ${objlist-deb} ${exedebflags} -o bin/flockcon-debug.exe
	
#Link only the release files
linkrel: $(objlist-rel)
	g++ -O2 ${objlist-rel} ${exerelflags} -o bin/flockcon.exe
	
#Link only the debug files
linkdeb: $(objlist-deb)
	g++ -g ${objlist-deb} ${exedebflags} -o bin/flockcon-debug.exe

#Cleanup	
clean:
	rm -R ${bindir}
	mkdir ${bindir}
	mkdir ${objdir}
	
#Create the backup directory
backstruct:
	mkdir ${bkdir}
	mkdir ${bkdir}/bin
	mkdir ${bkdir}/Resources
	mkdir ${bkdir}/experimental
	mkdir ${bkdir}/deprecated

#Make a backup of the current code
save: $(srclist)
	cp -R ./* ${bkdir}
	
#Restore the back up code
restore:
	cp -R ${bkdir}/* .
	
#Compile the experimental module. This version contains routines that are under development/investigation,
#but must be excluded from the release version. This version always has the debug flag enabled.
experiment: $(srclist)
	g++ -c -g EvenAwesomerSockets.cpp -o ${objdir}EvenAwesomerSockets-e.obj -D_debug_ ${objflags}
	g++ -c -g ServerMessages.cpp -o ${objdir}ServerMessages-e.obj -D_debug_ ${objflags}
	g++ -c -g experimental/dmodule.cpp -o ${objdir}dmodule-e.obj -D_debug_ ${objflags}
	g++ -c -g User.cpp -o ${objdir}User-e.obj -D_debug_ ${objflags}
	g++ -c -g Room.cpp -o ${objdir}Room-e.obj -D_debug_ ${objflags}
	g++ -c -g newmain.cpp -o ${objdir}Main-e.obj -static-libgcc -D_debug_ ${objflags}
	windres -iResources/flockcon.rc -O coff -D_experiment_ -o ${objdir}flockcon-exp.res
	g++ -g ${objlist-exp} ${exedebflags} -o bin/flockcon-exp.exe