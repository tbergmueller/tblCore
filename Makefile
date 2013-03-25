CC=g++
CFLAGS=-c -fPIC
AR=ar
ARFLAGS=rcs
OBJDIR=obj
OUTDIR=release
OUTFILE=libtbl.a
OUTDYNFILE=libtbl.so
RM=rm
MKDIR=mkdir


DC_HEADERFILE=DevCom.h

	



DCM_SRC_DIR=master
DCM_SRC_FILES=$(DCM_SRC_DIR)/dcm_basicFunctions.c $(DCM_SRC_DIR)/dcm_create.c $(DCM_SRC_DIR)/dcm_hardwareFunctions.c
DCM_OBJ_FILES=$(addprefix $(OBJDIR)/$(DCM_SRC_DIR)/,$(notdir $(DCM_SRC_FILES:.c=.o)))


M3S_SRC_DIR=m3s
M3S_SRC_FILES=$(M3S_SRC_DIR)/crc8.c $(M3S_SRC_DIR)/m3s_util.c $(M3S_SRC_DIR)/m3s.c
M3S_OBJ_FILES=$(addprefix $(OBJDIR)/$(M3S_SRC_DIR)/,$(notdir $(M3S_SRC_FILES:.c=.o)))





all: dcm dcs m3s

	@echo "~~~ Building static library..."
	@echo "Archive DevComMaster files.."
	@$(foreach objfile,$(DCM_OBJ_FILES), \
	$(AR) $(ARFLAGS) $(OUTDIR)/$(OUTFILE) $(objfile);)
	
	@echo "Archive M3S files.."
	@$(foreach objfile,$(M3S_OBJ_FILES), \
	$(AR) $(ARFLAGS) $(OUTDIR)/$(OUTFILE) $(objfile);)
	
	@echo "Copy headers..."
	@cp $(DC_HEADERFILE) $(OUTDIR)/$(DC_HEADERFILE)
	
	
	@echo "~~~ Building dynamic library..."
	$(CC) -shared -o $(OUTDIR)/$(OUTDYNFILE) $(M3S_OBJ_FILES) $(DCM_OBJ_FILES)
	
	@echo "=> Build library done"

	
clean:
	$(RM) -vfr $(OBJDIR)
	$(RM) -vfr $(OUTDIR)
	$(MKDIR) $(OBJDIR)
	$(MKDIR) $(OBJDIR)/$(DCM_SRC_DIR)
	$(MKDIR) $(OBJDIR)/$(M3S_SRC_DIR)
	$(MKDIR) $(OUTDIR)




	
dcm: $(DCM_OBJ_FILES)
	@echo "=> Build DevComMaster done"
	
dcs:
	@echo "============= WARNING: Dev Com Slave not in library yet ================ "
	
m3s: $(M3S_OBJ_FILES)
	@echo "=> Build M3S done"


$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

