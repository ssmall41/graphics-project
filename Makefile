###########################################################################
##                                                                       ##
##  This is a makefile for a simple OpenGL program.                      ##
##                                                                       ##
##  Usage:                                                               ##
##     "make"       -> Compiles an optimized version of the executable.  ##
##     "make debug" -> Compiles a debug version of the executable.       ##
##     "make clean" -> Removes all temporary & executable files.         ##
##                                                                       ##
##                                                                       ##
##  Chris Wyman (January 29, 2008)                                       ##
###########################################################################


# This sets the name of your compiled program
EXENAME = glFramework


###########################################################################
##                                                                       ##
## To add additional files to be compiled, add them to the list of files ## 
##   in the correct directory.  If you change the name of the directory, ##
##   make sure you change it here!  If you move the location of a file,  ##
##   make sure you change it here.  If you add a new directory, make     ##
##   sure to add information about it here.                              ##	
##                                                                       ##
###########################################################################

# The directories for various file types
UTILDIR = Utils/
SCNDIR = Scene/
MATLDIR = Materials/
DATADIR = DataTypes/
UIDIR = Interface/
OBJSDIR = Objects/
RNDRDIR = RenderingTechniques/
IMIODIR = Utils/ImageIO/
MDIODIR = Utils/ModelIO/
SMLDIR = Utils/ModelIO/simpleModelLib/

# These should be located in $(UTILDIR)
UTILSRC = drawTextToGLWindow.cpp \
	TextParsing.cpp \
	Trackball.cpp \
	frameGrab.cpp \
	frameRate.cpp \
	framebufferObject.cpp \
	glslProgram.cpp \
	searchPathList.cpp

# These should be located in $(IMIODIR)
IMIOSRC = bmp.cpp \
	loadHDR.cpp \
	ppm.cpp \
	readrgb.cpp \
	rgbe.cpp 

# These should be located in $(MDIODIR)
MDIOSRC = glm.cpp

# These should be located in $(SMLDIR)
SMLSRC = edge.cpp \
	edgelist.cpp \
	face.cpp \
	facelist.cpp \
	halfedge.cpp \
	heapsort.cpp \
	input_hem.cpp \
	list.cpp \
	loop.cpp \
	obj_read.cpp \
	output_hem.cpp \
	solid.cpp \
	vertex.cpp \
	vertexlist.cpp \
	wrapper.cpp

# These should be located in $(SCNDIR)
SCNSRC = Camera.cpp \
	Scene.cpp \
	SceneRenderFuncs.cpp \
	glLight.cpp

# These should be located in $(MATLDIR)
MATLSRC = GLConstantMaterial.cpp \
	GLLambertianMaterial.cpp \
	GLLambertianTexMaterial.cpp \
	GLMaterial.cpp \
	GLSLShaderMaterial.cpp

# These should be located in $(DATADIR)
DATASRC = Matrix4x4.cpp \
	RGBColor.cpp \
	Vector.cpp \
	glTexture.cpp 

# These should be located in $(UIDIR)
UISRC = SceneFileDefinedInteraction.cpp \
	glKeyAndMouseCallbacks.cpp \
	glTrackballInterface.cpp

# These should be located in $(OBJSDIR)
OBJSSRC = Cylinder.cpp \
	Group.cpp \
	Mesh.cpp \
	Object.cpp \
	Quad.cpp \
	Sphere.cpp \
	Triangle.cpp

# These should be located in $(RNDRDIR)
RNDRSRC = initializeRenderingData.cpp \
	utilRoutines.cpp

# These should be located in the root directory (with the makefile)
MAINSRC = sceneLoader.cpp \
	glInterface.cpp \
	reflect.cpp \
	silhouette.cpp \
	gooch.cpp \
	selection.cpp \
	extras.cpp


###########################################################################
##                                                                       ##
## If you just add files to existing directories, you should not need to ##
##   modify anything below!                                              ##
##                                                                       ##
## To add additional (new) directories, you need to do 4 things:         ##
##    1) Create a variable, above, to describe the path (e.g.,           ##
##         "COREDIR = Core/")                                            ##
##    2) Create a variable, above, that includes a list of files in that ##
##         directory.                                                    ##
##    3) Add your directory variable to the list of directories in       ##
##         "VPATH = ..." below.                                          ##
##    4) Add another line to the SRC variable below of the form:         ##
##                          $(addprefix $(newDirVar), $(newSrcVar))      ##
##                                                                       ##	
###########################################################################

# A color-separated list, e.g., $(UTILDIR):$(COREDIR):$(ANOTHERDIR)
VPATH = $(UTILDIR):$(SCNDIR):$(MATLDIR):$(DATADIR):$(UIDIR):$(OBJSDIR):$(RNDRDIR):$(IMIODIR):$(MDIODIR):$(SMLDIR)


# A list of all the src files from various directories, prepended with 
#    their relative directory path.  If you add additional lines, make 
#    sure you add a '\' to the end of all lines except the last!
SRC = $(MAINSRC) \
	$(addprefix $(UTILDIR),$(UTILSRC)) \
	$(addprefix $(SCNDIR),$(SCNSRC)) \
	$(addprefix $(MATLDIR),$(MATLSRC)) \
	$(addprefix $(DATADIR),$(DATASRC)) \
	$(addprefix $(UIDIR),$(UISRC)) \
	$(addprefix $(OBJSDIR),$(OBJSSRC)) \
	$(addprefix $(RNDRDIR),$(RNDRSRC)) \
	$(addprefix $(IMIODIR),$(IMIOSRC)) \
	$(addprefix $(MDIODIR),$(MDIOSRC)) \
	$(addprefix $(SMLDIR),$(SMLSRC)) 


###########################################################################
##                                                                       ##
## You shouldn't need to change anything below, unless you want to       ##
##   change where compiled files are stored, or you want to change the   ##
##   name of the name of the executable, or if you want to change the    ##
##   compilation flags or linked libraries.                              ##
##                                                                       ##	
###########################################################################


# Is this a debug compile or not?
#   This can be set to ON using "make DEBUG=ON"
DEBUG = OFF

# The C++ compiler
CC = g++

# The directory to hold the generated executable
BINDIR = bin/

# The name of the executables (in case of optimized and debug compiles)
OPTEXE = $(EXENAME)
DBGEXE = $(EXENAME).debug

# Directories to search for includes and libraries
INCLUDES = -I. -I /usr/X11R6/include/ -I /group/graphics/include/

# Set the Flags passed to the compiler & linker. 
#   In the first case, we use optimizations.  If debugging is on,
#   we do not use optimizations, and we enable the profiler.
ifeq "$(DEBUG)" "OFF"
LDFLAGS = -L /usr/X11R6/lib64/ -L /usr/X11R6/lib/ -L /group/graphics/lib/
CCFLAGS = -O3 
EXE = $(OPTEXE)
else
LDFLAGS = -pg -L /usr/X11R6/lib64/ -L /usr/X11R6/lib/ -L /group/graphics/lib/
CCFLAGS = -g -pg -DDEBUG
EXE = $(DBGEXE)
endif

# The linked libraries
LIBS = -lGLEW -lglut -lGL -lGLU -lX11 -lXmu -lXi -lm -lrt


# A list of objects (replace .cpp with .o)
OBJS = $(SRC:.cpp=.o)

# A list of all the objects w/o their relative directory names
BASEOBJ = $(notdir $(OBJS))

# A list of the .o files, which are stored in obj/optimized/ or obj/debug/
ifeq "$(DEBUG)" "OFF"
FINALOBJ = $(addprefix bin/obj/optimized/,$(BASEOBJ))
else
FINALOBJ = $(addprefix bin/obj/debug/,$(BASEOBJ))
endif

# The default make rule
default: $(SRC) $(EXE)

debug:
	make DEBUG=ON

$(EXE): $(FINALOBJ) 
	$(CC) $(LDFLAGS) $(PROFILE) $(FINALOBJ) $(LIBS) -o $(BINDIR)$@

bin/obj/optimized/%.o: %.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDES) $< -o $@

bin/obj/debug/%.o: %.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f $(addprefix bin/obj/optimized/,$(BASEOBJ)) $(addprefix bin/obj/debug/,$(BASEOBJ)) $(BINDIR)$(OPTEXE) $(BINDIR)$(DBGEXE)

