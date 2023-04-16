CC		= gcc
CFLAGS	= -Wall -O2 -g
LDFLAGS	= -lglfw -lGLU -lGL -lm

BINDIR	= bin/
INCDIR	= inc/
SRCDIR	= src/
OBJDIR	= obj/

GLOB_OBJ_TD= $(OBJDIR)3D_tools.o

OBJ= $(GLOB_OBJ_TD) $(OBJDIR)draw_scene.o $(OBJDIR)main.o
EXEC= corridor.out


all : create_dir run 

create_dir : 
	mkdir $(BINDIR) 
	mkdir $(OBJDIR)

run : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BINDIR)$(EXEC) $(LDFLAGS)

clean :
	rm -rf *~
	rm -rf $(SRCDIR)*/*~
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)*

$(OBJDIR)%.o: $(SRCDIR)%.c $(INCDIR)*.h
	mkdir -p `dirname $@`
	$(CC) -o $@ -I $(INCDIR) -c $< $(CFLAGS)
