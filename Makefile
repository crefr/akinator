FILENAME = akinator.exe
OBJDIR 		   = Obj/
SRCDIR 		   = sources/
HEADDIR 	   = headers/

BINTREEOBJDIR  = binTree/Obj/
BINTREESRCDIR  = binTree/sources/
BINTREEHEADDIR = binTree/headers/

CC = g++
BUILD  = RELEASE
# windows
CFLAGS_WINDOWS =-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline						\
		-Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default					\
		-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy 				\
		-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers 	\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing 		\
		-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

# linux
CFLAGS_LINUX = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations 						\
		-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported 			\
		-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security 					\
		-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual 			\
		-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo 						\
		-Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods 					\
		-Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code 		\
		-Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing 		\
		-Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector 				\
		-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE	\
		-Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

# release
CFLAGS_RELEASE = -O3

ifeq ($(BUILD),WIN)
	CFLAGS = $(CFLAGS_WINDOWS)
else ifeq ($(BUILD),LINUX)
	CFLAGS = $(CFLAGS_LINUX)
else ifeq ($(BUILD),RELEASE)
	CFLAGS = $(CFLAGS_RELEASE)
endif

# CFLAGS_TEMP = $(CFLAGS)
CFLAGS := -I./$(HEADDIR) -I./$(BINTREEHEADDIR) $(CFLAGS)

ALLDEPS = $(BINTREEHEADDIR)bintree.h $(BINTREEHEADDIR)logger.h $(HEADDIR)akinator.h $(HEADDIR)logger.h
OBJECTS    = main.o logger.o akinator.o
BINTREEOBJ = bintree.o
OBJECTS_WITH_DIR 	 = $(addprefix $(OBJDIR),$(OBJECTS))
BINTREE_OBJ_WITH_DIR = $(addprefix $(BINTREEOBJDIR),$(BINTREEOBJ))

$(FILENAME): $(OBJECTS_WITH_DIR) $(BINTREE_OBJ_WITH_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJECTS_WITH_DIR): $(OBJDIR)%.o: $(SRCDIR)%.cpp $(ALLDEPS)
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINTREE_OBJ_WITH_DIR): $(BINTREEOBJDIR)%.o: $(BINTREESRCDIR)%.cpp $(ALLDEPS)
	mkdir -p $(BINTREEOBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJDIR)*

run:
	./$(FILENAME)